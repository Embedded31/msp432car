#include <stdbool.h>
#include <stdint.h>

#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_SIZE 10
#define QUEUE_ELEMENT_SIZE 100

typedef struct {
    char data[QUEUE_SIZE][QUEUE_ELEMENT_SIZE];
    int8_t front;
    int8_t rear;
    uint8_t itemCount;
} StringQueue;

void queue_init(StringQueue *queue);

bool queue_isEmpty(const StringQueue *queue);

bool queue_isFull(const StringQueue *queue);

void queue_enqueue(StringQueue *queue, const char *data);

char *queue_dequeue(StringQueue *queue);

char *queue_front(StringQueue *queue);

void queue_clear(StringQueue *queue);

#endif // QUEUE_H
