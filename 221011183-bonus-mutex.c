#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>

#define MAX_FILES 2
#define MAX_LINES 3
#define TOTAL_NUMBERS 100

// Global variables
char filenames[MAX_FILES][256];
int files_created = 0;
int numbers_written = 0;
int numbers_read = 0;
int file_lines[MAX_FILES] = {0};  // Track lines in each file

// Synchronization primitives
sem_t files_ready;      // Signals when files are created
sem_t empty;           // Counts empty slots
sem_t full;            // Counts filled slots
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

// Thread functions declarations
void* create_files(void* arg);
void* produce_numbers(void* arg);
void* consume_numbers(void* arg);

// Thread 1: Create empty files
void* create_files(void* arg) {
    char filename[256];
    for (int i = 0; i < MAX_FILES; i++) {
        sprintf(filename, "file%d.txt", i + 1);
        
        pthread_mutex_lock(&file_mutex);
        FILE* file = fopen(filename, "w");
        if (file == NULL) {
            perror("Error creating file");
            pthread_mutex_unlock(&file_mutex);
            exit(1);
        }
        fclose(file);
        strcpy(filenames[i], filename);
        files_created++;
        pthread_mutex_unlock(&file_mutex);
        
        printf("Created file: %s\n", filename);
    }
    
    // Signal that all files are created
    sem_post(&files_ready);
    sem_post(&files_ready);  // Post twice for two threads
    return NULL;
}

// Thread 2: Producer - Generate and write random numbers
void* produce_numbers(void* arg) {
    FILE* file;
    int current_file = 0;
    srand(time(NULL));
    
    // Wait for files to be created
    sem_wait(&files_ready);
    
    while (1) {
        pthread_mutex_lock(&count_mutex);
        if (numbers_written >= TOTAL_NUMBERS) {
            pthread_mutex_unlock(&count_mutex);
            break;
        }
        pthread_mutex_unlock(&count_mutex);
        
        sem_wait(&empty);
        pthread_mutex_lock(&file_mutex);
        
        // Check if current file is full
        if (file_lines[current_file] >= MAX_LINES) {
            current_file = (current_file + 1) % MAX_FILES;
            if (file_lines[current_file] >= MAX_LINES) {
                pthread_mutex_unlock(&file_mutex);
                sem_post(&empty);
                continue;
            }
        }
        
        file = fopen(filenames[current_file], "a");
        if (file == NULL) {
            perror("Error opening file for writing");
            pthread_mutex_unlock(&file_mutex);
            exit(1);
        }
        
        int number = rand() % 100;
        fprintf(file, "%d\n", number);
        fclose(file);
        
        file_lines[current_file]++;
        pthread_mutex_lock(&count_mutex);
        numbers_written++;
        pthread_mutex_unlock(&count_mutex);
        
        printf("Producer: Written number %d to %s (line %d)\n", 
               number, filenames[current_file], file_lines[current_file]);
        
        pthread_mutex_unlock(&file_mutex);
        sem_post(&full);
        
        usleep(50000); // Sleep for 50ms to simulate processing time
    }
    return NULL;
}

// Thread 3: Consumer - Read numbers
void* consume_numbers(void* arg) {
    FILE* file;
    char line[10];
    int current_file = 0;
    
    // Wait for files to be created
    sem_wait(&files_ready);
    
    while (1) {
        pthread_mutex_lock(&count_mutex);
        if (numbers_read >= TOTAL_NUMBERS) {
            pthread_mutex_unlock(&count_mutex);
            break;
        }
        pthread_mutex_unlock(&count_mutex);
        
        sem_wait(&full);
        pthread_mutex_lock(&file_mutex);
        
        file = fopen(filenames[current_file], "r");
        if (file == NULL) {
            pthread_mutex_unlock(&file_mutex);
            current_file = (current_file + 1) % MAX_FILES;
            sem_post(&full);
            continue;
        }
        
        int numbers_in_file = 0;
        while (fgets(line, sizeof(line), file) && numbers_in_file < file_lines[current_file]) {
            numbers_in_file++;
            printf("Consumer: Read number %s from %s", line, filenames[current_file]);
            pthread_mutex_lock(&count_mutex);
            numbers_read++;
            pthread_mutex_unlock(&count_mutex);
        }
        
        fclose(file);
        
        if (numbers_in_file >= MAX_LINES) {
            current_file = (current_file + 1) % MAX_FILES;
        }
        
        pthread_mutex_unlock(&file_mutex);
        sem_post(&empty);
        
        usleep(75000); // Sleep for 75ms to simulate processing time
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2, thread3;
    
    // Initialize semaphores
    sem_init(&files_ready, 0, 0);
    sem_init(&empty, 0, MAX_FILES * MAX_LINES);  // Total slots available
    sem_init(&full, 0, 0);                       // Initially no items
    
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
    
    // Cleanup
    sem_destroy(&files_ready);
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&file_mutex);
    pthread_mutex_destroy(&count_mutex);
    
    printf("All threads completed successfully\n");
    printf("Final stats: Written: %d, Read: %d\n", numbers_written, numbers_read);
    return 0;
}
