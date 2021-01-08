#include "base_consumer.h"
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

class rebalance_callback: public RdKafka::RebalanceCb
{
public:
    void rebalance_cb(RdKafka::KafkaConsumer *consumer,
                      RdKafka::ErrorCode err,
                      std::vector<RdKafka::TopicPartition *> &partitions)
    {
        for(auto iter : partitions)
        {
            LOG(INFO) << "Rebalance topic" << iter->topic() << "," << iter->partition();
        }
        if(err == RdKafka::ERR__ASSIGN_PARTITIONS )
        {
            LOG(INFO) << "ASSIGN partitions";
            consumer->assign(partitions);
        }
        // else if(err == RdKafka::ERR__REVOKE_PARTITIONS)
        // {

        // }
        else
        {
            LOG(DEBUG) << "Rebalance callback " << err;
        }

    }
};
event_callback eventcb;
rebalance_callback rebalancecb;
Base_consumer::Base_consumer()
    :m_topic(""),m_groupid("")
{

}

bool Base_consumer::Init(std::string broker,std::string topic,std::string groupid)
{
    std::string errstr;
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    // RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    // conf->set("rebalance_cb", &rebalancecb, errstr);
    conf->set("event_cb", &eventcb, errstr);
    conf->set("metadata.broker.list", broker, errstr);
    conf->set("enable.partition.eof", "true", errstr);
    if (conf->set("group.id", groupid, errstr) != RdKafka::Conf::CONF_OK)
    {
        LOG(ERROR) << errstr;
        return false;
    }
    m_consumer = RdKafka::KafkaConsumer::create(conf, errstr);
    if (!m_consumer)
    {
        LOG(ERROR) << "Failed to create consumer: " << errstr;
        return false;
    }
    m_topic = topic;
    m_groupid = groupid;
    delete conf;
    return true;
}

void Base_consumer::msg_consume(RdKafka::Message *message, void *opaque)
{
    switch (message->err())
    {
    case RdKafka::ERR__TIMED_OUT:
        break;

    case RdKafka::ERR_NO_ERROR:
        /* Real message */

        RdKafka::MessageTimestamp ts;
        ts = message->timestamp();
        if (ts.type != RdKafka::MessageTimestamp::MSG_TIMESTAMP_NOT_AVAILABLE)
        {
            std::string tsname = "?";
            if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_CREATE_TIME)
                tsname = "create time";
            else if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_LOG_APPEND_TIME)
                tsname = "log append time";
            LOG(INFO) << "Timestamp: " << tsname << " " << ts.timestamp ;
        }
        LOG(DEBUG) <<"partition("<< message->partition() << ") msg key :" << message->key() << " ,msg payload :" << std::string(static_cast<char *>(message->payload()),message->len());
        break;

    case RdKafka::ERR__PARTITION_EOF:
        /* Last message */
        LOG(INFO) << message->partition() << " partition eof ";
        break;


    default:
        /* Errors */
        LOG(ERROR) << "unkonw type " <<message->err() << " " << message->errstr();
    }
}

void Base_consumer::process()
{
    std::vector<std::string> topics;
    topics.push_back(m_topic);
    RdKafka::ErrorCode err = m_consumer->subscribe(topics);
    if (err)
    {
        LOG(ERROR) << "Failed to subscribe " ;
        return;
    }
    try
    {
        while (m_bRun)
        {
            RdKafka::Message *msg = m_consumer->consume(1000);
            msg_consume(msg, NULL);
            delete msg;
        }
    }
    catch(...)
    {
        std::cout<< 1;
    }
}