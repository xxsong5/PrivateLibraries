#include "router.h"
#include "proxy/Proxy.h"
#include "log/log.h"

#include <chrono>
#include <ratio>

using namespace std;


Router::Router(long type, u_short listenPort, const std::string &listenIP, long maxConnect): 
m_listenIP(listenIP), m_listenPort(listenPort), m_maxConnect(maxConnect), m_routerProtocolType(type)
{
    m_listenSocket  =   -1;
    m_orangeServerSocket    =   -1;
    m_hasOrangeServerAuthority  =   false;

    m_epollFD   =   -1;

    addSwitchRule("google");
    addSwitchRule("youtube");
}

Router::~Router()
{
    // waitting all thread done
    for (size_t i = 0; i < m_threadPools.size(); ++i){
        m_threadPools[i].join();
    }

    //close left sockets
    for (auto iter = m_pairFDsSourceTarget.begin(); iter != m_pairFDsSourceTarget.end(); ++iter){
        close(iter->first);
        LOGINFO(iter->first);
    }

    LOGINFO("--");

    for (auto iter = m_pairFDsTargetSource.begin(); iter != m_pairFDsTargetSource.end(); ++iter){
        close(iter->first);
        LOGINFO(iter->first);
    }

    //print current rules info
    LOGINFO("all rules are : ");
    for(size_t i = 0, end = m_switchRules.size(); i < end; ++i){
        LOGINFO(m_switchRules[i]);
    }
}


void Router::Run(int threadCounts)
{
    if (m_listenIP.empty() || m_listenPort <= 0){
        LOGERROR("listenning IP or Port is empty");
        return;
    }

    if (CreateListener(m_listenPort, m_listenIP) != RouterStatus::SUCCESS){
        LOGERROR("create listenning socket failed");
        return;
    }

    SetProxy(m_routerProtocolType, m_proxyIP, m_proxyPort, m_proxyUserName, m_proxyPassword);

    m_doRun = true;

    DoWork(threadCounts);
}



void Router::Wait(int minutes)
{
    if (minutes > 0){
        std::this_thread::sleep_for(std::chrono::duration<int, std::ratio<1,1> >(minutes*60));
    }
}


void Router::Stop()
{
    m_doRun = false;
}

RouterStatus Router::SetOrangeServerInfo(const std::string &OrangeServerIP,
                                 u_short OrangeServerPort, 
                                 const std::string &OrangeServerUserName, 
                                 const std::string &OrangeServerPassword)
{
    m_orangeServerIP    =   OrangeServerIP;
    m_orangeServerPort  =   OrangeServerPort;
    m_orangeServerUserName  =   OrangeServerUserName;
    m_orangeServerPassword  =   OrangeServerPassword;

    return RouterStatus::SUCCESS;
}



 RouterStatus Router::SetRemoteServerProxy(const std::string &ProxyIP,
                                   u_short ProxyPort, 
                                   const std::string &ProxyUserName, 
                                   const std::string &ProxyPassword)
{
    m_proxyIP   =   ProxyIP;
    m_proxyPort =   ProxyPort;
    m_proxyUserName =   ProxyUserName;
    m_proxyPassword =   ProxyPassword;

    return RouterStatus::SUCCESS;
}



void Router::SetOrangeServerAuthority(bool hasAuthority)
{
    m_hasOrangeServerAuthority = hasAuthority;
}


RouterStatus Router::CreateListener(u_short localPort, std::string localIp)
{
    SOCKET fd;
 
    //创建一个inet类型的socket
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
        return RouterStatus::CREATE_LISTENER_FAIL;
 
 
    /* SO_REUSEADDR，只定义一个套接字在一个端口上进行监听,
     * 如果服务器出现意外而导致没有将这个端口释放，
     * 那么服务器重新启动后，你还可以用这个端口，因为你已经规定可以重用了，
     如果你没定义的话，你就会得到提示，ADDR已在使用中 
     */
    int opt=1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(opt));


    //设置非阻塞方式连接  
#ifndef __linux__
    unsigned long ul = 1;  
    int ret = ioctlsocket(fd, FIONBIO, (unsigned long*)&ul);  
#else
    int ret = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
