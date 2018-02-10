#pragma once  
  
#include <string>  
#include <vector>  
#include "defines/define.h"

#ifndef __linux__
#include <WinSock2.h>  
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif
  


typedef Status ProxyStatus;
  
class CProxy  
{  
public:  
    CProxy(long type, std::string ip, u_short port, std::string username, std::string password)  
        :m_proxyType(type), m_proxyIp(ip), m_proxyPort(port), m_proxyUserName(username), m_proxyUserPwd(password)  
    {}  
  
    ~CProxy(void){};  
  
    ProxyStatus ConnectProxyServer(SOCKET socket);  
    bool        HasProxyServer();
    ProxyStatus ConnectServer(SOCKET socket, std::string ip, u_short port);  
  
private:  
    ProxyStatus ConnectByHttp(SOCKET socket,  std::string ip, u_short port);  
    ProxyStatus ConnectBySock4(SOCKET socket, std::string ip, u_short port);  
    ProxyStatus ConnectBySock5(SOCKET socket, std::string ip, u_short port);  
  
    bool Send(SOCKET socket, const char* buf, int len);  
    int Receive(SOCKET socket, char* buf, int bufLen);  
  
private:  
    long m_proxyType;  
    std::string m_proxyIp;  
    u_short m_proxyPort;  
    std::string m_proxyUserName;  
    std::string m_proxyUserPwd;  
  
    bool m_blnProxyServerOk;  
};  