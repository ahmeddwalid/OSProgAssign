#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define MAX_FILES 2
#define MAX_LINES 3
#define TOTAL_NUMBERS 100

// Global variables
char filenames[MAX_FILES][256];
int files_created = 0;
int numbers_written = 0;
int numbers_read = 0;

// Thread functions declarations
void* create_files(void* arg);
void* produce_numbers(void* arg);
void* consume_numbers(void* arg);

// Thread 1: Create empty files
void* create_files(void* arg) {
    char filename[256];
    for (int i = 0; i < MAX_FILES; i++) {
        sprintf(filename, "file%d.txt", i + 1);
        FILE* file = fopen(filename, "w");
        if (file == NULL) {
            perror("Error creating file");
            exit(1);
        }
        fclose(file);
        strcpy(filenames[i], filename);
        files_created++;
        printf("Created file: %s\n", filename);
    }
    return NULL;
}

// Thread 2: Producer - Generate and write random numbers
void* produce_numbers(void* arg) {
    FILE* file;
    int current_file = 0;
    int lines_in_file = 0;
    srand(time(NULL));
    
    while (numbers_written < TOTAL_NUMBERS) {
        if (files_created < MAX_FILES) {
            usleep(1000); // Wait for files to be created
            continue;
        }
        
        file = fopen(filenames[current_file], "a");
        if (file == NULL) {
            perror("Error opening file for writing");
            exit(1);
        }
        
        int number = rand() % 100;
        fprintf(file, "%d\n", number);
        fclose(file);
        
        numbers_written++;
        lines_in_file++;
        printf("Producer: Written number %d to %s\n", number, filenames[current_file]);
        
        if (lines_in_file >= MAX_LINES) {
            current_file = (current_file + 1) % MAX_FILES;
            lines_in_file = 0;
        }
        
        usleep(50000); // Sleep for 50ms to simulate processing time
    }
    return NULL;
}

// Thread 3: Consumer - Read numbers
void* consume_numbers(void* arg) {
    FILE* file;
    char line[10];
    int current_file = 0;
    
    while (numbers_read < TOTAL_NUMBERS) {
        if (files_created < MAX_FILES) {
            usleep(1000); // Wait for files to be created
            continue;
        }
        
        file = fopen(filenames[current_file], "r");
        if (file == NULL) {
            current_file = (current_file + 1) % MAX_FILES;
            continue;
        }
        
        while (fgets(line, sizeof(line), file)) {
            printf("Consumer: Read number %s from %s", line, filenames[current_file]);
            numbers_read++;
        }
        
        fclose(file);
        current_file = (current_file + 1) % MAX_FILES;
        usleep(75000); // Sleep for 75ms to simulate processing time
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2, thread3;
    
    // Create all threads concurrently
    if (pthread_create(&thread1, NULL, create_files, NULL) != 0) {
        perror("Error creating thread 1");
        return 1;
    }
    
    if (pthread_create(&thread2, NULL, produce_numbers, NULL) != 0) {
        perror("Error creating thread 2");
        return 1;
    }
    
    if (pthread_create(&thread3, NULL, consume_numbers, NULL) != 0) {
        perror("Error creating thread 3");
        return 1;
    }
    
    // Wait for all threads to complete
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    
    printf("All threads completed successfully\n");
    return 0;
}