#endif
 
 
    /* sockaddr结构体的缺陷：sa_data把目标地址和端口信息混在一起了
     * sockaddr_in 结构体: 把port和addr 分开储存在两个变量中
     * sockaddr 和 sockaddr_in的相互关系
     */
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(struct sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(localPort);       // 指定port端口
//    sin.sin_addr.s_addr = INADDR_ANY; // ip地址不做绑定使用机器默认的ip地址
#ifndef __linux__
    sin.sin_addr.S_un.S_addr = inet_addr(localIp.c_str());  
#else
    sin.sin_addr.s_addr = inet_addr(localIp.c_str());  
#endif

    if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) != 0){
        close(fd);
        return RouterStatus::CREATE_LISTENER_FAIL;
    }
    
 
    // 开始侦听该端口
    if (listen(fd, 32) != 0){
        close(fd);
        return RouterStatus::CREATE_LISTENER_FAIL;
    }
 
    m_listenSocket  =   fd;
    m_listenIP      =   localIp;
    m_listenPort    =   localPort;

    return RouterStatus::SUCCESS;
}


void Router::DoWork(size_t threadCounts)
{
    threadCounts = threadCounts > 100 ? 100 : threadCounts;
    threadCounts = threadCounts < 1 ? 1 : threadCounts;

    //1. 创建epoll描述符, 可接受连接数
    if ((m_epollFD = epoll_create(m_maxConnect)) < 0){
        LOGERROR("create epoll failed");
        return;
    }
 
    //2. 创建、绑定和侦听socket; 并且将它放到epoll上面开始侦听数据到达和connect连接
    epoll_event ev;
    ev.data.fd = m_listenSocket;
    ev.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(m_epollFD, EPOLL_CTL_ADD, ev.data.fd, &ev) < 0){
        char Info[1000];
        sprintf(Info, "add listen fd failed!, which is belongs to %s:%d", m_listenIP.c_str(), m_listenPort);
        LOGERROR(Info);
        return;
    }


    for (int i = 0; i < threadCounts; ++i){
        m_threadPools.emplace_back(&Router::MainProcess, this, m_epollFD);
    }
}


void Router::MainProcess(SOCKET epoll_fd)
{
    LOGWARN("Waiting for service ...");

    while (m_doRun)
    {
        // 等待epoll上面的事件触发，然后将他们的fd检索出来
        struct epoll_event events[m_maxConnect];
        int evn_cnt = epoll_wait(epoll_fd, events, m_maxConnect, -1);
 
        // 循环处理检索出来的events队列中的每个fd
        for (int i=0; i<evn_cnt; i++)
        {
            if (events[i].data.fd == m_listenSocket)   // 有客户端connect过来
            {
                struct sockaddr_in caddr;
                socklen_t clen = sizeof(caddr);
 
                int connfd = accept(m_listenSocket, (sockaddr *)&caddr, &clen);
                if (connfd < 0)
                    continue;
                
                fillSourceFD(connfd);
            }
            else if (events[i].events & EPOLLIN)       // 有数据到达
            {
                SOCKET targetFD = getTargetFDbySourceFD(events[i].data.fd);
                SOCKET sourceFD = getSourceFDbyTargetFD(events[i].data.fd);

                //接收数据
                char *rcvDatas = NULL;
                int rcvLen = Rcv(events[i].data.fd, &rcvDatas);

                if (rcvLen <= 0){

                    close(events[i].data.fd);

                    if (targetFD > 0){
                        close(targetFD);
                    }
                    if (sourceFD > 0){
                        close(sourceFD);
                    }

                    dropPairFDsByAnyOne(events[i].data.fd);

                    LOGS << "SOCKET " << events[i].data.fd <<" with " << (targetFD > 0 || targetFD == SOCKET_NO_PAIR ? targetFD : sourceFD) <<" closed" << LOGE;
                    continue;
                }


                std::string strSnd; int  sndSize{-1};
                bool success = true;

                if (sourceFD > 0 && targetFD < 1){
                    //right hand Rcved
                    sndSize = Socket::Snd(sourceFD, rcvDatas, rcvLen);
                    //LOGINFO(sourceFD);
                    //LOGINFO(std::string(rcvDatas, rcvLen));

                } else if (targetFD > 0 || targetFD == SOCKET_NO_PAIR){
                    //left hand Rcved or first connected
                    if (targetFD == SOCKET_NO_PAIR){
                        targetFD = ConnectByRequest(rcvDatas);
                        //LOGERROR(targetFD); 
                        //LOGERROR(std::string(rcvDatas, rcvLen));

                        if (targetFD > 0){
                            fillTargetFDbySourceFD(events[i].data.fd, targetFD);
                            if (RoundBack(events[i].data.fd, rcvDatas, rcvLen)){
                                continue;
                            }
                        } else {
                            success = false;
                        }
                    }

                    if (success){
                        //LOGWARN(targetFD); 
                        //LOGWARN(std::string(rcvDatas, rcvLen));
                        sndSize = Socket::Snd(targetFD, rcvDatas, rcvLen);
                    }

                } else {
                    success = false;
                }
                    
                if (!success || sndSize <= 0){
                    close(events[i].data.fd);

                    if (targetFD > 0){
                        close(targetFD);
                    }
                    if (sourceFD > 0){
                        close(sourceFD);
                    }

                    dropPairFDsByAnyOne(events[i].data.fd);

                    LOGS << "SOCKET nosndSuccess " << events[i].data.fd <<" with " << (targetFD > 0 || targetFD == SOCKET_NO_PAIR ? targetFD : sourceFD) <<" closed" << LOGE;
                }

                free(rcvDatas);
            }
        }
    }

}

