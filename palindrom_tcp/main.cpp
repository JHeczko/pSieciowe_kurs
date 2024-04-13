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
#include <ctype.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "validator.cpp"
#include "server.cpp"
#define ERROR -1
#define LISTENING 0
using namespace std;
//class Validator{
//private:
//    int licznik;
//    int palindrom;
//    int error;
//public:
//    Validator(){
//        licznik = 0;
//        palindrom = 0;
//        error = false;
//    }
//    std::string buildAnswear(){
//        std::string answear;
//        if(!error) {
//            answear = std::to_string(palindrom) + '/' + std::to_string(licznik) + "\r\n";
//        }else{
//            answear = "ERROR\r\n";
//        }
//        return answear;
//    }
//
//    bool validate(std::string str, int len){
//        int i = 0;
//        int newLine = false;
//        int spaceOnceOccured = false;
//        //Jesli pierwsza spacja to błąd
//        if(str[0] == ' '){
//            fprintf(stderr, "Space shouldnt be first\n");
//            error = true;
//            return false;
//        }
//        while(i < len){
//            // jesli '\n' sam bez konca lini to zle
//            if(str[i] == '\r'){
//                if(str[i+1] == '\n'){
//                    str[i] = ' ';
//                    str[i+1] = ' ';
//                    i+=2;
//                    newLine = true;
//                    continue;
//                }
//            }
//
//            if(str[i] == ' ' && newLine){
//                fprintf(stderr, "Space at beginning of new line\n");
//                newLine = false;
//                error = true;
//                return false;
//            }
//            //warunek sprawdzajacy biale znaki, ktore nie powinny sie pojawic, wiem ze tego nie bylo w poleceniu, ale tak poprostu to dodałem
//            if(str[i] == '\v' || str[i] == '\t' || str[i] == '\0' || str[i] == '\n' || (str[i] == '\\' & str[i+1] == 'x')){
//                fprintf(stderr, "Forbidden symbols\n");
//                error = true;
//                return false;
//            }
//            //warunek sprawdzajacy czy mamy wiecej niz jedna spacja
//            if(str[i] == ' '){
//                if(spaceOnceOccured){
//                    fprintf(stderr, "More than two spaces\n");
//                    error = true;
//                    return false;
//                }else{
//                    spaceOnceOccured = true;
//                }
//            }else{
//                spaceOnceOccured = false;
//            }
//            newLine = false;
//            i++;
//        }
//        //jesli na koncu mielismy spacje to dajemy blad
//        if(spaceOnceOccured){
//            fprintf(stderr, "Space at the end\n");
//            error = true;
//            return false;
//        }
//        return true;
//    }
//
//    bool checkPalindrome(char word[], int len){
//        licznik++;
//        for(int i = 0; i<(len/2); i++){
//            if(tolower(word[i]) != tolower(word[len-1-i])){
//                return false;
//            }
//        }
//        palindrom++;
//        return true;
//    }
//
//    std::vector<std::string> splitBySep(char* str, const char* sep){
//        char *ptr = strtok(str, sep);
//        std::vector<std::string> array;
//
//        do {
//            if(ptr != NULL) {
//                array.push_back(ptr);
//            }else{
//                array.push_back("");
//            }
//            ptr = strtok(NULL, sep);
//        }while (ptr != NULL);
//        return array;
//    }
//
//    void checkRequest(std::string req){
//        if(error){
//            return;
//        }
//        char* str = new char[req.length()];
//        strcpy(str,req.c_str());
//        char *ptr = strtok(str, " ");
//        while (ptr != NULL) {
//            checkPalindrome(ptr, strlen(ptr));
//            ptr = strtok(NULL, " ");
//        }
//        delete[] str;
//    }
//
//    void resetValidator(){
//        this->licznik = 0;
//        this->palindrom = 0;
//        this->error = false;
//    }
//protected:
//
//};
//
//class Server{
//
//private:
//    int status;
//    int bufSize;
//    int listenSocketDescriptor;
//    in_port_t port;
//    Validator validator;
//
//public:
//    Server(in_port_t port, Validator validator){
//        this->port = port;
//        this->bufSize = 2000;
//        this->validator = validator;
//        int listenSocketDescriptor = startListening(port);
//        if(listenSocketDescriptor == -1){
//            this->status = ERROR;
//            this->listenSocketDescriptor = -1;
//            log_perror("Server error while trying to initialize");
//        }else{
//            this->status = LISTENING;
//            this->listenSocketDescriptor = listenSocketDescriptor;
//            log_printf("Server Initialized succesfully");
//        };
//    }
//    Server(in_port_t port, int bufSize, Validator validator){
//        this->port = port;
//        this->bufSize = bufSize;
//        this->validator = validator;
//        int listenSocketDescriptor = startListening(port);
//        if(listenSocketDescriptor == -1){
//            this->status = ERROR;
//            this->listenSocketDescriptor = -1;
//            log_perror("Server error while trying to initialize");
//        }else{
//            this->status = LISTENING;
//            this->listenSocketDescriptor = listenSocketDescriptor;
//            log_printf("Server Initialized succesfully");
//        };
//    }
//    void acceptLoop()
//    {
//        if(status == ERROR){
//            log_perror("Server has error status");
//            return;
//        }
//
//        //Inicjalizacja zmiennych
//        fd_set selectSocketSet;    // zbiór deskryptorów otwartych gniazdek
//        int max_fd;         // największy użyty numer deskryptora
//        char* buf = new char[bufSize+1];     // bufor do zczytywania
//        std::unordered_map<int,std::string> dataNotComplete;
//
//        // na początku zbiór zawiera tylko gniazdko odbierające połączenia
//        FD_ZERO(&selectSocketSet);
//        FD_SET(listenSocketDescriptor, &selectSocketSet);
//        max_fd = listenSocketDescriptor;
//
//        while (true) {
//            log_printf("calling select()");
//            // select() modyfikuje zawartość przekazanego mu zbioru, zostaną
//            // w nim tylko deskryptory mające gotowe do odczytu dane
//            fd_set readReadySelectSocketSet = selectSocketSet;
//            int num = select(max_fd + 1, &readReadySelectSocketSet, NULL, NULL, NULL);
//            if (num == -1) {
//                log_perror("select");
//                break;
//            }
//            log_printf("number of ready to read descriptors = %i", num);
//            for (int fd = 0; fd <= max_fd; ++fd) {
//
//                if (! FD_ISSET(fd, &readReadySelectSocketSet)) {
//                    continue;
//                }
//
//                if (fd == listenSocketDescriptor){
//                    int s = acceptClient(listenSocketDescriptor);
//                    if (s == -1) {
//                        goto break_out_of_main_loop;
//                    } else if (s >= FD_SETSIZE){
//                        log_printf("%i cannot be added to a fd_set", s);
//                        // tego klienta nie da się obsłużyć, więc zamknij gniazdko
//                        closeSocket(s);
//                        continue;
//                    }
//                    FD_SET(s, &selectSocketSet);
//                    if (s > max_fd) {
//                        max_fd = s;
//                    }
//                } else {    // fd != srv_sock
//                    int read = readFromSocket(fd,buf,bufSize);
//
//                    if (read <= 0) { // druga strona zamknęła połączenie lub wystąpił błąd
//                        FD_CLR(fd, &selectSocketSet);
//                        closeSocket(fd);
//                        continue;
//                    } else{ // tworzymy stringa
//                        buf[read] = '\0';
//                    }
//
//                    std::string input;
//                    std::vector<std::string> dataWithoutWhiteSymbols = validator.splitBySep(buf, "\r\n");
//                    // dodawanie niedokończonych danych do bufora do późniejszej obródbki
//                    if (read == bufSize && buf[bufSize - 1] != '\n' && buf[bufSize - 2] != '\r') {
//                        if (dataNotComplete.find(fd) == dataNotComplete.end()) {
//                            dataNotComplete[fd] = dataWithoutWhiteSymbols.back();
//                        } else {
//                            dataNotComplete[fd] += dataWithoutWhiteSymbols.back();
//                        }
//                        dataWithoutWhiteSymbols.pop_back();
//                    } else{
//                        std::string temp = dataWithoutWhiteSymbols[0];
//                        dataWithoutWhiteSymbols.at(0) = dataNotComplete[fd]+temp;
//                        dataNotComplete.erase(fd);
//                    }
//
//                    for(std::string& el : dataWithoutWhiteSymbols){
//                        validator.validate(el,el.length());
//                        validator.checkRequest(el);
//
//
//                        std::string output = validator.buildAnswear();
//                        int write = writeToSocket(fd,(void *)output.c_str(),output.length());
//                        validator.resetValidator();
//                        if(write == -1){
//                            log_perror("Write");
//                            FD_CLR(fd, &selectSocketSet);
//                            close(fd);
//                        }
//                    }
//                }
//            }
//        }
//
//        break_out_of_main_loop:
//        // zamknij wszystkie połączenia z klientami
//        for (int fd = 0; fd <= max_fd; ++fd) {
//            if (FD_ISSET(fd, &selectSocketSet) && fd != listenSocketDescriptor) {
//                closeSocket(fd);
//            }
//        }
//        delete[] buf;
//    }
//    int getStatus(){
//        return status;
//    }
//    ~Server(){
//        close(listenSocketDescriptor);
//    }
//protected:
//    void log_error(const char * msg, int errnum)
//    {
//        log_printf("%s: %s", msg, strerror(errnum));
//    }
//    void log_perror(const char * msg)
//    {
//        log_printf("%s: %s", msg, strerror(errno));
//    }
//    void log_printf(const char * fmt, ...)
//    {
//        // bufor na przyrostowo budowany komunikat, len mówi ile już znaków ma
//        char buf[1024];
//        int len = 0;
//
//        struct timespec date_unix;
//        struct tm date_human;
//        long int usec;
//        if (clock_gettime(CLOCK_REALTIME, &date_unix) == -1) {
//            perror("clock_gettime");
//            return;
//        }
//        if (localtime_r(&date_unix.tv_sec, &date_human) == NULL) {
//            perror("localtime_r");
//            return;
//        }
//        usec = date_unix.tv_nsec / 1000;
//
//        // getpid() i gettid() zawsze wykonują się bezbłędnie
//        pid_t pid = getpid();
//
//        len = snprintf(buf, sizeof(buf), "%02i:%02i:%02i.%06li PID=%ji ",
//                       date_human.tm_hour, date_human.tm_min, date_human.tm_sec,
//                       usec, (intmax_t) pid);
//        if (len < 0) {
//            return;
//        }
//
//        va_list ap;
//        va_start(ap, fmt);
//        int i = vsnprintf(buf + len, sizeof(buf) - len, fmt, ap);
//        va_end(ap);
//        if (i < 0) {
//            return;
//        }
//        len += i;
//
//        // zamień \0 kończące łańcuch na \n i wyślij całość na stdout, czyli na
//        // deskryptor nr 1; bez obsługi błędów aby nie komplikować przykładu
//        buf[len] = '\n';
//        write(1, buf, len + 1);
//    }
//    int startListening(in_port_t port)
//    {
//        int s;
//
//        if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
//            perror("socket");
//            return -1;
//        }
//
//        struct sockaddr_in a;
//        a.sin_family = AF_INET;
//        a.sin_addr.s_addr = htonl(INADDR_ANY);
//        a.sin_port = htons(port);
//
//        if (bind(s, (struct sockaddr *) &a, sizeof(a)) == -1) {
//            perror("bind");
//            goto close_and_fail;
//        }
//
//        if (listen(s, 10) == -1) {
//            perror("listen");
//            goto close_and_fail;
//        }
//
//        return s;
//
//        close_and_fail:
//        close(s);
//        return -1;
//    }
//    ssize_t readFromSocket(int fd, void * buf, size_t nbytes)
//    {
//        log_printf("calling read() on descriptor %i", fd);
//        ssize_t rv = read(fd, buf, nbytes);
//        if (rv == -1) {
//            log_perror("read");
//        } else {
//            log_printf("received %zi bytes on descriptor %i", rv, fd);
//        }
//        return rv;
//    }
//    ssize_t writeToSocket(int fd, void * buf, int nbytes)
//    {
//        log_printf("calling write() on descriptor %i", fd);
//        int rv = write(fd, buf, nbytes);
//        if (rv == -1) {
//            log_perror("write");
//        } else if (rv < nbytes) {
//            log_printf("partial write on %i, wrote only %zi of %zu bytes",
//                       fd, rv, nbytes);
//        } else {
//            log_printf("wrote %zi bytes on descriptor %i", rv, fd);
//        }
//        return rv;
//    }
//    int acceptClient(int srv_sock)
//    {
//        struct sockaddr_in a;
//        socklen_t a_len = sizeof(a);
//
//        log_printf("calling accept() on descriptor %i", srv_sock);
//        int rv = accept(srv_sock, (struct sockaddr *) &a, &a_len);
//        if (rv == -1) {
//            log_perror("accept");
//        } else {
//            char buf[INET_ADDRSTRLEN];
//            if (inet_ntop(AF_INET, &a.sin_addr, buf, sizeof(buf)) == NULL) {
//                log_perror("inet_ntop");
//                strcpy(buf, "???");
//            }
//            log_printf("new client %s:%u, new descriptor %i",
//                       buf, (unsigned int) ntohs(a.sin_port), rv);
//        }
//        return rv;
//    }
//    int closeSocket(int fd)
//    {
//        log_printf("closing descriptor %i", fd);
//        int rv = close(fd);
//        if (rv == -1) {
//            log_perror("close");
//        }
//        return rv;
//    }
//};

int main(int argc, char** argv){
    Validator& validator = *(new Validator());
    Server& server = *(new Server(2020,2000,validator));
    if(server.getStatus() != ERROR){
       server.acceptLoop();
    }else{
        return -1;
    }
//    Validator validator;
//    std::vector<std::string> vector = validator.splitBySep("\r\nghjjghjg");
//    cout << validator.validateReq("\r\nghjjghjg");
//    for(auto& el : vector){
//        cout << el << endl;
//    }
    return 0;
}
