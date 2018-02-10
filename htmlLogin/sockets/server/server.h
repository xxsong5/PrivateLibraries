#pragma once

#ifndef __linux__
#include "StdAfx.h"  
#include <WinSock2.h>  
#include <cstring>
#else
#include <stdio.h>
#include <string>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#endif

#include <vector>

#include "defines/define.h"
#include "socket/socket.h"

typedef Status ServerStatus;

typedef void (*ProcesserFunc)(const std::string &request, std::string &response);

class Server: public Socket{

public:

    Server(unsigned int  maxConnec = 100):m_maxConnec(maxConnec){
    }


    ServerStatus CreateListener(u_short localPort = 8787, std::string localIp = "127.0.0.1");


    void DoWork(ProcesserFunc process = [](const std::string &request, std::string &response){
        response = "hello, I'm OrangeServer";
    });

    void DoWork(size_t threadCounts, ProcesserFunc process = [](const std::string &request, std::string &response){
        response = "hello, I'm OrangeServer";
    });


    const std::string& GetListenIP(){
        return m_listenIP;
    }


    u_short   GetListenPort(){
        return m_listenPort;
    }


    unsigned int GetMaxConnec(){
        return m_maxConnec;
    }


private:

    int Snd(SOCKET fd, const std::string &strSnd);


    int Rcv(SOCKET fd, std::string &strRcv);


    void MainProcess(SOCKET fd, ProcesserFunc process);

private:

    SOCKET          m_listenSocket;
    std::string     m_listenIP;
    u_short         m_listenPort;
    unsigned int    m_maxConnec;

    
};