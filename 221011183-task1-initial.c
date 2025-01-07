#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_NUMBERS 10000
#define BATCH_SIZE 100

// Global variables
char* filename = "numbers.txt";
int numbers_written = 0;
int numbers_read = 0;

// Thread functions declarations
void* create_file(void* arg);
void* produce_numbers(void* arg);
void* consume_numbers(void* arg);

// Thread 1: Create empty file
void* create_file(void* arg) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error creating file");
        exit(1);
    }
    fclose(file);
    printf("Empty file created successfully\n");
    return NULL;
}

// Thread 2: Producer - Generate and write random numbers
void* produce_numbers(void* arg) {
    FILE* file;
    srand(time(NULL));
    
    while (numbers_written < BATCH_SIZE) {
        file = fopen(filename, "a");
        if (file == NULL) {
            perror("Error opening file for writing");
            exit(1);
        }
        
        int number = rand() % 100;
        fprintf(file, "%d\n", number);
        fclose(file);
        
        numbers_written++;
        printf("Producer: Written number %d\n", number);
        usleep(50000); // Sleep for 50ms to simulate processing time
    }
    return NULL;
}

// Thread 3: Consumer - Read numbers
void* consume_numbers(void* arg) {
    FILE* file;
    char line[10];
    
    while (numbers_read < BATCH_SIZE) {
        file = fopen(filename, "r");
        if (file == NULL) {
            perror("Error opening file for reading");
            exit(1);
        }
        
        int current_line = 0;
        while (fgets(line, sizeof(line), file) && current_line <= numbers_read) {
            current_line++;
        }
        
        if (current_line > numbers_read) {
            printf("Consumer: Read number %s", line);
            numbers_read++;
        }
        
        fclose(file);
        usleep(75000); // Sleep for 75ms to simulate processing time
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2, thread3;
    
    // Create threads
    if (pthread_create(&thread1, NULL, create_file, NULL) != 0) {
        perror("Error creating thread 1");
        return 1;
    }
    
    // Wait for file creation to complete
    pthread_join(thread1, NULL);
    
    // Create producer and consumer threads
    if (pthread_create(&thread2, NULL, produce_numbers, NULL) != 0) {
        perror("Error creating thread 2");
        return 1;
    }
    
    if (pthread_create(&thread3, NULL, consume_numbers, NULL) != 0) {
        perror("Error creating thread 3");
        return 1;
    }
    
    // Wait for producer and consumer to finish
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    
    printf("All threads completed successfully\n");
    return 0;
}
