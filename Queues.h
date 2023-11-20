#ifndef QUEUES_H
#define QUEUES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>

// typedef struct {
//     char** stories;
//     int curr_size;
//     int max_size;
//     sem_t semaphore;
//     pthread_mutex_t mutex;
// } BoundedQueue;

extern pthread_mutex_t global_mutex;

typedef struct Node{
	struct Node* next;
	char* data;
}Node;

typedef struct {
   Node *head, *tail;
   //pthread_mutex_t mutex;
   sem_t full_sem;  
}UnBoundedQueue;

typedef struct {
    Node *head, *tail;
    //pthread_mutex_t mutex;
    sem_t empty_sem;
    sem_t full_sem;
}BoundedQueue;

UnBoundedQueue* UnBoundedCreateQueue();

void UnBoundedDestroyQueue(UnBoundedQueue* queue);

int UnBoundedIsQueueEmpty(UnBoundedQueue* queue);

void UnBoundedEnqueue(UnBoundedQueue* queue, char* data);

char* UnBoundedDequeue(UnBoundedQueue* queue);



BoundedQueue* BoundedCreateQueue( int size);

void BoundedDestroyQueue(BoundedQueue* queue);

int BoundedIsQueueEmpty(BoundedQueue* queue);

void BoundedEnqueue(BoundedQueue* queue, char* data);

char* BoundedDequeue(BoundedQueue* queue);






#endif /* QUEUES_H */
