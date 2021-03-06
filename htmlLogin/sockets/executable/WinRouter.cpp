#include <thread>
#include "router/router.h"
#include "log/log.h"




int main(int argc, char **argv)
{
    LOGCFG(false);
    LOGINFO("startting ...");
    Router  routerWithoutProxy(0, 8989, "127.0.0.1", 1000);

//    routerWithProxy.SetRemoteServerProxy("172.17.18.80", 8080);
    routerWithoutProxy.SetOrangeServerInfo("47.93.53.181", 8989);
    routerWithoutProxy.SetOrangeServerAuthority(true);

    LOGINFO("I'm running...");
    routerWithoutProxy.Run(2);

    LOGINFO("the main thread is sleeping now");

    std::this_thread::sleep_for(std::chrono::duration<int>(1000));

    routerWithoutProxy.Stop();

    return 0;
}