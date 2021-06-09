#pragma once
#include <librdkafka/rdkafkacpp.h>
#include "thread.h"
#include <iostream>

class Base_producer:public Thread
{
public:
    Base_producer();
    bool Init(std::string broker,std::string topic);
    bool msg_produce(std::string key,char *payload,size_t payload_len);
private:
    std::string m_topic;

    RdKafka::Producer *m_producer;
protected:
    virtual void process(); 
    /* data */
};