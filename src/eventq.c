#include "eventq.h"
#include "debug.h"
#include "utils.h"

struct ev_queue *g_queue = NULL;

static void *worker(void *ctx)
{
    struct ev_queue *q = NULL;
    struct event *e = NULL;
    q = (struct ev_queue*) ctx;
    while (1) {
        infof("sem_wait");
        sem_wait(&(q->sem));
        e = TAILQ_FIRST(&(q->event));
        e->handle(e->ctx);
        sfree(e->ctx);
        TAILQ_REMOVE(&(q->event), e, tailq);
    }
}

int init_event_queue(struct ev_queue *q)
{
    TAILQ_INIT(&q->event);
    sem_init(&(q->sem), 0, 0);
    pthread_create(&q->id, NULL, worker, (void *) q);
    g_queue = q;
    return 0;
}

int push_event(struct event *e)
{
    TAILQ_INSERT_TAIL(&(g_queue->event), e, tailq);
    sem_post(&(g_queue->sem));
}
