#include "base_producer.h"
#include "easylogging++.h"
#include "callback.h"

event_callback eventcb_2;
delivery_callback deliverycb;


Base_producer::Base_producer()
    :m_topic(""),m_producer(nullptr)
{

}

bool Base_producer::Init(std::string broker,std::string topic)
{
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    std::string errstr;
    if (conf->set("bootstrap.servers", broker, errstr) !=RdKafka::Conf::CONF_OK)
    {
        LOG(ERROR) << errstr;
        return false;
    }

    conf->set("event_cb", &eventcb_2, errstr);

    if (conf->set("dr_cb", &deliverycb, errstr) != RdKafka::Conf::CONF_OK)
    {
        LOG(ERROR) << errstr;
        return false;
    }
    m_producer = RdKafka::Producer::create(conf, errstr);
    m_topic = topic;
    return true;
}

bool Base_producer::msg_produce(std::string key,char *payload,size_t payload_len)
{
    RdKafka::ErrorCode err =
        m_producer->produce(m_topic,
                            RdKafka::Topic::PARTITION_UA,
                            RdKafka::Producer::RK_MSG_COPY,
                            payload, payload_len,
                            key.c_str(), key.size(),
                            0,
                            NULL);
    if (err != RdKafka::ERR_NO_ERROR)
    {
        LOG(ERROR) << "produce <" << key << "> error ,errcode " << err << " payload :" << payload;
        return false;
    }
    else
    {
        LOG(INFO) << "produce <" << key << "> success, payload :" << payload;
        return true;
    }
    
}

void Base_producer::process()
{
    try
    {
        while (m_bRun)
        {
            int ret = m_producer->poll(-1);
            LOG(INFO) << "producer poll return " << ret;
        }
    }
    catch(...)
    {
        LOG(ERROR) << "producer poll throw an error";
    }
}