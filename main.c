#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Richard Gajdosik, WSL Ubuntu, IDE = Clion
// gcc 9.3

// Returns number if the given string contains a number and nothing but number chars, otherwise returns -1
int check_if_number(char string[]){
    char *ptr;
    int number = 0, i = 0;
    // We just check if everything in string is between 9 and 0 chars
    while(string[i] != '\0'){
        if(string[i] <= '9' && string[i] >= '0'){
        } else {
            fprintf(stderr, "One of the given arguments is not an integer!\n");
            return -1;
        }
        i++;
    }
    // Conversion
    number = strtol(string, &ptr, 0);
    return number;
}
int main(int argc, char *argv[]) {
    char string[1001];
    int flag_if_number = 0, arguments_values[4] = {0};
    // We check wheter the number of arguments is correct
    if(argc != 5){
        fprintf(stderr, "The number of arguments is incorrect!\n");
        return -1;
    }
    // We load and convert arguments from char arrays to integer arrays
    for(int i = 1; i < argc; i++){
        strcpy(string, argv[i]);
        flag_if_number = check_if_number(string);
        if(flag_if_number == -1){
            return -1;
        }
        arguments_values[i-1] = flag_if_number;
    }

//    for(int i = 0; i < 4; i++) {
//        printf("%d\n", arguments_values[i]);
//    }
    return 0;
}
