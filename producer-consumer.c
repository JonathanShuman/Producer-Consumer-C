//Jonathan Shuman 206839086
#include "Queues.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
#include <errno.h>

typedef struct {
    int id;
    int num_elements;
    int queue_size;
} producerDetails;

typedef struct {
    producerDetails* producer;
    BoundedQueue* queue;
} ThreadData;

typedef struct {
    int numOfQueues;
    BoundedQueue** queues;
    UnBoundedQueue* co_types[3];
} dispatcherThreadData;

typedef struct {
    UnBoundedQueue* coEditorQueue;
    BoundedQueue *screenManagerQueue;
} coEditorsThreadData;

pthread_mutex_t global_mutex;
pthread_mutex_t print_mutex;


void printBoundedQueue(BoundedQueue *queue, int numQueues)
{

    

    for (int i = 0; i < numQueues; i++)
    {

        
        printf("Screen BoundedQueue %d:\n", i);

        Node *currentNode = queue->head;

        while (currentNode != NULL)
        {

            

            printf("%s\n", currentNode->data);

            // printf("You are in while inside if in print\n");

            currentNode = currentNode->next;
        }

        printf("\n");
    }
}

void printBoundedQueues(BoundedQueue **queues, int numQueues)
{

    for (int i = 0; i < numQueues; i++)
    {

        BoundedQueue *queue = queues[i];

        printf("BoundedQueue %d:\n", i);

        Node *currentNode = queue->head;

        while (currentNode != NULL)
        {

            printf("%s\n", currentNode->data);

            currentNode = currentNode->next;
        }

        printf("\n");
    }
}


int count_producers(const char *file_path)
{
    FILE *file = fopen(file_path, "r");

    if (file == NULL)
    {
        printf("Error opening the configuration file.\n");
        return 0;
    }

    int current_producer = 0;
    int num_producers = 0;

    // Read the configuration file line by line
    char line[100];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Skip empty lines
        if (line[0] == '\n' || line[0] == '\r')
        {
            continue;
        }

        // Remove carriage return character if present (for Windows files)
        if (line[strlen(line) - 1] == '\r')
        {
            line[strlen(line) - 1] = '\0';
        }

        current_producer++;
    }

    num_producers = current_producer / 3;

    fclose(file);

    return num_producers;
}


int count_screen(const char *file_path)
{
    FILE *file = fopen(file_path, "r");
    if (file == NULL)
    {
        printf("Error opening the configuration file.\n");
        return 0;
    }
    int num_producers = 0;
    int x = 0;
    char line[100];
    char last_line[100] = "";

    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Skip empty lines
        if (line[0] == '\n' || line[0] == '\r')
        {
            continue;
        }

        // Remove carriage return character if present (for Windows files)
        if (line[strlen(line) - 1] == '\r')
        {
            line[strlen(line) - 1] = '\0';
        }

        strncpy(last_line, line, sizeof(last_line));
    }
    fclose(file);

    x = atoi(last_line);

    return x;
}




void init_producersDetails(const char *file_path, producerDetails* producers)
{
    FILE *file = fopen(file_path, "r");

    if (file == NULL)
    {
        printf("Error opening the configuration file.\n");
        return;
    }

    int current_producer = 0;
    int num_producers = 0;

    // Read the configuration file line by line
    char line[100];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Skip empty lines
        if (line[0] == '\n' || line[0] == '\r')
        {
            continue;
        }

        // Remove carriage return character if present (for Windows files)
        if (line[strlen(line) - 1] == '\r')
        {
            line[strlen(line) - 1] = '\0';
        }

        // Parse the values from each line
        int value = atoi(line);

        if (current_producer % 3 == 0)
        {   
            producers[num_producers].id = value;
        }
        else if (current_producer % 3 == 1)
        {
            producers[num_producers].num_elements = value;
        }
        else if (current_producer % 3 == 2)
        {
            producers[num_producers].queue_size = value;
            num_producers++;
        }

        current_producer++;
    }

    fclose(file);
}


void printUnBoundedQueues(UnBoundedQueue **queues, int numQueues)
{

    for (int i = 0; i < numQueues; i++)
    {

        UnBoundedQueue *queue = queues[i];

        printf("UnBoundedQueue %d:\n", i);

        Node *currentNode = queue->head;

        while (currentNode != NULL)
        {

            printf("%s\n", currentNode->data);

            currentNode = currentNode->next;
        }

        printf("\n");
    }
}


