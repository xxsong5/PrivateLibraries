#include <thread>
#include "router/router.h"
#include "log/log.h"




int main(int argc, char **argv)
{
    LOGINFO("startting ...");
    Router  routerWithProxy(0, 8989, "127.0.0.1", 2000);

    routerWithProxy.SetRemoteServerProxy("172.17.18.80", 8080);
//    routerWithProxy.SetOrangeServerInfo("47.93.53.181", 8989);

    LOGINFO("I'm running...");
    routerWithProxy.Run();

    LOGINFO("the main thread is sleeping now");

    std::this_thread::sleep_for(std::chrono::duration<int>(10000));

    routerWithProxy.Stop();

    return 0;
}