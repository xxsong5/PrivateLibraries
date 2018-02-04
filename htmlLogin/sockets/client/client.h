#pragma once

#ifndef __linux__
#include "StdAfx.h"  
#include <WinSock2.h>  
#else
#include <stdio.h>
#include <cstring>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#endif

#include "Proxy.h"

typedef ProxyStatus ClientStatus;

class Client {

public:
    // type 0 http
    // type 1 sock4
    // type 2 sock5
    // ip port username password (proxy server's info)
    Client(long type=-1L, string ip="", u_short port=0, string username="", string password="")  
        :m_localProxy(type, ip, port, username, password) {}  
    

    ClientStatus CreateLisener(u_short localPort = 8787, string localIp = "127.0.0.1");


    ClientStatus Connect(string ServerIp, u_short ServerPort);


    bool Transmitting();



    int Snd(SOCKET socketfd, const char* snd, size_t len);

    int Rcv(SOCKET socketfd, char **rcv);


private:
static void* epoll_func(void *p);
    


private:

    CProxy  m_localProxy;
    SOCKET  m_lisenSocket;

};