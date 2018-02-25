#include <iostream>
#include "client/client.h"
#include "log/log.h"


int main(int argc, char **argv)
{
    LOGS << "I'm Client" << LOGE;

    Client myClient(0, "172.17.18.80", 8080);
    if ( myClient.Connect("47.93.53.181", 8989) != ClientStatus::SUCCESS ){
        LOGS << "connected failed!" << LOGE;
        return -1;
    }

    for (int i = 0; i < 100; ++i){

        char sss[100];
        sprintf(sss, "hi, server. I'm %09d", i);

        myClient.Snd(sss);

        std::string ss;
        myClient.Rcv(ss);

        LOGS << ss << LOGE;
    }

    return 0;
}
