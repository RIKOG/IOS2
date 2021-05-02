// Richard Gajdosik, WSL Ubuntu, IDE = Clion
// gcc 9.3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define MMAP(pointer) {(pointer) = mmap(NULL, sizeof(*(pointer)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);}
#define UNMAP(pointer) {munmap((pointer), sizeof((pointer)));}
//Todo try catch states where fork didnt get created

// Inicialization of global variables
int *order_of_prints = NULL;
int *number_of_elves_waiting = NULL;
int *number_of_reindeers_waiting = NULL;
int *reindeers_ready_flag = NULL;
int *christmas_flag = NULL;
int *number_of_elves_working = NULL;

// Inicialization of semaphores
sem_t *semaphore_elf;
sem_t *semaphore_santa;
sem_t *semaphore_reindeer;
sem_t *semaphore_writing_incrementing;
sem_t *semaphore_working_shop;

// Inicialization of file pointers

FILE *fp, *random_generator;

void process_santa(int number_of_elves_total, int number_of_reindeers_total) {
    while (1) {
        // Santa got initialized we tell the .out text we are starting to sleep
        sem_wait(semaphore_writing_incrementing);
        fprintf(fp, "%d: Santa: going to sleep\n", *order_of_prints);
        fflush(fp);
        (*order_of_prints)++;
        sem_post(semaphore_writing_incrementing);

        // Waiting for reindeers or elves to wake me up
        sem_wait(semaphore_santa);
        // All the reindeers are back from vacation
        if ((*reindeers_ready_flag) == 1) {

            sem_wait(semaphore_writing_incrementing);
            fprintf(fp, "%d: Santa: closing workshop\n", *order_of_prints);
            fflush(fp);
            (*order_of_prints)++;
            sem_post(semaphore_writing_incrementing);

            // We hitch the reindeers
            for (int i = 0; i < number_of_reindeers_total; i++) {
                sem_post(semaphore_reindeer);
            }
            // We set global flag to 1 telling rest of the program santa wont be helping at the shop
            sem_wait(semaphore_writing_incrementing);
            (*christmas_flag) = 1;
            sem_post(semaphore_writing_incrementing);
            // We let the elves waiting infront of the shop to take their vacation, to make sure everything works we open the semaphore as many times as there are elves
            for (int i = 0; i < number_of_elves_total; i++) {
                sem_post(semaphore_elf);
            }
            exit(0);
        }
        // Otherwise we got woken up by elves, we start helping...
        sem_wait(semaphore_writing_incrementing);
        fprintf(fp, "%d: Santa: helping elves\n", *order_of_prints);
        fflush(fp);
        (*order_of_prints)++;
        (*number_of_elves_waiting) -= 3;
        // Global flag variable that counts down when we helped elve, elve process decrements it
        (*number_of_elves_working) = 3;
        sem_post(semaphore_writing_incrementing);
        // Tell 3 elve to start working
        sem_post(semaphore_elf);
        sem_post(semaphore_elf);
        sem_post(semaphore_elf);
        // Waiting until all of the 3 elves got help from santa, elves decrement (*number_of_elves_working) and if elve finds out he is a third one, he sends Santa from shop to bed
        sem_wait(semaphore_working_shop);
    }
}

