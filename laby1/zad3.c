#include <stdbool.h>
#include <stdio.h>

bool check_one(const void* ptr){
    int ptr_value = *(int*)ptr;
    if(ptr_value <= 126 && ptr_value >= 32){
        return true;
    }else{
        return false;
    }   
}

bool printable_buf(const void* ptr, int len){
        int* char_table = (int*)ptr;
        for(int i = 0 ; i < len; i++){
            if(!check_one(&char_table[i])){
                return false;
            }
        }
        return true;
}

int main(int argc, char** args){
    int table[] = {'c','a',67};
    int table2[] = {563, 'f', 'y'};
    bool git = printable_buf(table,3);
    bool niegit = printable_buf(table2, 3);
    printf("%d %d", git, niegit);
}