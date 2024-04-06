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
#include <sys/types.h>

int licznik = 0;
int palindrom = 0;

char* buildAnswear(){
    char* answear;
    asprintf(&answear, "%d/%d\n", palindrom,licznik);
    return answear;
}

bool validate(char str[]){
    int i = 0;
    int spaceOnceOccured = false;
    while(str[i] != '\0'){
        if(str[i] == '\v' || str[i] == '\t'){
           return false;
        }else if(str[i] == ' '){
            if(spaceOnceOccured){
                return false;
            }else{
                spaceOnceOccured = true;
            }
        }else{
            spaceOnceOccured = false;
        }
        i++;
    }
    return true;
}

bool checkPalindrome(char word[], int len){
    licznik++;
    for(int i = 0; i<(len/2); i++){
        if(word[i] != word[len-1-i]){
            return false;
        }
    }
    palindrom++;
    return true;
}

void strokeSplit(char* str){
    char *ptr = strtok(str, " ");

    while(ptr != NULL)
    {
        printf("%d",checkPalindrome(ptr,strlen(ptr)));
        printf("%s\n", ptr);
        ptr = strtok(NULL, " ");
    }
}

int main(){
    char string[50] = "czm oto\n";
    printf("%d\n", validate(string));
    strokeSplit(string);
    printf("%s", buildAnswear());
}