void process_elf(int elfID, int wait_value) {
    unsigned int random_value;
    // Elve got initialized
    sem_wait(semaphore_writing_incrementing);
    fprintf(fp, "%d: Elf %d: started\n", *order_of_prints, elfID);
    fflush(fp);
    (*order_of_prints)++;
    sem_post(semaphore_writing_incrementing);

    fread(&random_value, sizeof(random_value), 1, random_generator);
    // Generating random numbers
    if(wait_value != 0){
        usleep(random_value % wait_value);
    }

    // Elve screams as he is left alone in front of the shop
    sem_wait(semaphore_writing_incrementing);
    fprintf(fp, "%d: Elf %d: need help\n", *order_of_prints, elfID);
    fflush(fp);
    (*order_of_prints)++;
    (*number_of_elves_waiting)++;
    sem_post(semaphore_writing_incrementing);

    // ... then he founds out its closed for now, and hes feeling like an idiot, going to work at holidays pff!
    if ((*christmas_flag) == 1) {
        sem_wait(semaphore_writing_incrementing);
        fprintf(fp, "%d: Elf %d: taking holidays\n", *order_of_prints, elfID);
        fflush(fp);
        (*order_of_prints)++;
        (*number_of_elves_waiting)--;
        sem_post(semaphore_writing_incrementing);
        exit(0);
    }

    // But if elve comes up to the shop and finds out he is a third one, he goes and wakes up santa to help them
    if ((*number_of_elves_waiting) >= 3) {
        sem_post(semaphore_santa);
        sem_wait(semaphore_elf);
    } else {
        sem_wait(semaphore_elf);
    }
    // Solving a problem where elves were waiting in front of the shop not knowing santa left, santa tells them to take a vacation when leaving north pole
    if ((*christmas_flag) == 1) {
        sem_wait(semaphore_writing_incrementing);
        fprintf(fp, "%d: Elf %d: taking holidays\n", *order_of_prints, elfID);
        fflush(fp);
        (*order_of_prints)++;
        sem_post(semaphore_writing_incrementing);
        exit(0);
    }
    // Elve got told he is getting a help through semaphore from santa, is working and getting helped, also he counts himself out as he finishes
    sem_wait(semaphore_writing_incrementing);
    fprintf(fp, "%d: Elf %d: get help\n", *order_of_prints, elfID);
    fflush(fp);
    (*order_of_prints)++;
    (*number_of_elves_working)--;
    sem_post(semaphore_writing_incrementing);
    // He finds out hes the last one leaving shop, tells santa to go to sleep
    if((*number_of_elves_working) == 0){
        sem_post(semaphore_working_shop);
    }
    exit(0);
}

void process_reindeer(int reindeerID, int wait_value, int number_of_reindeers_total) {
    unsigned int random_value;

    // Reindeer got initialized
    sem_wait(semaphore_writing_incrementing);
    fprintf(fp, "%d: RD %d: rstarted\n", *order_of_prints, reindeerID);
    fflush(fp);
    (*order_of_prints)++;
    sem_post(semaphore_writing_incrementing);


    // We read random numbers until we find one that is bigger than TR/2 after applying modulo TR, thanks to which we make sure its in range of <TR/2,TR>
    // Didnt work, infinite cycle
    //    do {
        fread(&random_value, sizeof(random_value), 1, random_generator);
//    }while((int)(random_value % wait_value) <= (wait_value / 2));

    // Generating numbers from range of (upperval - lowerval + 1) + lowerval
    wait_value = (wait_value - (wait_value / 2 + 1)) + wait_value / 2;
    if(wait_value != 0){
        usleep(random_value % wait_value);
    }
    // He goes do whatever in a forest waiting for his brothers and sisters
    sem_wait(semaphore_writing_incrementing);
    fprintf(fp, "%d: RD %d: return home\n", *order_of_prints, reindeerID);
    fflush(fp);
    (*order_of_prints)++;
    // Counts himself in
    (*number_of_reindeers_waiting)++;
    // If he finds out he is the last one everyone was waiting for, sets ready flag and wakes up santa, santa then closes shop and christmas starts!
    if ((*number_of_reindeers_waiting) >= number_of_reindeers_total) {
        (*reindeers_ready_flag) = 1;
        sem_post(semaphore_santa);
    }
    sem_post(semaphore_writing_incrementing);
    // Otherwise waits with others for santas hitching
    sem_wait(semaphore_reindeer);

    // Santa started hitching
    sem_wait(semaphore_writing_incrementing);
    fprintf(fp, "%d: RD %d: get hitched\n", *order_of_prints, reindeerID);
    fflush(fp);
    (*order_of_prints)++;
    (*number_of_reindeers_waiting)--;
    sem_post(semaphore_writing_incrementing);

    // If he is the last one to get hitched, he impersonates santa and screams for him the christmas started!
    if((*number_of_reindeers_waiting) == 0){
        sem_wait(semaphore_writing_incrementing);
        fprintf(fp, "%d: Santa: Christmas started\n", *order_of_prints);
        fflush(fp);
        (*order_of_prints)++;
        sem_post(semaphore_writing_incrementing);
    }
    exit(0);
}

