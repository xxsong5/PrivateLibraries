#pragma once

#ifndef __linux__
#include "StdAfx.h"  
#include <WinSock2.h>  
#include <cstring>
#else
#include <stdio.h>
#include <unistd.h> 
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#endif

#include "defines/define.h"
#include "proxy/Proxy.h"
#include "socket/socket.h"

typedef Status ClientStatus;

class Client : public Socket {

public:
    // type 0 http
    // type 1 sock4
    // type 2 sock5
    // ip port username password (proxy server's info)
    Client(long type=-1L, std::string ip="", u_short port=0, std::string username="", std::string password="")  
        :m_localProxy(type, ip, port, username, password) {}  


    ~Client() {
        close(m_socket);
    }
    

    ClientStatus Connect(std::string ServerIp, u_short ServerPort);


    int Snd(const std::string  &strSnd);
    int Rcv(std::string &strRcv);

    int Snd(const char* snd, size_t len);
    int Rcv(char **rcv);

private:

    CProxy  m_localProxy;
    SOCKET  m_socket;

};