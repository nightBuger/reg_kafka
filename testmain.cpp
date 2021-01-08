#include "base_consumer.h"
#include "easylogging++.h"
#include <unistd.h>
INITIALIZE_EASYLOGGINGPP

int main()
{
    el::Configurations conf("./log.conf");
    el::Loggers::reconfigureLogger("default", conf);

    el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);

    Base_consumer consumer;
    consumer.Init("120.53.15.76","req","testgroup");
    consumer.start_thread();
    sleep(999999999);
    return 0;
}