// Function executed by the producer threads
void* Producer(void* arg)
{
    ThreadData* thread_data = (ThreadData*)arg;
    producerDetails* producer = thread_data->producer;
    BoundedQueue* queue = thread_data->queue;
    int producer_id= producer->id;
    int number_elements= producer->num_elements;
    int queue_size = producer->queue_size;

    srand(time(NULL));

    int sports = 0, news = 0, weather = 0;

    for (int i = 0; i < number_elements; i++)
    {
        char product[300];
        int subject = rand() % 3;

        if (subject == 0)
        {
            snprintf(product, sizeof(product), "Producer %d SPORTS %d", producer_id, sports);
            sports++;
        }
        else if (subject == 1)
        {
            snprintf(product, sizeof(product), "Producer %d NEWS %d", producer_id, news);
            news++;
        }
        else if (subject == 2)
        {
            snprintf(product, sizeof(product), "Producer %d WEATHER %d", producer_id, weather);
            weather++;
        }

         sem_wait(&(queue->empty_sem));
         pthread_mutex_lock(&(global_mutex));
         BoundedEnqueue(queue, product);
         pthread_mutex_unlock(&(global_mutex));
         sem_post(&(queue->full_sem));
    }

        sem_wait(&(queue->empty_sem));
         pthread_mutex_lock(&(global_mutex));
         BoundedEnqueue(queue, "DONE");
         pthread_mutex_unlock(&(global_mutex));
         sem_post(&(queue->full_sem));

//     pthread_mutex_lock(&(print_mutex));
//     printBoundedQueue(queue, 1);
//     pthread_mutex_unlock(&(print_mutex));   
 }

void *Dispatcher(void *arg)
{
    dispatcherThreadData* thread_data = (dispatcherThreadData*)arg;
    int num_queues = thread_data->numOfQueues;
    BoundedQueue** queues = thread_data->queues;
    UnBoundedQueue** co_types = thread_data->co_types;
    char *news = "NEWS";
    char *sports = "SPORTS";
    char *weather = "WEATHER";

    for (int current_queue = 0, ended = 0; ended != num_queues; current_queue = (current_queue + 1) % num_queues)
    {
        char story[300];
        memset(story, '\0', sizeof(story));
        if (BoundedIsQueueEmpty(queues[current_queue]))
        {
            continue;
        }
        sem_wait(&(queues[current_queue]->full_sem));
        pthread_mutex_lock(&(global_mutex));
        strcpy(story, BoundedDequeue(queues[current_queue]));
        //printf("%s\n", story);
        pthread_mutex_unlock(&(global_mutex));
        sem_post(&(queues[current_queue]->empty_sem));

         if (strcmp(story, "DONE") == 0)
        {
            ended++;
            continue;
        }

        if (strstr(story, news) != NULL)
        {
            pthread_mutex_lock(&(global_mutex));
            UnBoundedEnqueue(co_types[0], story);
            pthread_mutex_unlock(&(global_mutex));
            sem_post(&(co_types[0]->full_sem));
        }

        if (strstr(story, sports) != NULL)
        {
            pthread_mutex_lock(&(global_mutex));
            UnBoundedEnqueue(co_types[1], story);
            pthread_mutex_unlock(&(global_mutex));
            sem_post(&(co_types[1]->full_sem));
        }

        if (strstr(story, weather) != NULL)
        {
            pthread_mutex_lock(&(global_mutex));
            UnBoundedEnqueue(co_types[2], story);
            pthread_mutex_unlock(&(global_mutex));
            sem_post(&(co_types[2]->full_sem));
        }

    }

    for (int i = 0; i < 3; i++)
    {
    pthread_mutex_lock(&(global_mutex));
    UnBoundedEnqueue(co_types[i], "DONE");
    pthread_mutex_unlock(&(global_mutex));
    sem_post(&(co_types[i]->full_sem));
    }

    //  pthread_mutex_lock(&(print_mutex));
    //  printUnBoundedQueues(co_types, 3);
    //  pthread_mutex_unlock(&(print_mutex));   
}

void *Co_editors(void *arg)
{
coEditorsThreadData* thread_data = (coEditorsThreadData*)arg;
UnBoundedQueue* coEditorQueue = thread_data->coEditorQueue;
BoundedQueue* screenManagerQueue = thread_data->screenManagerQueue;

//         pthread_mutex_lock(&(print_mutex));
//         Node *currentNode = coEditorQueue->head;

//         while (currentNode != NULL)
//         {
//             printf("%s\n", currentNode->data);
//             currentNode = currentNode->next;
//         }
// pthread_mutex_unlock(&(print_mutex));

 while (true)
    {
        char story[300];
        memset(story, '\0', sizeof(story));
        sem_wait(&(coEditorQueue->full_sem));
        pthread_mutex_lock(&(global_mutex));
        strcpy(story, UnBoundedDequeue(coEditorQueue));
        pthread_mutex_unlock(&(global_mutex));

        if (strcmp(story, "DONE") == 0)
        {
            break;
        }

        usleep(100 * 1000);

       //printf("the story before enqueue to screen is: %s\n", story);

        sem_wait(&(screenManagerQueue->empty_sem));
        pthread_mutex_lock(&(global_mutex));
        BoundedEnqueue(screenManagerQueue, story);
        // printf("story has added to the screen manager queue : %s\n", story);
        pthread_mutex_unlock(&(global_mutex));
        sem_post(&(screenManagerQueue->full_sem));
    }
    sem_wait(&(screenManagerQueue->empty_sem));
    pthread_mutex_lock(&(global_mutex));
    BoundedEnqueue(screenManagerQueue, "DONE");
    pthread_mutex_unlock(&(global_mutex));
    sem_post(&(screenManagerQueue->full_sem));
}