bool Router::switchRuleMatched(const std::string &url)
{
    boost::shared_lock<boost::shared_mutex>  slck(m_switchRuleSharedMutex);
    
    auto iter = std::find_if(m_switchRules.begin(), m_switchRules.end(), [&](const std::string &rule){
        return url.find(rule) != std::string::npos ? true : false;
    });

    if (iter != m_switchRules.end()){
        return true;
    }

    return false;
}

void Router::addSwitchRule(const std::string &RawAtom)
{
    if (!switchRuleMatched(RawAtom)){
        boost::unique_lock<boost::shared_mutex>  ulck(m_switchRuleSharedMutex);
        m_switchRules.push_back(RawAtom);
    }
}

int Router::getHostPortbyHttpRequest(const std::string &httpRequest, std::string &url)
{
    const std::string hostTag = "Host: ";
    const std::string endTag  = "\r\n";

    size_t startpos = httpRequest.find(hostTag);

    if (startpos != std::string::npos){
        startpos += hostTag.length();
        size_t endpos = httpRequest.find(endTag, startpos);

        if (endpos != std::string::npos){
            std::string strhost = httpRequest.substr(startpos, endpos - startpos);

            size_t segmentpos   =   strhost.find(":");
            if (segmentpos != std::string::npos){
                url =   strhost.substr(0, segmentpos);
                return std::stoi(strhost.substr(segmentpos+1, strhost.length()- (segmentpos+1)));
            }else{
                url =   strhost;
                return 80; //如果url中没有指明端口号，就设定默认的80  
            }
        }
    }

    return -1;
}

SOCKET Router::ConnectByRequest(const std::string &request)
{
    //get remotehost by request
    string  _host = "";  
    int     _port = getHostPortbyHttpRequest(request, _host);


    sockaddr_in addr_server;  
    memset(&addr_server, 0, sizeof(addr_server));

    if (!m_hasOrangeServerAuthority || !switchRuleMatched(_host)){

        SOCKET sock;
        if (HasProxyServer()){

            addr_server.sin_family = AF_INET;
            addr_server.sin_port = htons(GetProxyPort());
            addr_server.sin_addr.s_addr = inet_addr(GetProxyIP().c_str());

            sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);  
        } else {
            struct hostent *p_hostent = gethostbyname(_host.c_str());  
            if(p_hostent == NULL) {
                return -1;  
            }
            addr_server.sin_family = AF_INET;  
            addr_server.sin_port = htons(_port);
            memcpy(&(addr_server.sin_addr),p_hostent->h_addr_list[0],sizeof(addr_server.sin_addr));  

            sock = socket(p_hostent->h_addrtype,SOCK_STREAM,IPPROTO_TCP);  
        }

        int res = connect(sock,(sockaddr*)&addr_server,sizeof(addr_server));  

        if(res < 0){
            close(sock);
            return -1;
        } else {
            return sock;
        }

    } else {

        SOCKET sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);  
        if (HasProxyServer()){

            if (ConnectProxyServer(sock) != ProxyStatus::SUCCESS){
                close(sock);
                return -1;
            }

            if (ConnectServer(sock, m_orangeServerIP, m_orangeServerPort) == ProxyStatus::SUCCESS){
                return sock;
            } else {
                close(sock);
                return -1;
            }

        } else {

            addr_server.sin_family = AF_INET;
            addr_server.sin_port = htons(m_orangeServerPort);
            addr_server.sin_addr.s_addr = inet_addr(m_orangeServerIP.c_str());
            if (connect(sock,(sockaddr*)&addr_server,sizeof(addr_server)) >= 0){
                return sock;
            } else {
                close(sock);
                return -1;
            }
        }
    }
}

void Router::AddEpollSocket(SOCKET fd, uint32_t events)
{
    if (fd > 0){
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = events;
        epoll_ctl(m_epollFD, EPOLL_CTL_ADD, fd, &ev);
    }
}

