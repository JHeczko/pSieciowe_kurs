#define _POSIX_C_SOURCE 200809L
#include "server.cpp"
#include "validator.cpp"

int main(int argc, char** argv){
   Server server = *(new Server(2020));
   if(server.getStatus() != ERROR){
       server.accept();
   }
}
