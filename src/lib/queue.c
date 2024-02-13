#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../../inc/queue.h"

void queue_init(StringQueue *queue) {
    queue->front = 0;
    queue->rear = -1;
    queue->itemCount = 0;
}

bool queue_isEmpty(const StringQueue *queue) { return queue->itemCount == 0; }

bool queue_isFull(const StringQueue *queue) { return queue->itemCount == QUEUE_SIZE; }

void queue_enqueue(StringQueue *queue, const char *string) {
    if (!queue_isFull(queue) && strlen(string)<QUEUE_ELEMENT_SIZE) {
        queue->rear = (queue->rear + 1) % QUEUE_SIZE;
        strcpy(queue->data[queue->rear], string);
        queue->itemCount++;
    }
}

char* queue_dequeue(StringQueue *queue) {
    if (queue_isEmpty(queue))
        return NULL;

    char* string = queue->data[queue->front];
    queue->front = (queue->front + 1) % QUEUE_SIZE;
    queue->itemCount--;
    if (queue->itemCount == 0) {
        queue->front = 0;
        queue->rear = -1;
    }
    return string;
}

char* queue_front(StringQueue *queue) {
    if(queue_isEmpty(queue))
        return NULL;
    return queue->data[queue->front];
}

void queue_destroy(StringQueue *queue) {
    while (!queue_isEmpty(queue))
        queue_dequeue(queue);
}