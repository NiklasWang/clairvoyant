#ifndef __QUEUEC_H__
#define __QUEUEC_H__

#include <pthread.h>
#include "clist.h"

namespace pandora {

typedef bool (*match_fn_data_c)(void *data, void *user_data, void *match_data);
typedef void (*release_data_fn_c)(void* data, void *user_data);
typedef bool (*match_fn_c)(void *data, void *user_data);

class QueueC {
public:
    QueueC();
    QueueC(release_data_fn_c data_rel_fn, void *user_data);
    virtual ~QueueC();
    void init();
    bool enqueue(void *data);
    bool enqueueWithPriority(void *data);
    /* This call will put queue into uninitialized state.
     * Need to call init() in order to use the queue again */
    void flush();
    void flushNodes(match_fn_c match);
    void flushNodes(match_fn_data_c match, void *spec_data);
    void* dequeue(bool bFromHead = true);
    void* dequeue(match_fn_data_c match, void *spec_data);
    void* peek();
    bool isEmpty();
    int size();
    int getCurrentSize();

private:
    typedef struct {
        struct clist list;
        void* data;
    } q_node;

    q_node m_head; // dummy head
    int m_size;
    bool m_active;
    pthread_mutex_t m_lock;
    release_data_fn_c m_dataFn;
    void * m_userData;
};

};

#endif
