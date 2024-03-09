#include <stdbool.h>
#include <stdio.h>

bool check_one(const int ptr_value){
    if(ptr_value <= 126 && ptr_value >= 32){
        return true;
    }else{
        return false;
    }
}

bool printable_buf(const char* ptr){
    char* buf = ptr;
    while(*buf != '\0'){
        if(!check_one(*buf)){
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