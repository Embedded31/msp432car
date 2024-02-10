#include <stdbool.h>

#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_SIZE 10

typedef struct QueueStruct Queue;

Queue *queue_create();

bool queue_isEmpty(Queue *queue);

bool queue_isFull(Queue *queue);

void queue_enqueue(Queue *queue, void *data);

void *queue_dequeue(Queue *queue);

void queue_clear(Queue *queue);

#endif // QUEUE_H
