#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/errno.h>


int main(int argc, char** argv){
    mode_t mode = O_RDWR;
    char buf[10];

    int in = open(argv[1], mode, 111111111);
    int out = open(argv[2],mode,111111111);
    if(in == -1 || out == -1){
        perror(sys_errlist[errno]);
    }

    int read_count;
    do{
        if((read_count = read(in,buf,10)) == -1){
            perror(sys_errlist[errno]);
            return -1;
        }

        if(write(out,buf,read_count) == -1){
            perror(sys_errlist[errno]);
            return -1;
        }
    }while(read_count != 0);
    close(in);
    close(out);
}