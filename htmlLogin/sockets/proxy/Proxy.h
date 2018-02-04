#pragma once  
  
#include <string>  
#include <vector>  

#ifndef __linux__
#include <WinSock2.h>  
#else
#define SOCKET int
#define SOCKET_ERROR -1
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif
  
using namespace std;  
  
enum ProxyStatus  
{  
    SUCCESS,  
    CONNECT_PROXY_FAIL,  
    NOT_CONNECT_PROXY,  
    CONNECT_SERVER_FAIL, 
    CREATE_LISTENER_FAIL
};  
  
class CProxy  
{  
public:  
    CProxy(long type, string ip, u_short port, string username, string password)  
        :m_proxyType(type), m_proxyIp(ip), m_proxyPort(port), m_proxyUserName(username), m_proxyUserPwd(password)  
    {}  
  
    ~CProxy(void){};  
  
    ProxyStatus ConnectProxyServer(SOCKET socket);  
    ProxyStatus ConnectServer(SOCKET socket, string ip, u_short port);  
  
private:  
    ProxyStatus ConnectByHttp(SOCKET socket, string ip, u_short port);  
    ProxyStatus ConnectBySock4(SOCKET socket, string ip, u_short port);  
    ProxyStatus ConnectBySock5(SOCKET socket, string ip, u_short port);  
  
    bool Send(SOCKET socket, const char* buf, int len);  
    int Receive(SOCKET socket, char* buf, int bufLen);  
  
private:  
    long m_proxyType;  
    string m_proxyIp;  
    u_short m_proxyPort;  
    string m_proxyUserName;  
    string m_proxyUserPwd;  
  
    bool m_blnProxyServerOk;  
};  
  
struct TSock4req1   
{   
    char VN;   
    char CD;   
    unsigned short Port;   
    unsigned long IPAddr;   
    char other;   
};   
  
struct TSock4ans1   
{   
    char VN;   
    char CD;   
};  
  
struct TSock5req1   
{   
    char Ver;   
    char nMethods;   
    char Methods;   
};   
  
struct TSock5ans1   
{   
    char Ver;   
    char Method;   
};   
  
struct TSock5req2   
{   
    char Ver;   
    char Cmd;   
    char Rsv;   
    char Atyp;   
    char other;   
};   
  
struct TSock5ans2   
{   
    char Ver;   
    char Rep;   
    char Rsv;   
    char Atyp;   
    char other;   
};   
  
struct TAuthreq   
{   
    char Ver;   
    char Ulen;   
    char Name;   
    char PLen;   
    char Pass;   
};   
  
struct TAuthans   
{   
    char Ver;   
    char Status;   
};