#ifndef _EVENT_H
#define _EVENT_H

#include <sys/queue.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

//TODO: define a call-back to event
//worker should call call-back after sem_wait

struct event_header {
    uint8_t type;
    uint16_t len;
};

struct event {
    TAILQ_ENTRY(event) tailq;
    struct event_header header;
    void *event;
} __attribute__((packed));

TAILQ_HEAD(eventq, event);
struct ev_queue {
    struct eventq event;
    pthread_t id;
    sem_t sem;
};

int init_event_queue(struct ev_queue *q);
int push_event(struct event *e, struct ev_queue *q);
#endif
