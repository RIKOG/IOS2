// Richard Gajdosik, WSL Ubuntu, IDE = Clion
// gcc 9.3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <limits.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#define max_time 1000
#define DEBUG
#define MMAP(pointer) {(pointer) = mmap(NULL, sizeof(*(pointer)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);}
#define mysleep(max_time) {if (max_time != 0) sleep(rand() % max_time);}
#define UNMAP(pointer) {munmap((pointer), sizeof((pointer)));}
//Todo poviem procesom nech sa zatvoria

//todo pridat writing semafor
int *sdilenaprom = NULL;
sem_t *semafor_elf;
sem_t *semafor_santa;
sem_t *semafor_santa_end;
sem_t *semafor_writing_incrementing;
// TRYWAIT CATCH ENDING FOR SANTA THROUGH THAT

FILE *fp;

void process_santa() {
    while (1) {
//        sem_wait(semafor_writing_incrementing);
//        fprintf(fp, "%d: Santa: going to sleep\n", *sdilenaprom);
//        (*sdilenaprom)++;
//        sem_post(semafor_writing_incrementing);

//        sem_wait(semafor_santa);

//        sem_wait(semafor_writing_incrementing);
//        fprintf(fp, "%d: Santa: helping elves\n", *sdilenaprom);
//        (*sdilenaprom)++;
//        sem_post(semafor_writing_incrementing);

//        sem_post(semafor_elf);
//        sem_post(semafor_elf);
//        sem_post(semafor_elf);
        exit(0);
    }
}

void process_elf(int elfID) {

    while (1) {
        sem_wait(semafor_writing_incrementing);
        fprintf(fp, "%d: Elf %d: started\n", *sdilenaprom, elfID);
        (*sdilenaprom)++;
        sem_post(semafor_writing_incrementing);
//        sem_wait(semafor_writing_incrementing);
        fprintf(fp, "%d: Elf %d: need help\n", *sdilenaprom, elfID);
        (*sdilenaprom)++;
//        sem_post(semafor_writing_incrementing);

        sem_wait(semafor_elf);

//        sem_wait(semafor_writing_incrementing);
        fprintf(fp, "%d: Elf %d: get help\n", *sdilenaprom, elfID);
        (*sdilenaprom)++;
//        sem_post(semafor_writing_incrementing);
        exit(0);
    }
}

int init_semaphores() {
    MMAP(sdilenaprom);
    sem_unlink("/xgajdo33.semafor_elf");
    sem_unlink("/xgajdo33.semafor_santa");
    sem_unlink("/xgajdo33.semafor_santa_end");
    sem_unlink("/xgajdo33.semafor_writing_incrementing");

    if ((semafor_elf = sem_open("/xgajdo33.semafor_elf", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {
        return -1;
    }
    if ((semafor_santa = sem_open("/xgajdo33.semafor_santa", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {
        return -1;
    }
    if ((semafor_santa_end = sem_open("/xgajdo33.semafor_santa_end", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {
        return -1;
    }
    if ((semafor_writing_incrementing = sem_open("/xgajdo33.semafor_writing_incrementing", O_CREAT | O_EXCL, 0644,
                                                 1)) == SEM_FAILED) {
        return -1;
    }
    return 0;
}

void clean_up() {
    UNMAP(sdilenaprom);
    sem_close(semafor_elf);
    sem_unlink("/xgajdo33.semafor_elf");
    sem_unlink("/xgajdo33.semafor_santa");
    sem_unlink("/xgajdo33.semafor_santa_end");
    sem_unlink("/xgajdo33.semafor_writing_incrementing");


    if (fclose(fp) == EOF) {
        fprintf(stderr, "Closing of the file failed!\n");
    }
}

// Returns number if the given string contains a number and nothing but number chars, otherwise returns -1
int check_if_number(char string[]) {
    char *ptr;
    int number = 0, i = 0;
    // We just check if everything in string is between 9 and 0 chars
    while (string[i] != '\0') {
        // todo add support for +500 and -500.. maybe even support for 5.2 decimals
        if (string[i] <= '9' && string[i] >= '0') {
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
    if ((fp = fopen("text.txt", "w+")) == NULL) {
        fprintf(stderr, "The file failed to open!\n");
        exit(-1);
    }
    if (init_semaphores() != 0) {
        fprintf(stderr, "Initialization of semaphores failed!\n");
        exit(-1);
    }
    // Todo what if we get really big argument ?
    char string[1001];
    int flag_if_number = 0, arguments_values[4] = {0};
    // We check whether the number of arguments is correct
    if (argc != 5) {
        fprintf(stderr, "The number of arguments is incorrect!\n");
        clean_up();
        exit(-1);
    }
    // We load and convert arguments from char arrays to integer arrays
    for (int i = 1; i < argc; i++) {
        strcpy(string, argv[i]);
        flag_if_number = check_if_number(string);
        // The function returned -1 meaning the string wasnt holding numeric chars
        if (flag_if_number == -1) {
            clean_up();
            return -1;
        }
        arguments_values[i - 1] = flag_if_number;
    }
    // We check whether the arguments are of the right size
    if (
            (arguments_values[0] >= 1000 || arguments_values[0] <= 0)
            ||
            (arguments_values[1] >= 20 || arguments_values[1] <= 0)
            ||
            (arguments_values[2] > 1000 || arguments_values[2] < 0)
            ||
            (arguments_values[3] > 1000 || arguments_values[3] < 0)
            ) {
        fprintf(stderr, "The size of arguments is incorrect!\n");
        clean_up();
        return -1;
    }
// Prints out values
    for (int i = 0; i < 4; i++) {
        printf("%d\n", arguments_values[i]);
    }

//    pid_t santa = fork();
//    if (santa == 0) {
//        process_santa();
//    }
    for (int i = 1; i < arguments_values[0]; i++) {
        printf("%d\n", i);
//            if(i%3 == 0){
//                sem_post(semafor_elf);
//                sem_post(semafor_elf);
//                sem_post(semafor_elf);
//            }
        pid_t elf = fork();
        if (elf == 0) {
            process_elf(i);
        }
//            sem_post(semafor_elf);
    }
    sem_post(semafor_elf);

    clean_up();
    return 0;
}
