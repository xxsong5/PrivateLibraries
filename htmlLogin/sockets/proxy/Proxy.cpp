
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
#include "CBase64.h"  
#include "log/log.h"  
  
#include <time.h>  

  
ProxyStatus CProxy::ConnectProxyServer(SOCKET socket)  
{  
    int ret;  
    struct timeval timeout ;  
    fd_set r;  
    string ip;  
    u_short port;  
  
    ip = m_proxyIp;  
    port = m_proxyPort;  
  
    sockaddr_in servAddr;  
    memset(&servAddr, 0, sizeof(struct sockaddr_in));
    servAddr.sin_family = AF_INET;  
    servAddr.sin_port = htons(port);  
#ifndef __linux__
    servAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());  
#else
    servAddr.sin_addr.s_addr = inet_addr(ip.c_str());  
#endif
  
    //设置非阻塞方式连接  
#ifndef __linux__
    unsigned long ul = 1;  
    ret = ioctlsocket(socket, FIONBIO, (unsigned long*)&ul);  
#else
    ret = fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK);
#endif

    if(ret == SOCKET_ERROR)   
    {  
        return CONNECT_PROXY_FAIL;  
    }  
  
    connect(socket, (sockaddr*)&servAddr, sizeof(sockaddr));  
  
    FD_ZERO(&r);  
    FD_SET(socket, &r);  
    timeout.tv_sec = 5;   
    timeout.tv_usec =0;  
    ret = select(0, 0, &r, 0, &timeout);  
  
    if (ret <= 0)  
    {  
        m_blnProxyServerOk = false;  
        return CONNECT_PROXY_FAIL;  
    }  
    else  
    {  
        m_blnProxyServerOk = true;  
        return SUCCESS;  
    }  
}  
  
ProxyStatus CProxy::ConnectServer(SOCKET socket, string ip, u_short port)  
{  
    int ret;  
    int nTimeout;  
  
    if (!m_blnProxyServerOk)  
    {  
        return NOT_CONNECT_PROXY;  
    }  
  
    nTimeout = 5000;  
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nTimeout, sizeof(int));    //设置接收超时  
  
#ifndef __linux__
    unsigned long ul = 0;  
    ret = ioctlsocket(socket, FIONBIO, (unsigned long*)&ul);    //设置阻塞方式连接  
#else
    int oldfl = fcntl(socket, F_GETFL);
    if (oldfl != -1) {
        fcntl(socket, F_SETFL, oldfl & ~O_NONBLOCK);
    }
#endif
  
    switch(m_proxyType)  
    {  
    case 0: //HTTP  
        return ConnectByHttp(socket, ip, port);  
        break;  
    case 1: //SOCK4  
        return ConnectBySock4(socket, ip, port);  
        break;  
    case 2: //SOCK5  
        return ConnectBySock5(socket, ip, port);  
        break;  
    default:  
        break;  
    }  
  
    return CONNECT_SERVER_FAIL;  
}  
  
ProxyStatus CProxy::ConnectByHttp(SOCKET socket, string ip, u_short port)  
{  
    char buf[1024];  
  
    if (m_proxyUserName != "")  
    {  
        string str;  
        string strBase64;  
        str = m_proxyUserName + ":" + m_proxyUserPwd;  
        strBase64 = CBase64::Encode((unsigned char*)str.c_str(), str.length());  
        sprintf(buf, "CONNECT %s:%d HTTP/1.1\r\nHost: %s:%d\r\nAuthorization: Basic %s\r\n\r\nProxy-Authorization: Basic %s\r\n\r\n",   
            ip.c_str(), port, ip.c_str(), port, strBase64.c_str(), strBase64.c_str());  
    }  
    else  
    {  
        //sprintf_s(buf, 512, "CONNECT %s:%d HTTP/1.1\r\nHost: %s:%d\r\n\r\n", ip.c_str(), port, ip.c_str(), port);  
        sprintf(buf, "CONNECT %s:%d HTTP/1.1\r\nUser-Agent: MyApp/0.1\r\n\r\n", ip.c_str(), port);  
    }  
  
    Send(socket, buf, strlen(buf));  
    Receive(socket, buf, sizeof(buf));  
  
    if (strstr(buf, "HTTP/1.0 200 Connection established") != NULL)  
    {  
        return SUCCESS;  
    }  
    else  
    {  
        return CONNECT_SERVER_FAIL;  
    }  
}  
  
