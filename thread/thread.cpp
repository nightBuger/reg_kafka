#include "thread.h"

void *Thread::thread_process(void* self)
{
    Thread* p_this = static_cast<Thread*>(self);
    if(p_this)
    {
        p_this->process();
    }
    p_this->m_bRun = false;
    return 0;
}

Thread::Thread():
    m_bRun(false),m_tid(0)
{

}

bool Thread::start_thread()
{
    m_bRun = true;
    return pthread_create(&m_tid,0,Thread::thread_process,this) == 0 ? true : m_bRun = false;
}