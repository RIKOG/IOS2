// Richard Gajdosik, WSL Ubuntu, IDE = Clion
// gcc 9.3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#define DEBUG
#define MMAP(pointer) {(pointer) = mmap(NULL, sizeof(*(pointer)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);}
#define mysleep(max_time) {if (max_time != 0) sleep(rand() % max_time);}
#define UNMAP(pointer) {munmap((pointer), sizeof((pointer)));}

// Returns number if the given string contains a number and nothing but number chars, otherwise returns -1
int check_if_number(char string[]){
    char *ptr;
    int number = 0, i = 0;
    // We just check if everything in string is between 9 and 0 chars
    while(string[i] != '\0'){
        // todo add support for +500 and -500
        if(string[i] <= '9' && string[i] >= '0'){
        } else {
            fprintf(stderr, "One of the given arguments is not an integer or has negative value!\n");
            return -1;
        }
        i++;
    }
    // Conversion
    number = strtol(string, &ptr, 0);
    return number;
}
int main(int argc, char *argv[]) {
    // Todo what if we get really big argument ?
    char string[1001];
    int flag_if_number = 0, arguments_values[4] = {0};
    // We check whether the number of arguments is correct
    if(argc != 5){
        fprintf(stderr, "The number of arguments is incorrect!\n");
        return -1;
    }
    // We load and convert arguments from char arrays to integer arrays
    for(int i = 1; i < argc; i++){
        strcpy(string, argv[i]);
        flag_if_number = check_if_number(string);
        // The function returned -1 meaning the string wasnt holding numeric chars
        if(flag_if_number == -1){
            return -1;
        }
        arguments_values[i-1] = flag_if_number;
    }
    // We check whether the arguments are of the right size
    if(
                (arguments_values[0] >= 1000 || arguments_values[0] <= 0)
            ||
                (arguments_values[1] >= 20 || arguments_values[1] <= 0)
            ||
                (arguments_values[2] > 1000 || arguments_values[2] < 0)
            ||
                (arguments_values[3] > 1000 || arguments_values[3] < 0)
            ){
        fprintf(stderr, "The size of arguments is incorrect!\n");
        return -1;
    }
    for(int i = 0; i < 4; i++) {
        printf("%d\n", arguments_values[i]);
    }
    return 0;
}