ProxyStatus CProxy::ConnectBySock4(SOCKET socket, string ip, u_short port)  
{  
    char buf[512];  
  
    memset(buf, 0, sizeof(buf));   
    struct TSock4req1 *proxyreq;  
    proxyreq = (struct TSock4req1*)buf;   
    proxyreq->VN = 4;   
    proxyreq->CD = 1;   
    proxyreq->Port = ntohs(port);   
    proxyreq->IPAddr = inet_addr(ip.c_str());   
  
    Send(socket, buf, 9);  
  
    struct TSock4ans1 *proxyans;   
    proxyans = (struct TSock4ans1*)buf;   
    memset(buf, 0, sizeof(buf));   
  
    Receive(socket, buf, sizeof(buf));  
    if(proxyans->VN == 0 && proxyans->CD == 90)   
    {   
        return SUCCESS;   
    }   
    else  
    {  
        return CONNECT_SERVER_FAIL;  
    }  
}  
  
ProxyStatus CProxy::ConnectBySock5(SOCKET socket, string ip, u_short port)  
{  
    char buf[512];  
  
    struct TSock5req1 *proxyreq1;   
    proxyreq1 = (struct TSock5req1 *)buf;   
    proxyreq1->Ver = 5;   
    proxyreq1->nMethods = 1;   
    proxyreq1->Methods = m_proxyUserName != "" ? 2 : 0;  
  
    Send(socket, buf, 3);   
      
    struct TSock5ans1 *proxyans1;   
    proxyans1 = (struct TSock5ans1 *)buf;   
  
    memset(buf, 0, sizeof(buf));  
    Receive(socket, buf, sizeof(buf));  
    if(proxyans1->Ver != 5 || (proxyans1->Method != 0 && proxyans1->Method != 2))   
    {   
        return CONNECT_SERVER_FAIL;   
    }  
  
    if(proxyans1->Method == 2)   
    {   
        int nUserLen = m_proxyUserName.length();  
        int nPassLen = m_proxyUserPwd.length();  
        //struct TAuthreq *authreq;   
        //authreq = (struct TAuthreq *)buf;   
        //authreq->Ver = 1;   
        //authreq->Ulen = nUserLen;   
        //strcpy(authreq->Name, m_proxyUserName.c_str());   
        //authreq->PLen = nPassLen;   
        //strcpy(authreq->Pass, m_proxyUserPwd.c_str());   
  
        buf[0] = 1;  
        buf[1] = nUserLen;  
        memcpy(buf + 2, m_proxyUserName.c_str(), nUserLen);  
        buf[2 + nUserLen] = nPassLen;  
        memcpy(buf + 3 + nUserLen, m_proxyUserPwd.c_str(), nPassLen);  
  
        Send(socket, buf, 3 + nUserLen + nPassLen);  
  
        struct TAuthans *authans;   
        authans = (struct TAuthans *)buf;   
        memset(buf, 0, sizeof(buf));   
  
        Receive(socket, buf, sizeof(buf));  
        if(authans->Ver != 1 || authans->Status != 0)   
        {   
            return CONNECT_SERVER_FAIL;  
        }   
    }  
  
    memset(buf, 0, sizeof(buf));  
    struct TSock5req2 *proxyreq2;   
    proxyreq2 = (struct TSock5req2 *)buf;   
    proxyreq2->Ver = 5;   
    proxyreq2->Cmd = 1;   
    proxyreq2->Rsv = 0;   
    proxyreq2->Atyp = 1;   
    unsigned long tmpLong = inet_addr(ip.c_str());   
    unsigned short port1 = ntohs(port);   
    memcpy((char*)&proxyreq2->other, &tmpLong, 4);   
    memcpy((char*)(&proxyreq2->other) + 4, &port1, 2);   
  
    //Send(socket, buf, sizeof(struct TSock5req2) + 5);   
    Send(socket, buf, 10);   
    struct TSock5ans2 *proxyans2;   
    memset(buf ,0, sizeof(buf));   
    proxyans2 = (struct TSock5ans2 *)buf;   
      
    Receive(socket, buf, sizeof(buf));  
    if(proxyans2->Ver != 5 || proxyans2->Rep != 0)   
    {   
        return CONNECT_SERVER_FAIL;   
    }  
  
    return SUCCESS;  
}  
  
int CProxy::Receive(SOCKET socket, char* buf, int bufLen)  
{  
    return recv(socket, buf, bufLen, 0);  
}  
  
bool CProxy::Send(SOCKET socket, const char* buf, int len)  
{  
    long ilen = len;  
    int sendCnt = 0;  
    int ret;  
  
    while(sendCnt < ilen)  
    {  
        if((ret = send(socket, buf + sendCnt, ilen - sendCnt, 0)) == SOCKET_ERROR)  
        {  
            return false;  
        }  
        else  
        {  
            sendCnt += ret;  
        }  
    }  
  
    return true;  
}  


bool CProxy::HasProxyServer()
{
    return !m_proxyIp.empty();
}