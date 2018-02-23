#include <iostream>
#include "client/client.h"
#include "log/log.h"


int main(int argc, char **argv)
{
    LOGS << "I'm Client" << LOGE;

    Client myClient;
    if ( myClient.Connect("47.93.53.181", 8989) != ClientStatus::SUCCESS ){
        return -1;
    }
//    if ( myClient.Connect("127.0.0.1", 8989) != ClientStatus::SUCCESS ){
//        return -1;
//    }

    for (int i = 0;true; ++i){

        char sss[100];
        sprintf(sss, "hi, server. I'm %09d", i);

        myClient.Snd(sss);

        std::string ss;
        myClient.Rcv(ss);

        LOGS << ss << LOGE;
    }

    return 0;
}
