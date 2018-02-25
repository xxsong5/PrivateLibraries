#pragma once

#include "defines/define.h"

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

#include <string>
#include <vector>
#include "defines/define.h"


class Socket{

public:

    int Snd(SOCKET socketfd, const char* snd, size_t len);
    int Snd(SOCKET socketfd, const std::string &strSnd);

    int Rcv(SOCKET socketfd, char **rcv);
    int Rcv(SOCKET socketfd, std::string &strRcvd);
};