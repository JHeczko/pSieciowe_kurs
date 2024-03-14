#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <stdbool.h>

bool printable_buf(const char* ptr, int len){
    char* buf = ptr;
    for(int i = 0; i<len; i++){
        if(!(*buf <= 126 && *buf >= 32)){
            return false;
        } else if(*buf == '\n' || *buf == '\r' || *buf == '\t'){
            return false;
        }
        buf++;
    }
    return true;
}

int main(int argc, char** argv){
    int socket_descriptor;
    socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_descriptor == -1){
        perror(sys_errlist[errno]);
    }

    struct sockaddr_in adress = {
            .sin_family = AF_INET,
            .sin_port = htons(atoi(argv[2])),
            .sin_addr = { .s_addr = inet_addr(argv[1])}
    };

    const char* messege_output = "";
    char messege_input[30];

//ssize_t sendto(int socket, const void *message, size_t length,
//       int flags, const struct sockaddr *dest_addr,
//       socklen_t dest_len);
//       ssize_t recv(int sockfd, void buf[.len], size_t len,
//       int flags);
        int send = sendto(socket_descriptor, (const char *) messege_output, 0, 0,
                          (const struct sockaddr *) &adress, sizeof(adress));
        if (send == -1) {
            perror(sys_errlist[errno]);
        };
        int receive = recvfrom(socket_descriptor, messege_input, sizeof(messege_input), 0, NULL, NULL);
        if (receive == -1) {
            perror(sys_errlist[errno]);
        };
        if(printable_buf(messege_input, receive) == false){
            perror("Not printable input");
        }
        printf("%s", messege_input);
}