void Router::RemoveEpollSocket(SOCKET fd, uint32_t events)
{
    if (fd > 0){
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = events;
//        epoll_ctl(m_epollFD, EPOLL_CTL_DEL, fd, &ev);
    }
}

void Router::RemoveEpollSocket(struct epoll_event   *ev)
{
}


void Router::fillSourceFD(SOCKET sourcefd)
{
    {
        std::lock_guard<std::mutex>  lck(m_FDsMutex);
        if (m_pairFDsSourceTarget.find(sourcefd) == m_pairFDsSourceTarget.end()){
            m_pairFDsSourceTarget[sourcefd] = SOCKET_NO_PAIR;
        }
    }

    AddEpollSocket(sourcefd);
}

void Router::fillTargetFDbySourceFD(SOCKET indexfd, SOCKET targetfd)
{
    {
        std::lock_guard<std::mutex>  lck(m_FDsMutex);

        m_pairFDsSourceTarget[indexfd]  = targetfd;

        m_pairFDsTargetSource[targetfd] = indexfd;
    }

    AddEpollSocket(targetfd);
}

void Router::dropPairFDsBySourceFD(SOCKET srcFd)
{
    std::lock_guard<std::mutex>  lck(m_FDsMutex);
    if (m_pairFDsSourceTarget.find(srcFd) != m_pairFDsSourceTarget.end()){
        SOCKET targetfd = m_pairFDsSourceTarget[srcFd];
        m_pairFDsSourceTarget.erase(srcFd);
        if (targetfd != SOCKET_NO_PAIR){
            m_pairFDsTargetSource.erase(targetfd);
            RemoveEpollSocket(targetfd);
        }
    }

    RemoveEpollSocket(srcFd);
}

void Router::dropPairFDsByTargetFD(SOCKET tgtFd)
{
    std::lock_guard<std::mutex>  lck(m_FDsMutex);
    if (m_pairFDsTargetSource.find(tgtFd) != m_pairFDsTargetSource.end()){
        SOCKET sourcefd = m_pairFDsTargetSource[tgtFd];
        m_pairFDsTargetSource.erase(tgtFd);
        if (sourcefd > 0){
            m_pairFDsSourceTarget.erase(sourcefd);
            RemoveEpollSocket(sourcefd);
        }
    }

    RemoveEpollSocket(tgtFd);
}


void Router::dropPairFDsByAnyOne(SOCKET fd)
{
    std::lock_guard<std::mutex>  lck(m_FDsMutex);
    if (m_pairFDsTargetSource.find(fd) != m_pairFDsTargetSource.end()){
        SOCKET sourcefd = m_pairFDsTargetSource[fd];
        m_pairFDsTargetSource.erase(fd);
        if (sourcefd > 0){
            m_pairFDsSourceTarget.erase(sourcefd);
            RemoveEpollSocket(sourcefd);
        }
    } else if (m_pairFDsSourceTarget.find(fd) != m_pairFDsSourceTarget.end()){
        SOCKET targetfd = m_pairFDsSourceTarget[fd];
        m_pairFDsSourceTarget.erase(fd);
        if (targetfd != SOCKET_NO_PAIR){
            m_pairFDsTargetSource.erase(targetfd);
            RemoveEpollSocket(targetfd);
        }
    }

    RemoveEpollSocket(fd);
}

SOCKET Router::getTargetFDbySourceFD(SOCKET srcFd) // invaild if less than 1
{
    std::lock_guard<std::mutex>  lck(m_FDsMutex);
    
    if (m_pairFDsSourceTarget.find(srcFd) != m_pairFDsSourceTarget.end()){
        return m_pairFDsSourceTarget[srcFd];
    }

    return -1;
}

SOCKET Router::getSourceFDbyTargetFD(SOCKET tgtFd) // invaild if less than 1
{
    std::lock_guard<std::mutex>  lck(m_FDsMutex);
    
    if (m_pairFDsTargetSource.find(tgtFd) != m_pairFDsTargetSource.end()){
        return m_pairFDsTargetSource[tgtFd];
    }

    return -1;
}


bool Router::RoundBack(SOCKET srcfd, char *rcvRaw, size_t len)
{
    std::string strRaw(rcvRaw, len);

    if (m_proxyIP.empty() && strRaw.find("CONNECT") == 0){

        std::string strSnd("HTTP/1.1 200 Connection established\r\n\r\n");
        Socket::Snd(srcfd, strSnd);

        return true;
    }

    return false;
}