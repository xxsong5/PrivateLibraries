#include "client.h"
#include "log/log.h"

using namespace std;


ClientStatus Client::Connect(string ServerIp, u_short ServerPort)
{
    //创建一个套接字  
    LOGS << "2.0" << LOGE;
    m_socket = socket(AF_INET,SOCK_STREAM,0);  

    if (m_localProxy.HasProxyServer()){

        m_localProxy.ConnectProxyServer(m_socket);
        return m_localProxy.ConnectServer(m_socket, ServerIp, ServerPort);

    } else {

        ///定义sockaddr_in
        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(ServerPort);
        servaddr.sin_addr.s_addr = inet_addr(ServerIp.c_str());

    LOGS << "2.1" << LOGE;
        //连接服务器，成功返回0，错误返回-1
        if (connect(m_socket, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    LOGS << "2.2" << LOGE;
            close(m_socket);  
            return ClientStatus::CONNECT_SERVER_FAIL;
        }
    }
    LOGS << "2.3" << LOGE;

    return ClientStatus::SUCCESS;
}



int Client::Snd(const char* snd, size_t len)
{
    return Socket::Snd(m_socket,snd, len);
}

int Client::Rcv(char **rcv)
{
    return Socket::Rcv(m_socket, rcv);
}


int Client::Snd(const std::string  &strSnd)
{
    return Snd(strSnd.c_str(), strSnd.length());
}


int Client::Rcv(std::string  &strRcv)
{
    char *str = NULL;
    int   strLen = Rcv(&str);
    strRcv = std::string(str, strLen);
    free(str);
    return strLen;
}