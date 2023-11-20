#include "Queues.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
#include <errno.h>

UnBoundedQueue *UnBoundedCreateQueue()
{
    UnBoundedQueue *q = malloc(sizeof(UnBoundedQueue));
    if (q == NULL)
        return NULL;
    q->head = q->tail = NULL;
    sem_init(&(q->full_sem), 0, 0);
    return q;
}

void UnBoundedDestroyQueue(UnBoundedQueue *queue)
{
    if (queue == NULL)
        return;

    while (!UnBoundedIsQueueEmpty(queue)) {
        char *data = UnBoundedDequeue(queue);
        free(data);
    }

    free(queue);
}

int UnBoundedIsQueueEmpty(UnBoundedQueue *q)
{
    return (q->tail == NULL && q->head == NULL);
}

void UnBoundedEnqueue(UnBoundedQueue *q, char *data)
{
    Node *node = malloc(sizeof(Node));
    node->data = malloc(strlen(data) + 1);
    strcpy(node->data, data);
    node->next = NULL;

    if (q->tail == NULL)
    {
        q->head = q->tail = node;
        return;
    }
    q->tail->next = node;
    q->tail = node;
}

char *UnBoundedDequeue(UnBoundedQueue *q)
{
    Node *previousHead;
    char *data;

    previousHead = q->head;

    if (previousHead == NULL)
        return NULL;

    q->head = q->head->next;

    if (q->head == NULL)
        q->tail = NULL;

    data = previousHead->data;
    free(previousHead);

    return data;
}

BoundedQueue* BoundedCreateQueue(int size)
{
    BoundedQueue *q = malloc(sizeof(BoundedQueue));
    if (q == NULL)
        return NULL;
    q->head = q->tail = NULL;

   sem_init(&(q->empty_sem), 0, size);
   sem_init(&(q->full_sem), 0, 0);
   
    return q;
}

void BoundedDestroyQueue(BoundedQueue* queue)
{
    if (queue == NULL)
        return;

    while (!BoundedIsQueueEmpty(queue)) {
        char* data = BoundedDequeue(queue);
        free(data);
    }

    free(queue);
}

int BoundedIsQueueEmpty(BoundedQueue* q)
{
    return (q->tail == NULL && q->head == NULL);
}

void BoundedEnqueue(BoundedQueue* q, char* data)
{
    Node *node = malloc(sizeof(Node));
    node->data = malloc(strlen(data) + 1);
    strcpy(node->data, data);
    node->next = NULL;

    if (q->tail == NULL)
    {
        q->head = q->tail = node;
        return;
    }

    q->tail->next = node;
    q->tail = node;
}

char* BoundedDequeue(BoundedQueue* q)
{
    Node *previousHead;
    char* data;

    previousHead = q->head;

    if (previousHead == NULL)
        return NULL;

    q->head = q->head->next;

    if (q->head == NULL)
        q->tail = NULL;

    data = previousHead->data;
    free(previousHead);

    return data;
}
