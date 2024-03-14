#include <stdbool.h>
#include <stdio.h>

bool printable_buf(const char* ptr){
    char* buf = ptr;
    while(*buf != '\0'){
        if(!(*buf <= 126 && *buf >= 32)){
            return false;
        }
        printf("%c", *buf);
        buf++;
    }
    return true;
}
int main(){
    const char slowo[5] = {'c','a','f','d','\0'};
    const char slowo2[5] = {'b','l','2','d','\0'};
    printf("%d\n", printable_buf(slowo));
    printf("%d", printable_buf(slowo2));
}