int init_semaphores() {
    MMAP(order_of_prints);
    MMAP(number_of_elves_waiting);
    MMAP(number_of_reindeers_waiting);
    MMAP(reindeers_ready_flag);
    MMAP(christmas_flag);
    MMAP(number_of_elves_working);
    // In case we crashed last time
    sem_unlink("/xgajdo33.semaphore_elf");
    sem_unlink("/xgajdo33.semaphore_santa");
    sem_unlink("/xgajdo33.semaphore_reindeer");
    sem_unlink("/xgajdo33.semaphore_writing_incrementing");
    sem_unlink("/xgajdo33.semaphore_working_shop");

    if ((fp = fopen("proj2.out", "w+")) == NULL) {
        fprintf(stderr, "The file failed to open!\n");
        exit(-1);
    }
    if ((random_generator = fopen("/dev/random", "r")) == NULL) {
        fprintf(stderr, "The file failed to open!\n");
        exit(-1);
    }
    if ((semaphore_elf = sem_open("/xgajdo33.semaphore_elf", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {
        return -1;
    }
    if ((semaphore_santa = sem_open("/xgajdo33.semaphore_santa", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {
        return -1;
    }
    if ((semaphore_reindeer = sem_open("/xgajdo33.semaphore_reindeer", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {
        return -1;
    }
    if ((semaphore_writing_incrementing = sem_open("/xgajdo33.semaphore_writing_incrementing", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED) {
        return -1;
    }
    if ((semaphore_working_shop = sem_open("/xgajdo33.semaphore_working_shop", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {
        return -1;
    }
    return 0;
}

void clean_up() {
    UNMAP(order_of_prints);
    UNMAP(number_of_elves_waiting);
    UNMAP(number_of_reindeers_waiting);
    UNMAP(reindeers_ready_flag);
    UNMAP(christmas_flag);
    UNMAP(number_of_elves_working);

    sem_close(semaphore_elf);
    sem_close(semaphore_santa);
    sem_close(semaphore_reindeer);
    sem_close(semaphore_writing_incrementing);
    sem_close(semaphore_working_shop);

    sem_unlink("/xgajdo33.semaphore_elf");
    sem_unlink("/xgajdo33.semaphore_santa");
    sem_unlink("/xgajdo33.semaphore_reindeer");
    sem_unlink("/xgajdo33.semaphore_writing_incrementing");
    sem_unlink("/xgajdo33.semaphore_working_shop");

    if (fclose(fp) == EOF) {
        fprintf(stderr, "Closing of the file failed!\n");
    }
    if (fclose(random_generator) == EOF) {
        fprintf(stderr, "Closing of the file failed!\n");
    }
}

// Returns number if the given string contains a number and nothing but number chars, otherwise returns -1
int check_if_number(char string[]) {
    char *ptr;
    int number = 0, i = 0;
    // We just check if everything in string is between 9 and 0 chars and in scope of size of our char array
    while (string[i] != '\0') {
        if ((string[i] <= '9' && string[i] >= '0') || i == 1000) {
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
    if (init_semaphores() != 0) {
        fprintf(stderr, "Initialization of semaphores failed!\n");
        exit(-1);
    }
    // We just arbitrary increment order of prints because we dont want the printing start at 0 but at 1
    (*order_of_prints)++;
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
    // We create process Santa
    pid_t santa = fork();
    if (santa == 0) {
        process_santa(arguments_values[0], arguments_values[1]);
    }
    // We create a process that starts creating elve processes
    pid_t elve_generator = fork();
    if (elve_generator == 0) {
        // Creating elve processes in a for loop
        for (int i = 1; i <= arguments_values[0]; i++) {
            pid_t elf = fork();
            if (elf == 0) {
                process_elf(i, arguments_values[2]);
            }
        }
        // Telling a process generating another processes to end
        exit(0);
    }
    // We create a process that starts creating reindeer processes
    pid_t reindeer_generator = fork();
    if (reindeer_generator == 0) {
        // Creating reindeer processes in a for loop
        for (int i = 1; i <= arguments_values[1]; i++) {
            pid_t reindeer = fork();
            if (reindeer == 0) {
                process_reindeer(i, arguments_values[3], arguments_values[1]);
            }
        }
        // Telling a process generating another processes to end
        exit(0);
    }

    // Waiting until parent process is the only one left
    int IDs;
    do {
        IDs = wait(NULL);
    } while (IDs != -1);
    // Unlinking and closing everything
    clean_up();
    return 0;
}
