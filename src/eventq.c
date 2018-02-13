#include "eventq.h"
#include "debug.h"

static void *worker(void *ctx)
{
    struct ev_queue *q = NULL;
    q = (struct ev_queue*) ctx;
    while (1) {
        infof("sem_wait");
        sem_wait(&(q->sem));
    }
}

int init_event_queue(struct ev_queue *q)
{
    TAILQ_INIT(&q->event);
    sem_init(&(q->sem), 0, 0);
    pthread_create(&q->id, NULL, worker, (void *) q);
    return 0;
}

int push_event(struct event *e, struct ev_queue *q)
{
    infof("pushing");
    TAILQ_INSERT_TAIL(&(q->event), e, tailq);
    sem_post(&(q->sem));
}
