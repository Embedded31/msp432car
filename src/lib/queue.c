#include <stdint.h>
#include <stdio.h>

#include "../../inc/queue.h"

struct QueueStruct {
    void *data[QUEUE_SIZE];
    int8_t front;
    int8_t rear;
    uint8_t itemCount;
};

Queue *queue_create() {
    static struct QueueStruct queue;
    queue.front = 0;
    queue.rear = -1;
    queue.itemCount = 0;
    return &queue;
}

bool queue_isEmpty(Queue *queue) { return queue->itemCount == 0; }

bool queue_isFull(Queue *queue) { return queue->itemCount == QUEUE_SIZE; }

void queue_enqueue(Queue *queue, void *data) {
    if (!queue_isFull(queue)) {
        queue->rear = (queue->rear + 1) % QUEUE_SIZE;
        queue->data[queue->rear] = data;
        queue->itemCount++;
    }
}

void *queue_dequeue(Queue *queue) {
    if (queue_isEmpty(queue))
        return NULL;

    void *data = queue->data[queue->front];
    queue->front = (queue->front + 1) % QUEUE_SIZE;
    queue->itemCount--;
    if (queue->itemCount == 0) {
        queue->front = 0;
        queue->rear = -1;
    }
    return data;
}

void queue_destroy(Queue *queue) {
    while (!queue_isEmpty(queue))
        queue_dequeue(queue);
}