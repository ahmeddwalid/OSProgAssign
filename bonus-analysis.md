# Analysis of Critical Sections and Data Integrity in Multi-File Producer-Consumer System

## Critical Sections Identified

1. **File Creation and Name Storage**
   - Writing to the filenames array
   - Incrementing files_created counter
   - File creation operations

2. **Producer Operations**
   - File access for writing
   - Updating file_lines counter
   - Incrementing numbers_written
   - Checking file capacity

3. **Consumer Operations**
   - File access for reading
   - Incrementing numbers_read
   - Checking file contents

4. **Shared Resources**
   - filenames array
   - files_created counter
   - numbers_written and numbers_read counters
   - file_lines array

## Race Conditions in Initial Implementation

The initial version (bonus-initial.c) has several potential race conditions:

1. **File Creation Phase**
   - Producer and consumer might start before files are created
   - Race condition in accessing filenames array
   - Inconsistent files_created counter

2. **Production Phase**
   - Multiple writes to the same file
   - Inconsistent line counting
   - Race condition in numbers_written counter

3. **Consumption Phase**
   - Reading partially written data
   - Missing or duplicate reads
   - Race condition in numbers_read counter

## Synchronization Solution

The modified implementation (bonus-mutex.c) uses three semaphores and two mutexes:

1. **Semaphores**
   - files_ready: Coordinates file creation completion
   - empty: Tracks available slots for writing
   - full: Tracks available data for reading

2. **Mutexes**
   - file_mutex: Protects file operations
   - count_mutex: Protects counter variables

### Implementation Details

1. **File Creation Synchronization**
```c
sem_t files_ready;
// In create_files:
sem_post(&files_ready);
sem_post(&files_ready);  // Post twice for two threads
```

2. **Producer-Consumer Synchronization**
```c
sem_t empty;
sem_t full;
pthread_mutex_t file_mutex;
pthread_mutex_t count_mutex;
```

## Data Integrity Verification

To ensure no data is lost or overwritten, the implementation includes several mechanisms:

1. **Counter Verification**
   - Tracks numbers_written and numbers_read
   - Final comparison ensures equality
   - Per-file line counting with file_lines array

2. **Synchronization Barriers**
   - files_ready semaphore ensures proper initialization
   - empty/full semaphores prevent data overflow/underflow
   - Mutexes ensure atomic operations

3. **File Management**
   - Separate line counting per file