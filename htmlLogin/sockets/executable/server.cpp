
#include <iostream>
#include "server/server.h"
#include "log/log.h"


int main(int argc, char **argv)
{
    LOGS << "I'm Server" << LOGE;

    Server myServer;
    
    if (myServer.CreateListener(8787) == ServerStatus::SUCCESS){
        LOGS << "Server is listenning ... " << LOGE;
        myServer.DoWork(7);
    }

    return 0;
}