#ifndef _EVENT_H
#define _EVENT_H

#include <sys/queue.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

struct event {
    TAILQ_ENTRY(event) tailq;
    int (*handle)(void *ctx);
    int *ctx;
} __attribute__((packed));

TAILQ_HEAD(eventq, event);
struct ev_queue {
    struct eventq event;
    pthread_t id;
    sem_t sem;
};

int init_event_queue(struct ev_queue *q);
int push_event(struct event *e);
#endif
