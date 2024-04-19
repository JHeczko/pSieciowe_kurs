//
// Created by Jakub Heczko on 06/04/2024.
//
#pragma once

#define _GNU_SOURCE
#include <iostream>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <ctype.h>
#include <cstdio>
#include <string>

class Validator{
private:
    int licznik;
    int palindrom;
    int error;
public:
    Validator(){
        licznik = 0;
        palindrom = 0;
        error = false;
    }
   std::string buildAnswear(){
        std::string answear;
        if(!error) {
            answear = std::to_string(palindrom) + '/' + std::to_string(licznik) + "\r\n";
        }else{
            answear = "ERROR\r\n";
        }
        return answear;
    }

    bool isError(){
        return error;
    }

    bool validate(std::string str, int len){
        int newLine = false;
        int spaceOnceOccured = false;
        if(error){
            return false;
        }

        //Jesli pierwsza spacja to błąd
        if(str[0] == ' '){
            fprintf(stderr, "Space shouldnt be first\n");
            error = true;
            return false;
        }
        for(int i = 0; i<len; i++){
            // jesli '\n' sam bez konca lini to zle
            if(str[i] == '\r'){
                if(str[i+1] == '\n'){
                    i+=1;
                    newLine = true;
                    continue;
                }
            }

            if(str[i] == ' ' && newLine){
                fprintf(stderr, "Space at beginning of new line\n");
                newLine = false;
                error = true;
                return false;
            }
            //warunek sprawdzajacy biale znaki, ktore nie powinny sie pojawic, wiem ze tego nie bylo w poleceniu, ale tak poprostu to dodałem
            if(str[i] == '\v' || str[i] == '\t' || str[i] == '\0' || str[i] == '\n' || str[i] == '\r'){
                fprintf(stderr, "Forbidden symbols\n");
                error = true;
                return false;
            }
            //warunek sprawdzajacy czy mamy wiecej niz jedna spacja
            if(str[i] == ' '){
                if(spaceOnceOccured){
                    fprintf(stderr, "More than two spaces\n");
                    error = true;
                    return false;
                }else{
                    spaceOnceOccured = true;
                }
            }else{
                spaceOnceOccured = false;
            }
            newLine = false;
            if(!((str[i] >= 65 && str[i] <= 90))){
                if(!(str[i] >= 97 && str[i] <= 122)) {
                    if(str[i] != 32){
                        fprintf(stderr, "Not palidnorme symbol\n");
                        error = true;
                        return false;
                    }
                }
            }
        }
        //jesli na koncu mielismy spacje to dajemy blad
        if(spaceOnceOccured){
            fprintf(stderr, "Space at the end\n");
            error = true;
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

    //char* str, const char* sep
    std::vector<std::string> splitBySep(std::string data){
        std::vector<std::string> array;

        std::string buf = "";
        for(size_t i = 0; i < data.size(); ++i){
            if(data[i] == '\r' && data[i+1] == '\n'){
                array.push_back(buf);
                buf = "";
                i++;
                continue;
            }
            buf += data[i];
        }
        if(buf != "") {
            array.push_back(buf);
        }
        return array;
    }

    int countWholeReq(char* data, int sizeData){
        int licznik = 0;
        for(size_t i = 0; i < sizeData; i++){
            if(data[i] == '\0'){
                fprintf(stderr, "Zero symbol in beetwen\n");
                error = true;
                return -1;
            }
            if(data[i] == '\r'){
                if(data[i+1] == '\n'){
                    licznik++;
                    i++;
                }else{
                    error = true;
                    return -1;
                }
            }
        }
        return licznik;
    }

    void checkForPalindrome(std::string req){
        if(error){
            return;
        }
        char* str = new char[req.length()];
        strcpy(str,req.c_str());
        char *ptr = strtok(str, " ");
        while (ptr != NULL) {
            checkPalindrome(ptr, strlen(ptr));
            ptr = strtok(NULL, " ");
        }
        delete[] str;
    }

    void resetValidator(){
        this->licznik = 0;
        this->palindrom = 0;
        this->error = false;
    }
protected:

};
