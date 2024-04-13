#pragma once

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include "validator.cpp"
#include <ctype.h>


#define ERROR -1
#define LISTENING 0

class Server{

private:
    int status;
    int bufSize;
    int listenSocketDescriptor;
    in_port_t port;
    Validator validator;

public:
    Server(in_port_t port, Validator validator){
        this->port = port;
        this->bufSize = 2000;
        this->validator = validator;
        int listenSocketDescriptor = startListening(port);
        if(listenSocketDescriptor == -1){
            this->status = ERROR;
            this->listenSocketDescriptor = -1;
            log_perror("Server error while trying to initialize");
        }else{
            this->status = LISTENING;
            this->listenSocketDescriptor = listenSocketDescriptor;
            log_printf("Server Initialized succesfully");
        };
    }
    Server(in_port_t port, int bufSize, Validator validator){
        this->port = port;
        this->bufSize = bufSize;
        this->validator = validator;
        int listenSocketDescriptor = startListening(port);
        if(listenSocketDescriptor == -1){
            this->status = ERROR;
            this->listenSocketDescriptor = -1;
            log_perror("Server error while trying to initialize");
        }else{
            this->status = LISTENING;
            this->listenSocketDescriptor = listenSocketDescriptor;
            log_printf("Server Initialized succesfully");
        };
    }
    void acceptLoop()
    {
        if(status == ERROR){
            log_perror("Server has error status");
            return;
        }

        //Inicjalizacja zmiennych
        fd_set selectSocketSet;    // zbiór deskryptorów otwartych gniazdek
        int max_fd;         // największy użyty numer deskryptora
        char* buf = new char[bufSize+1];     // bufor do zczytywania
        std::unordered_map<int,std::string> dataNotComplete;

        // na początku zbiór zawiera tylko gniazdko odbierające połączenia
        FD_ZERO(&selectSocketSet);
        FD_SET(listenSocketDescriptor, &selectSocketSet);
        max_fd = listenSocketDescriptor;

        while (true) {
            log_printf("calling select()");
            // select() modyfikuje zawartość przekazanego mu zbioru, zostaną
            // w nim tylko deskryptory mające gotowe do odczytu dane
            fd_set readReadySelectSocketSet = selectSocketSet;
            int num = select(max_fd + 1, &readReadySelectSocketSet, NULL, NULL, NULL);
            if (num == -1) {
                log_perror("select");
                break;
            }
            log_printf("number of ready to read descriptors = %i", num);
            for (int fd = 0; fd <= max_fd; ++fd) {

                if (! FD_ISSET(fd, &readReadySelectSocketSet)) {
                    continue;
                }

                if (fd == listenSocketDescriptor){
                    int s = acceptClient(listenSocketDescriptor);
                    if (s == -1) {
                        goto break_out_of_main_loop;
                    } else if (s >= FD_SETSIZE){
                        log_printf("%i cannot be added to a fd_set", s);
                        // tego klienta nie da się obsłużyć, więc zamknij gniazdko
                        closeSocket(s);
                        continue;
                    }
                    FD_SET(s, &selectSocketSet);
                    if(dataNotComplete.find(s) != dataNotComplete.end()){
                        dataNotComplete.erase(s);
                    }
                    if (s > max_fd) {
                        max_fd = s;
                    }
                } else {    // fd != srv_sock
                    int read = readFromSocket(fd,buf,bufSize);

                    if (read <= 0) { // druga strona zamknęła połączenie lub wystąpił błąd
                        FD_CLR(fd, &selectSocketSet);
                        if(dataNotComplete.find(fd) != dataNotComplete.end()){
                            dataNotComplete.erase(fd);
                        }
                        closeSocket(fd);
                        continue;
                    } else{ // tworzymy stringa
                        buf[read] = '\0';
                    }

                    std::string input = buf;
                    int quantityReq = validator.validateReq(buf);

                    // wstepne sprawdzenie przed zrobieniem podzielenia na zapytania czy sa dobre separatory
                    if(validator.isError()){
                        std::string output = validator.buildAnswear();
                        int write = writeToSocket(fd,(void *)output.c_str(),output.length());
                        validator.resetValidator();
                        if(write == -1){
                            log_perror("Write");
                            FD_CLR(fd, &selectSocketSet);

                            if(dataNotComplete.find(fd) != dataNotComplete.end()){
                                dataNotComplete.erase(fd);
                            }
                            close(fd);
                        }
                    }

                    //tablica zapytan
                    std::vector<std::string> dataWithoutWhiteSymbols = validator.splitBySep(input);

                    // logika dodawania niedokonczonego zapytania dla jednego znaku, bo to trudno sprawdzic w jendym warunku
                    if(read == 1){
                        if (dataNotComplete.find(fd) == dataNotComplete.end()) {
                            dataNotComplete[fd] = dataWithoutWhiteSymbols.back();
                        } else {
                            dataNotComplete[fd] += dataWithoutWhiteSymbols.back();
                        }
                        continue;
                    }
                    // dodawanie niedokończonych danych do bufora do późniejszej obródbki
                    if (buf[read - 1] != '\n' && buf[read - 2] != '\r') {
                        if (dataNotComplete.find(fd) == dataNotComplete.end()) {
                            dataNotComplete[fd] = dataWithoutWhiteSymbols.back();
                        } else {
                            dataNotComplete[fd] += dataWithoutWhiteSymbols.back();
                        }
                        dataWithoutWhiteSymbols.pop_back();
                    } else{
                        std::string temp = dataWithoutWhiteSymbols[0];
                        dataWithoutWhiteSymbols.at(0) = dataNotComplete[fd]+temp;
                        dataNotComplete.erase(fd);
                    }

                    if(quantityReq > 0){
                        if (dataNotComplete.find(fd) != dataNotComplete.end()){
                            std::string temp = dataWithoutWhiteSymbols[0];
                            dataWithoutWhiteSymbols.at(0) = dataNotComplete[fd]+temp;
                            dataNotComplete.erase(fd);
                        }
                    }

                    // obrobka poszczegolnych zapytan i wysylanie odpowiedzi
                    for(std::string& el : dataWithoutWhiteSymbols){
                        validator.validate(el,el.length());
                        validator.checkRequest(el);


                        std::string output = validator.buildAnswear();
                        int write = writeToSocket(fd,(void *)output.c_str(),output.length());
                        validator.resetValidator();
                        if(write == -1){
                            log_perror("Write");
                            if(dataNotComplete.find(fd) != dataNotComplete.end()){
                                dataNotComplete.erase(fd);
                            }
                            FD_CLR(fd, &selectSocketSet);
                            close(fd);
                        }
                    }
                }
            }
        }

        break_out_of_main_loop:
        // zamknij wszystkie połączenia z klientami
        for (int fd = 0; fd <= max_fd; ++fd) {
            if (FD_ISSET(fd, &selectSocketSet) && fd != listenSocketDescriptor) {
                if(dataNotComplete.find(fd) != dataNotComplete.end()){
                    dataNotComplete.erase(fd);
                }
                closeSocket(fd);
            }
        }
        delete[] buf;
    }
    int getStatus(){
        return status;
    }
    ~Server(){
        close(listenSocketDescriptor);
    }
protected:
    void log_error(const char * msg, int errnum)
    {
        log_printf("%s: %s", msg, strerror(errnum));
    }
    void log_perror(const char * msg)
    {
        log_printf("%s: %s", msg, strerror(errno));
    }
    void log_printf(const char * fmt, ...)
    {
        // bufor na przyrostowo budowany komunikat, len mówi ile już znaków ma
        char buf[1024];
        int len = 0;

        struct timespec date_unix;
        struct tm date_human;
        long int usec;
        if (clock_gettime(CLOCK_REALTIME, &date_unix) == -1) {
            perror("clock_gettime");
            return;
        }
        if (localtime_r(&date_unix.tv_sec, &date_human) == NULL) {
            perror("localtime_r");
            return;
        }
        usec = date_unix.tv_nsec / 1000;

        // getpid() i gettid() zawsze wykonują się bezbłędnie
        pid_t pid = getpid();

        len = snprintf(buf, sizeof(buf), "%02i:%02i:%02i.%06li PID=%ji ",
                       date_human.tm_hour, date_human.tm_min, date_human.tm_sec,
                       usec, (intmax_t) pid);
        if (len < 0) {
            return;
        }

        va_list ap;
        va_start(ap, fmt);
        int i = vsnprintf(buf + len, sizeof(buf) - len, fmt, ap);
        va_end(ap);
        if (i < 0) {
            return;
        }
        len += i;

        // zamień \0 kończące łańcuch na \n i wyślij całość na stdout, czyli na
        // deskryptor nr 1; bez obsługi błędów aby nie komplikować przykładu
        buf[len] = '\n';
        write(1, buf, len + 1);
    }
    int startListening(in_port_t port)
    {
        int s;

        if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            return -1;
        }

