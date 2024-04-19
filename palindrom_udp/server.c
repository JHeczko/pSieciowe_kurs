#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>

int licznik = 0;
int palindrom = 0;
int ERROR = false;

char* buildAnswear(){
    char* answear;
    asprintf(&answear, "%d/%d\n", palindrom,licznik);
    return answear;
}

bool validate(char str[], int len){
    int i = 0;
    int newLine = false;
    int spaceOnceOccured = false;
    //Jesli pierwsza spacja to błąd
    if(str[0] == ' '){
        fprintf(stderr, "Space shouldnt be first\n");
        return false;
    }
    while(i < len){
        // jesli '\n' sam bez konca lini to zle
        if(str[i] == '\r'){
           if(str[i+1] == '\n'){
               str[i] = ' ';
               str[i+1] = ' ';
               i+=2;
               newLine = true;
               continue;
           }
        }

        if(str[i] == ' ' && newLine){
            fprintf(stderr, "Space at beginning of new line\n");
            newLine = false;
            return false;
        }
        //warunek sprawdzajacy biale znaki, ktore nie powinny sie pojawic, wiem ze tego nie bylo w poleceniu, ale tak poprostu to dodałem
        if(str[i] == '\v' || str[i] == '\t' || str[i] == '\0' || str[i] == '\n'){
            fprintf(stderr, "Forbidden symbols\n");
            return false;
        }
        //warunek sprawdzajacy czy mamy wiecej niz jedna spacja
        if(str[i] == ' '){
            if(spaceOnceOccured){
                fprintf(stderr, "More than two spaces\n");
                return false;
            }else{
                spaceOnceOccured = true;
            }
        }else{
            spaceOnceOccured = false;
        }
        newLine = false;
        i++;
    }
    //jesli na koncu mielismy spacje to dajemy blad
    if(spaceOnceOccured){
        fprintf(stderr, "Space at the end\n");
        return false;
    }
    return true;
}

bool checkPalindrome(char word[], int len){
    licznik++;
    for(int i = 0; i<(len/2); i++){
        if(tolower(word[i]) != tolower(word[len-1-i])){
            return false;
        }
    }
    palindrom++;
    return true;
}

void splitAndCheck(char* str){
    char *ptr = strtok(str, " ");
    while (ptr != NULL) {
        printf("%d: ", checkPalindrome(ptr, strlen(ptr)));
        printf("%s\n", ptr);
        ptr = strtok(NULL, " ");
    }
    printf("%d/%d\n", palindrom,licznik);
}

int main(int argc, char** argv){
    int PORT;
    if(argc != 1) {
        PORT = atoi(argv[1]);
    }else{
        PORT = 2020;
    }

    int listenSocketDesctiption = socket(AF_INET6, SOCK_DGRAM, 0);
    if(listenSocketDesctiption == -1){
        perror("Socket: ");
    }

    struct sockaddr_in serverAdress = {
            .sin_addr.s_addr = INADDR_ANY,
            .sin_port = htons(PORT),
            .sin_family = AF_INET6
    };

    int no = 0;
    setsockopt(listenSocketDesctiption, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no));

    int bindReturn = bind(listenSocketDesctiption, (const struct sockaddr*)&serverAdress, sizeof(serverAdress));
    if(bindReturn == -1){
        perror("Bind: ");
    }

    //ssize_t recvfrom(int socket, void *restrict buffer, size_t length,
    //       int flags, struct sockaddr *restrict address,
    //       socklen_t *restrict address_len);

    struct sockaddr_in adressOfClient;
    memset(&adressOfClient, 0 ,sizeof(adressOfClient));
    socklen_t lenAdressOfClient = sizeof(adressOfClient);

    // Can handle ~64Kb input
    char input[63000];

    char* output;

    while(true){
        licznik = 0;
        palindrom = 0;
        ERROR = false;

        int rcvValue = recvfrom(listenSocketDesctiption, (char *) input, sizeof(input), 0, (struct sockaddr *) &adressOfClient, &lenAdressOfClient);
        if (rcvValue == -1) {
            perror("Receive: ");
            return -1;
        };
        if(rcvValue != 63000){
            input[rcvValue] = '\0';
        }

        if(!validate(input, rcvValue)){
            output = "ERROR\n";
            ERROR = true;
        }else{
            //rozdzielam i sprawdzam czy slowa sa palindormami i zmieniam stan zmiennej globalnej
            splitAndCheck(input);
            //buduje stringa odpowiedz, na podstawie ile mam palindomow
            output = buildAnswear();
        };

        int sendValue = sendto(listenSocketDesctiption, output, strlen(output), 0, (struct sockaddr *) &adressOfClient,
                               lenAdressOfClient);
        if (sendValue == -1) {
            perror("Send: ");
            return -1;
        }

        //zwalniam pamiec przydzielona za pomoca asprintf() w buildAnswear()
        if(!ERROR) {
            free(output);
        }
    }
}