int main(int argc, char const* argv[])
{
    pthread_mutex_init(&(print_mutex), NULL);
    // initialize mutex
    pthread_mutex_init(&(global_mutex), NULL);
    // get number producers
    int number_producers = count_producers(argv[1]);    
    int screen_queue_size = count_screen(argv[1]);  
    // allocate memory to producers details array
    producerDetails* producers = (producerDetails*)malloc((number_producers+1) * sizeof(producerDetails));
    
    // fill the array with the correct data from conf.txt
    init_producersDetails(argv[1], producers);

    // allocate memory for queue for each producer

    BoundedQueue** producer_queues = (BoundedQueue**)malloc((number_producers+1) * sizeof(BoundedQueue*));

    for (int i = 0; i < number_producers; i++) {
        producer_queues[i] = BoundedCreateQueue(producers[i].queue_size);
    }

    // create threads forproducer func
    pthread_t* produce_threads = (pthread_t*)malloc(number_producers * sizeof(pthread_t));
    for (int i = 0; i < number_producers; i++){
        //pthread_t produce_thread;
        ThreadData* producer_thread_data = (ThreadData*)malloc(sizeof(ThreadData));
        producer_thread_data->producer = &producers[i];
        producer_thread_data->queue = producer_queues[i];
        if (pthread_create(&produce_threads[i], NULL, Producer, (void*)producer_thread_data) != 0) {
            printf("Error creating thread %d\n", i);
            return 1;
        }
        //free(producer_thread_data);
    }

    
    //make dispatcher thread ans send to dispach function
        UnBoundedQueue* types[3];
        for (int i = 0; i < 3; i++)
        {
            types[i] = UnBoundedCreateQueue();
        }
        
        dispatcherThreadData* dispatcher_thread_data = (dispatcherThreadData*)malloc(sizeof(dispatcherThreadData));
        pthread_t dispatcher_thread;
        dispatcher_thread_data->numOfQueues = number_producers;
        dispatcher_thread_data->queues = producer_queues;
        for (int i = 0; i < 3; i++) {
            dispatcher_thread_data->co_types[i] = types[i];
        }
        if (pthread_create(&dispatcher_thread, NULL, Dispatcher, (void*)dispatcher_thread_data) != 0) {
            printf("Error creating thread\n");
            return 1;
        }

    BoundedQueue* screenQueue = BoundedCreateQueue(screen_queue_size);

    //makes 3 threads for coeditors to pop from his own queue
    pthread_t coEditors_threads[3];
    for (int i = 0; i < 3; i++)
    {
      coEditorsThreadData* coEditorData= (coEditorsThreadData*)malloc(sizeof(coEditorsThreadData));
      coEditorData->coEditorQueue = types[i]; 
      coEditorData->screenManagerQueue = screenQueue;
       if (pthread_create(&coEditors_threads[i], NULL, Co_editors, (void*)coEditorData) != 0) {
            printf("Error creating thread\n");
            return 1;
        }
        //free(coEditorData);
    }

     int end = 0;

    //int count = 0;

    while (end != 3)
    {

        char story[300];
        memset(story, '\0', sizeof(story));
        sem_wait(&(screenQueue->full_sem));
        pthread_mutex_lock(&(global_mutex));
        strcpy(story, BoundedDequeue(screenQueue));
        pthread_mutex_unlock(&(global_mutex));
        sem_post(&(screenQueue->empty_sem));
        if (strcmp(story, "DONE") == 0)
        {
            end++;
        }
        else
        {
            printf("%s\n", story);
            //count++;
        }
    }
      printf("DONE\n");


    



    // join producer threads

     for (int i = 0; i < number_producers; i++) {
      pthread_join(produce_threads[i], NULL);
    }

    // join dispatcher threads
     pthread_join(dispatcher_thread, NULL);

    //join coEditors threads
      for (int i = 0; i < 3; i++) {
      pthread_join(coEditors_threads[i], NULL);
    }

    free(producers);
    for (int i = 0; i < number_producers; i++) {
        BoundedDestroyQueue(producer_queues[i]);
    }
    free(producer_queues);
    BoundedDestroyQueue(screenQueue);
    for (int i = 0; i < 3; i++) {
        UnBoundedDestroyQueue(types[i]);
    }
        free(produce_threads);
        




   
}