        struct sockaddr_in a;
        a.sin_family = AF_INET;
        a.sin_addr.s_addr = htonl(INADDR_ANY);
        a.sin_port = htons(port);

        if (bind(s, (struct sockaddr *) &a, sizeof(a)) == -1) {
            perror("bind");
            goto close_and_fail;
        }

        if (listen(s, 10) == -1) {
            perror("listen");
            goto close_and_fail;
        }

        return s;

        close_and_fail:
        close(s);
        return -1;
    }
    ssize_t readFromSocket(int fd, void * buf, size_t nbytes)
    {
        log_printf("calling read() on descriptor %i", fd);
        ssize_t rv = read(fd, buf, nbytes);
        if (rv == -1) {
            log_perror("read");
        } else {
            log_printf("received %zi bytes on descriptor %i", rv, fd);
        }
        return rv;
    }
    ssize_t writeToSocket(int fd, void * buf, int nbytes)
    {
        log_printf("calling write() on descriptor %i", fd);
        int rv = write(fd, buf, nbytes);
        if (rv == -1) {
            log_perror("write");
        } else if (rv < nbytes) {
            log_printf("partial write on %i, wrote only %zi of %zu bytes",
                       fd, rv, nbytes);
        } else {
            log_printf("wrote %zi bytes on descriptor %i", rv, fd);
        }
        return rv;
    }
    int acceptClient(int srv_sock)
    {
        struct sockaddr_in a;
        socklen_t a_len = sizeof(a);

        log_printf("calling accept() on descriptor %i", srv_sock);
        int rv = accept(srv_sock, (struct sockaddr *) &a, &a_len);
        if (rv == -1) {
            log_perror("accept");
        } else {
            char buf[INET_ADDRSTRLEN];
            if (inet_ntop(AF_INET, &a.sin_addr, buf, sizeof(buf)) == NULL) {
                log_perror("inet_ntop");
                strcpy(buf, "???");
            }
            log_printf("new client %s:%u, new descriptor %i",
                       buf, (unsigned int) ntohs(a.sin_port), rv);
        }
        return rv;
    }
    int closeSocket(int fd)
    {
        log_printf("closing descriptor %i", fd);
        int rv = close(fd);
        if (rv == -1) {
            log_perror("close");
        }
        return rv;
    }
};
