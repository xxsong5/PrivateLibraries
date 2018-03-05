#pragma once

#ifndef __linux__
#include "StdAfx.h"  
#include <WinSock2.h>  
#else
#include <unistd.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#endif

#include <stdio.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "boost/thread/thread.hpp"  
#include "boost/bind.hpp"
#include "boost/thread/mutex.hpp"
#include <unordered_map>
#include "proxy/Proxy.h"
#include "socket/socket.h"
#include "defines/define.h"

typedef Status  RouterStatus;
typedef bool (*ProcesserFunc)(SOCKET fd, const std::string &request, std::string &response);

class Router : public Socket, public CProxy
{
public:    
    // type 0 http
    // type 1 sock4
    // type 2 sock5
    Router(long type= 0L, u_short listenPort=8989, const std::string &listenIP="127.0.0.1", long maxConnect=1000);
    ~Router();

    RouterStatus SetOrangeServerInfo(const std::string &OrangeServerIP,
                                     u_short OrangeServerPort, 
                                     const std::string &OrangeServerUserName = "", 
                                     const std::string &OrangeServerPassword = "");


    RouterStatus SetRemoteServerProxy(const std::string &ProxyIP,
                                      u_short ProxyPort, 
                                      const std::string &ProxyUserName = "", 
                                      const std::string &ProxyPassword = "");

    void SetOrangeServerAuthority(bool hasAuthority);

    void addSwitchRule(const std::string &url);

    void Run(int threadCounts = 1);
    void Wait(int minutes = 0);
    void Stop();


private:

    void keepOrangeServerProxyAlive();

    RouterStatus CreateListener(u_short localPort, std::string localIp);

    void DoWork(size_t threadCounts);
    void MainProcess(SOCKET epoll_fd);

    SOCKET ConnectByRequest(const std::string &request);

    int getHostPortbyHttpRequest(const std::string &httpRequest, std::string &url);

    bool switchRuleMatched(const std::string &url);


    bool RoundBack(SOCKET srcfd, char *rcvRaw, size_t len);

    void AddEpollSocket(SOCKET fd, uint32_t events = EPOLLIN|EPOLLET);
    void RemoveEpollSocket(SOCKET fd, uint32_t events = EPOLLIN|EPOLLET);
    void RemoveEpollSocket(struct epoll_event   *epoll);


    void fillTargetFDbySourceFD(SOCKET indexfd, SOCKET targetfd);
    void fillSourceFD(SOCKET sourcefd);
    void dropPairFDsBySourceFD(SOCKET srcFd);
    void dropPairFDsByTargetFD(SOCKET tgtFd);
    void dropPairFDsByAnyOne(SOCKET fd);
    SOCKET getTargetFDbySourceFD(SOCKET srcFd); // invaild if less than 1
    SOCKET getSourceFDbyTargetFD(SOCKET tgtFd); // invaild if less than 1
    std::mutex  m_FDsMutex;
    std::unordered_map<SOCKET, SOCKET>  m_pairFDsSourceTarget;
    std::unordered_map<SOCKET, SOCKET>  m_pairFDsTargetSource;

private:

    //router interface
    std::string              m_listenIP;
    u_short                  m_listenPort;
    SOCKET                   m_listenSocket;
    const int                m_maxConnect;
    long                     m_routerProtocolType;

    SOCKET                   m_orangeServerSocket;

    SOCKET                   m_epollFD;

    boost::shared_mutex      m_switchRuleSharedMutex;
    std::vector<std::string> m_switchRules; //*NOTE* call directly or through OrangeServer, all matched will call through Orangeserver


    //OrangeServerInformation
    std::string     m_orangeServerIP;
    u_short         m_orangeServerPort;

    std::string     m_orangeServerUserName;
    std::string     m_orangeServerPassword;
    bool            m_hasOrangeServerAuthority;


    // Proxy Information
    std::string     m_proxyIP;
    u_short         m_proxyPort;

    std::string     m_proxyUserName;
    std::string     m_proxyPassword;

    //running thread information
    volatile bool   m_doRun;
    std::vector< std::thread >  m_threadPools;

};


