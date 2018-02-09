#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

#include "iface.h"
#include "factory.h"
#include "utils.h"
#include "debug.h"

TAILQ_HEAD(ifaceq, iface);
struct queue {
    struct ifaceq iface;
};

static void init_iface_queue(struct queue *q);
static void run_iface_queue(struct queue *q);

//TODO: add signal handler to main
int main(int argc, char *argv[])
{
    struct queue q;
    init_iface_queue(&q);
    run_iface_queue(&q);
    return 0;
}

static void init_iface_queue(struct queue *q)
{
    int i;
    TAILQ_INIT(&(q->iface));
    for (i = 0; i < IFACE_LAST; i++) {
        int ret;
        struct iface *iface = NULL;
        iface = (struct iface*) malloc(sizeof(struct iface));
        if ((ret = create_iface(i, iface))) {
            errorf("failed to create iface %d", i);
            sfree(iface);
            continue;
        }
        TAILQ_INSERT_TAIL(&(q->iface), iface, tailq);
    }
}

static void run_iface_queue(struct queue *q)
{
    struct iface *p = NULL;
    TAILQ_FOREACH(p, &q->iface, tailq) {
        pthread_join(p->id, NULL);
    }
    return;
}
