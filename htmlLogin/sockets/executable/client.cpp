#include <iostream>
#include "client/client.h"
#include "log/log.h"


int main(int argc, char **argv)
{
    LOGS << "I'm Client" << LOGE;

    for (int i = 0;true ; ++i){
    LOGS << "1" << LOGE;
    Client myClient;
    LOGS << "2" << LOGE;
    if ( myClient.Connect("47.93.53.181", 8989) == ClientStatus::SUCCESS ){
    LOGS << "3" << LOGE;
        char hello[100]; sprintf(hello, "hello server, I'm %07d", i);
        std::string abc(hello);
    LOGS << "4" << LOGE;
        myClient.Snd(abc.c_str(), abc.length());
    LOGS << "5" << LOGE;
        char *rcvd;
        std::string ss(rcvd, myClient.Rcv(&rcvd));
    LOGS << "6" << LOGE;
        LOGS << ss << LOGE;
        free(rcvd);
    LOGS << "7" << LOGE;
    }
    }

    return 0;
}
