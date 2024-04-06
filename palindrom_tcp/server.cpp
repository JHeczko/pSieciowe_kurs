#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

#define ERROR -1
#define LISTENING 0

class Server{

private:
    char buf[2000];
    int status;
    int listenSocketDescriptor;
    in_port_t port;

public:
    Server(in_port_t port){
        this->port = port;
        int listenSocketDescriptor = startListening(port);
        if(listenSocketDescriptor == -1){
            this->status = ERROR;
            this->listenSocketDescriptor = -1;
            log_printf("Server Initialized succesfully");
        }else{
            this->status = LISTENING;
            this->listenSocketDescriptor = listenSocketDescriptor;
            log_printf("Server error while trying to initialize");
        };
    }
    void select_loop(int srv_sock)
    {
        fd_set sock_fds;    // zbiór deskryptorów otwartych gniazdek
        int max_fd;         // największy użyty numer deskryptora

        // na początku zbiór zawiera tylko gniazdko odbierające połączenia
        FD_ZERO(&sock_fds);
        FD_SET(srv_sock, &sock_fds);
        max_fd = srv_sock;

        while (true) {
            log_printf("calling select()");
            // select() modyfikuje zawartość przekazanego mu zbioru, zostaną
            // w nim tylko deskryptory mające gotowe do odczytu dane
            fd_set read_ready_fds = sock_fds;
            int num = select(max_fd + 1, &read_ready_fds, NULL, NULL, NULL);
            if (num == -1) {
                log_perror("select");
                break;
            }
            log_printf("number of ready to read descriptors = %i", num);

            for (int fd = 0; fd <= max_fd; ++fd) {
                if (! FD_ISSET(fd, &read_ready_fds)) {
                    continue;
                }

                if (fd == srv_sock) {

                    int s = acceptClient(srv_sock);
                    if (s == -1) {
                        goto break_out_of_main_loop;
                    } else if (s >= FD_SETSIZE) {
                        log_printf("%i cannot be added to a fd_set", s);
                        // tego klienta nie da się obsłużyć, więc zamknij gniazdko
                        closeSocket(s);
                        continue;
                    }
                    FD_SET(s, &sock_fds);
                    if (s > max_fd) {
                        max_fd = s;
                    }

                } else {    // fd != srv_sock

                    if (readFromSocket(fd,buf, 2000) <= 0) {
                        // druga strona zamknęła połączenie lub wystąpił błąd
                        FD_CLR(fd, &sock_fds);
                        closeSocket(fd);
                    }

                }
            }
        }
        break_out_of_main_loop:

        // zamknij wszystkie połączenia z klientami
        for (int fd = 0; fd <= max_fd; ++fd) {
            if (FD_ISSET(fd, &sock_fds) && fd != srv_sock) {
                closeSocket(fd);
            }
        }
    }
    int getStatus(){
        return status;
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

        len = snprintf(buf, sizeof(buf), "%02i:%02i:%02i.%06li PID=%ji",
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

        struct sockaddr_in a = {
                .sin_family = AF_INET,
                .sin_addr.s_addr = htonl(INADDR_ANY),
                .sin_port = htons(port)
        };

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
    ssize_t writeToSocket(int fd, void * buf, size_t nbytes)
    {
        log_printf("calling write() on descriptor %i", fd);
        ssize_t rv = write(fd, buf, nbytes);
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
