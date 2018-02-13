#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <unistd.h>

#include "iface.h"
#include "eventq.h"
#include "factory.h"
#include "utils.h"
#include "debug.h"

TAILQ_HEAD(ifaceq, iface);
struct if_queue {
    struct ifaceq iface;
};

static int init_iface_queue(struct if_queue *q);

//TODO: add signal handler to main
int main(int argc, char *argv[])
{
    struct if_queue if_q;
    struct ev_queue ev_q;
    struct iface *p = NULL;

    if (init_iface_queue(&if_q)) {
        errorf("failed to init iface queue");
        goto bail;
    }

    if (init_event_queue(&ev_q)) {
        errorf("failed to init event queue");
        goto bail;
    }

    TAILQ_FOREACH(p, &if_q.iface, tailq) {
        pthread_join(p->id, NULL);
    }

    pthread_join(ev_q.id, NULL);
    return 0;
bail:
    return 1;
}

static int init_iface_queue(struct if_queue *q)
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
            goto bail;
        }
        TAILQ_INSERT_TAIL(&(q->iface), iface, tailq);
    }
    return 0;
    //TODO: release all re-sources
bail:
    return 1;
}
