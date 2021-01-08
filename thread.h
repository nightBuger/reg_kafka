#include <pthread.h>

class Thread{

public:
    Thread();
    virtual bool start_thread();

protected:
    virtual void process() = 0;
    bool m_bRun;
private:
    static void *thread_process(void* self);
    pthread_t m_tid;
};