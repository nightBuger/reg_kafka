#pragma once
#include <librdkafka/rdkafkacpp.h>
#include <iostream>
#include <vector>
#include "thread.h"

class Base_consumer : public Thread
{
public:
    Base_consumer();
    bool Init(std::string broker,std::string topic,std::string groupid);
protected:
    virtual void process();
private:
    // virtual void msg_cb(std::string key,char *buf,size_t buflen) = 0;
    void msg_consume(RdKafka::Message *message, void *opaque);

    std::string m_topic;
    std::string m_groupid;

    RdKafka::KafkaConsumer *m_consumer;
};