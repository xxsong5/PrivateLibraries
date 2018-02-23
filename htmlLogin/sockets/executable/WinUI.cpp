#include <iostream>
#include <string>
#include "client/client.h"
#include "server/server.h"
#include "log/log.h"


using namespace std;


bool MainProcess(const string &request, string &reponse);


int main(int argc, char **argv)
{
    LOGCFG(false);
    LOGS << "I'm WinUI " << LOGE;

    Server myServer;
    
    if (myServer.CreateListener(8989, "127.0.0.1") == ServerStatus::SUCCESS){
        LOGS << "I'm working ... " << LOGE;
        myServer.DoWork(7, MainProcess);
    }

    return 0;
}


bool MainProcess(const string &request, string &response)
{

    LOGERROR(request + "  + source");

    Client myClient;

    if ( myClient.Connect("47.93.53.181", 8989) != ClientStatus::SUCCESS ){
        return true;
    }

    myClient.Snd(request);

    myClient.Rcv(response);

    LOGERROR(response + "  + fromRemote");

    return true;
}
