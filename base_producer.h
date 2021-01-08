#include <librdkafka/rdkafkacpp.h>
#include "thread.h"
#include <iostream>

class base_producer:public Thread
{
public:
    base_producer();
    bool Init(std::string broker,std::string topic);
private:

protected:
    virtual void process(); 
    /* data */
};