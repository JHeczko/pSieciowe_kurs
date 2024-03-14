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

int main(int argc, char** argv){
    int PORT = atoi(argv[2]);

    int listenSocketDesctiption = socket(AF_INET, SOCK_DGRAM, 0);
    if(listenSocketDesctiption == -1){
        perror(sys_errlist[errno]);
    }

    struct sockaddr_in serverAdress = {
            .sin_addr.s_addr = INADDR_ANY,
            .sin_port = htons(PORT),
            .sin_family = AF_INET};

    int bindReturn = bind(listenSocketDesctiption, (const struct sockaddr*)&serverAdress, sizeof(serverAdress));
    if(bindReturn == -1){
        perror(sys_errlist[errno]);
    }

    //ssize_t recvfrom(int socket, void *restrict buffer, size_t length,
    //       int flags, struct sockaddr *restrict address,
    //       socklen_t *restrict address_len);

    struct sockaddr_in adressOfClient;
    memset(&adressOfClient, 0 ,sizeof(adressOfClient));
    socklen_t lenAdressOfClient;
    char input[30];
    char* output = "Witam jestem serwer!";
    while(1) {
        int rcvValue = recvfrom(listenSocketDesctiption, (char *) input, sizeof(input), 0, (struct sockaddr *) &adressOfClient, &lenAdressOfClient);
        if (rcvValue == -1) {
            perror(sys_errlist[errno]);
            return -1;
        };
        int sendValue = sendto(listenSocketDesctiption, output, strlen(output), 0, (struct sockaddr *) &adressOfClient,
                               lenAdressOfClient);
        if (sendValue == -1) {
            perror(sys_errlist[errno]);
            return -1;
        }

        fprintf(stdout,"Dostałem wiadomość: %s\n", input);
        fflush(stdout);
    }
}