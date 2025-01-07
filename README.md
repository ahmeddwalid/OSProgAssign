
# 12th Programming Assignment

<h3 align="center">Ahmed Walid</h3>

---

## Files Overview

### 1. `task1-initial.c`

#### **Description:**

- Simulates a producer-consumer scenario without using explicit synchronization mechanisms like semaphores or mutexes.
- Three threads perform the following tasks:
  1. **Thread 1:** Creates an empty file.
  2. **Thread 2 (Producer):** Generates random numbers and writes them to the file.
  3. **Thread 3 (Consumer):** Reads numbers from the file.

#### **Key Features:**

- Demonstrates basic multithreading.
- Uses file I/O for communication between producer and consumer.

#### **Limitations:**

- No synchronization, leading to potential race conditions (e.g., consumer reading before producer writes).

---

### 2. `task1-mutex.c`

#### **Description:**

- Extends `task1-initial.c` by adding synchronization using semaphores and mutexes to avoid race conditions.

#### **Key Features:**

- **Semaphores**:
  - `empty`: Tracks empty slots in the buffer (file).
  - `full`: Tracks full slots in the buffer (file).
- **Mutex:** Ensures mutual exclusion during file access.

#### **Improvements:**

- Prevents consumer from reading before producer writes.
- Handles concurrent access to shared resources more robustly.

---

### 3. `bonus-initial.c`

#### **Description:**

- Implements a multi-file producer-consumer scenario.
- Three threads perform the following tasks:
  1. **Thread 1:** Creates multiple empty files.
  2. **Thread 2 (Producer):** Writes random numbers across the files, cycling between them.
  3. **Thread 3 (Consumer):** Reads numbers from the files in a round-robin manner.

#### **Key Features:**

- Demonstrates advanced file handling with multiple producers and consumers.
- Focuses on managing multiple files dynamically.

#### **Limitations:**

- No synchronization, making it prone to race conditions in concurrent environments.

---

### 4. `bonus-mutex.c`

#### **Description:**

- Builds upon `bonus-initial.c` by introducing synchronization to handle multiple files safely.

#### **Key Features:**

- **Semaphores:**
  - `files_ready`: Signals when all files are created.
  - `empty` and `full`: Manage buffer states across multiple files.
- **Mutexes:**
  - `file_mutex`: Protects file access.
  - `count_mutex`: Protects global counters (e.g., numbers\_written, numbers\_read).

#### **Improvements:**

- Handles concurrency with proper synchronization.
- Ensures correct order of operations between producers and consumers.

---

## Compilation and Execution

Each program can be compiled and executed using GCC.

### Compilation:

```bash
gcc -pthread -o output_file program_name.c
```

### Execution:

```bash
./output_file
```

---

## Key Concepts Demonstrated

### 1. **Multithreading with pthreads:**

- Creation and management of threads using `pthread_create` and `pthread_join`.

### 2. **Synchronization:**

- Avoiding race conditions with semaphores (`sem_init`, `sem_wait`, `sem_post`) and mutexes (`pthread_mutex_lock`, `pthread_mutex_unlock`).

### 3. **File I/O Operations:**

- Writing to and reading from files in a multithreaded context.

### 4. **Producer-Consumer Problem:**

- Simulating real-world scenarios of producer-consumer workflows.

---

## Improvements and Customizations

- **Error Handling:** Enhance error messages for better debugging.
- **Dynamic File Handling:** Increase the number of files or lines dynamically based on input.
- **Performance Optimization:** Optimize sleep intervals and buffer management for efficiency.
- **Thread Scaling:** Extend programs to support multiple producers and consumers simultaneously.

---

## Author

Ahmed Walid
