
#include <iostream>
#include "server/server.h"
#include "log/log.h"


int main(int argc, char **argv)
{
    LOGINFO("START");
    LOGS << "I'm Server" << LOGE;

    Server myServer;
    
    if (myServer.CreateListener(8989, "127.0.0.1") == ServerStatus::SUCCESS){
        LOGS << "Server is listenning ... " << LOGE;
        myServer.DoWork(7);
    }

    return 0;
}
