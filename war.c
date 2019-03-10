#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#define LANNISTER_SIZE 16
#define STARK_SIZE 8
#define BUFFER_SIZE 5
#define LANNISTER 0
#define STARK 1

// Buffer
int size = 0;

// Condition variables
pthread_cond_t ice = PTHREAD_COND_INITIALIZER;
pthread_cond_t shutdown = PTHREAD_COND_INITIALIZER;

// Mutex
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

int lannister_waiting_soldiers = LANNISTER_SIZE;
int stark_waiting_soldiers = STARK_SIZE;
int turn;
int max_buffer;

void* Lannister(void* arg) {
    pthread_mutex_lock(&buffer_mutex);
   
    while (turn == 1 || size > 5) {
        pthread_cond_wait(&ice, &buffer_mutex);
    }
    size++;
    if (size == 1) {
        if (lannister_waiting_soldiers > 5) {
            max_buffer = BUFFER_SIZE;
        } else {
            max_buffer = lannister_waiting_soldiers;
        }
    }
    
    lannister_waiting_soldiers--;
    printf("Lannister soldier %i has crossed. There are %i soldiers on ice\n", LANNISTER_SIZE - lannister_waiting_soldiers, size);
    if (size == max_buffer) {
        if (stark_waiting_soldiers > 0) {
            turn = 1;
        }
        size = 0;
        pthread_cond_broadcast(&ice);
    }
    
    pthread_mutex_unlock(&buffer_mutex);
    pthread_exit(NULL);
}

void* Stark(void* arg) {
    pthread_mutex_lock(&buffer_mutex);
    
    while (turn == 0 || size > 5) {
        pthread_cond_wait(&ice, &buffer_mutex);
    }
    size++;
    if (size == 1) {
        if (stark_waiting_soldiers > 5) {
            max_buffer = BUFFER_SIZE;
        } else {
            max_buffer = stark_waiting_soldiers;
        }
    }
    stark_waiting_soldiers--;
    printf("Stark soldier %i has crossed. There are %i soldiers on the ice.\n", STARK_SIZE - stark_waiting_soldiers, size);
    if (size == max_buffer) {
        if (lannister_waiting_soldiers > 0) {
            turn = 0;
        }
        size = 0;
        pthread_cond_broadcast(&ice);
    }

    pthread_mutex_unlock(&buffer_mutex);
    pthread_exit(NULL);
}

int main() {
    // We let lannisters go first
    turn = 0;

    pthread_t lannister_threads[LANNISTER_SIZE];
    pthread_t stark_threads[STARK_SIZE];
    for (int i = 0; i < LANNISTER_SIZE; i++) {
        pthread_create(&lannister_threads[i], NULL, Lannister, NULL);
    }

    for (int i = 0; i < STARK_SIZE; i++) {
        pthread_create(&stark_threads[i], NULL, Stark, NULL);
    }

    for (int i = 0; i < LANNISTER_SIZE; i++) {
        pthread_join(lannister_threads[i], NULL);
    }

    for (int i = 0; i < STARK_SIZE; i++) {
        pthread_join(stark_threads[i], NULL);
    }
    return 0;
}