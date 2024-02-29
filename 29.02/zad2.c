#include <stdlib.h>
#include <stdio.h>

void drukuj(int tablica[], int size_tablica){
    for(int i = 0; i < size_tablica; i++){
        if(tablica[i] < 100 && tablica[i] > 10){
            printf("%d\n", *(tablica++));
        }
    }
}

int main(int argc, char** args){
    int liczby[50];
    int licznik ;
    
    for(int i = 0; i < 50; i++){
        printf("Podaj liczbe: \n");
        int buf;
        scanf("%d", &buf);
        if(buf == 0){
            printf("Wychodze\n");
            break;
        }else{
            liczby[i] = buf;
            licznik++;
        }
    }

    drukuj(liczby, licznik);


}