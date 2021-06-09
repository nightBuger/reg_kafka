#pragma once
#include <librdkafka/rdkafkacpp.h>
#include "easylogging++.h"

class event_callback : public RdKafka::EventCb
{
public:
    void event_cb(RdKafka::Event &event)
    {
        switch (event.type())
        {
        case RdKafka::Event::EVENT_ERROR :
            LOG(ERROR) <<"EVENT_ERROR:"<< event.err() << event.str();
            break;
        case RdKafka::Event::EVENT_LOG:
            LOG(ERROR) <<"EVENT_LOG:"<< event.err();
            break;
        case RdKafka::Event::EVENT_STATS:
            LOG(ERROR) <<"EVENT_STATS:"<< event.err();
            LOG(ERROR) <<"EVENT_STATS event.str:"<< event.str();
            break;
        case RdKafka::Event::EVENT_THROTTLE:
            LOG(ERROR) <<"EVENT_THROTTLE:"<< event.err();
            LOG(ERROR) <<"EVENT_THROTTLE event.str:"<< event.str();
            break;
        default:
            break;
        }
    }
};

class delivery_callback : public RdKafka::DeliveryReportCb
{
public:
    void dr_cb(RdKafka::Message &message)
    {

        if (message.err())
            LOG(ERROR) << message.errstr();
        else
            LOG(INFO) <<"partition :" << message.partition() << "<"<<*message.key() << "> send ok payload :" << static_cast<char*>(message.payload()) ; 
    }
};

// class rebalance_callback: public RdKafka::RebalanceCb
// {
// public:
//     void rebalance_cb(RdKafka::KafkaConsumer *consumer,
//                       RdKafka::ErrorCode err,
//                       std::vector<RdKafka::TopicPartition *> &partitions)
//     {
//         for(auto iter : partitions)
//         {
//             LOG(INFO) << "Rebalance topic" << iter->topic() << "," << iter->partition();
//         }
//         if(err == RdKafka::ERR__ASSIGN_PARTITIONS )
//         {
//             LOG(INFO) << "ASSIGN partitions";
//             consumer->assign(partitions);
//         }
//         // else if(err == RdKafka::ERR__REVOKE_PARTITIONS)
//         // {

//         // }
//         else
//         {
//             LOG(DEBUG) << "Rebalance callback " << err;
//         }

//     }
// };