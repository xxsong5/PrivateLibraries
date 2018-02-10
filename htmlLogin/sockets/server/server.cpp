#include "server.h"
#include "log/log.h"
#include <sys/epoll.h>
#include <thread>




ServerStatus Server::CreateListener(u_short localPort, std::string localIp)
{
    SOCKET fd;
 
    //创建一个inet类型的socket
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
        return ServerStatus::CREATE_LISTENER_FAIL;
 
 
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
    if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) != 0)
    {
        close(fd);
        return ServerStatus::CREATE_LISTENER_FAIL;
    }
    
 
    // 开始侦听该端口
    if (listen(fd, 32) != 0)
    {
        close(fd);
        return ServerStatus::CREATE_LISTENER_FAIL;
    }
 
    m_listenSocket  =   fd;
    m_listenIP      =   localIp;
    m_listenPort    =   localPort;

    return ServerStatus::SUCCESS;
}



void Server::DoWork()
{
    //1. 创建epoll描述符, 可接受连接数
    int epoll_fd = epoll_create(m_maxConnec);
 
    //2. 创建、绑定和侦听socket; 并且将它放到epoll上面开始侦听数据到达和connect连接
    epoll_event ev;
    ev.data.fd = m_listenSocket;
    ev.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev) < 0){
        char Info[1000];
        sprintf(Info, "add listen fd failed!, which is belongs to %s:%d", m_listenIP.c_str(), m_listenPort);
        LOGERROR(Info);
    }
 
    while (1)
    {
        // 等待epoll上面的事件触发，然后将他们的fd检索出来
        struct epoll_event events[m_maxConnec];
    LOGS << "22.0" << LOGE;
        int evn_cnt = epoll_wait(epoll_fd, events, m_maxConnec, -1);
 
    LOGS << "22.1" << LOGE;
        // 循环处理检索出来的events队列中的每个fd
        for (int i=0; i<evn_cnt; i++)
        {
    LOGS << "22.2" << LOGE;
            if (events[i].data.fd == m_listenSocket)   // 有客户端connect过来
            {
                struct sockaddr_in caddr;
                socklen_t clen = sizeof(caddr);
 
    LOGS << "22.3" << LOGE;
                // 接收连接
                int connfd = accept(m_listenSocket, (sockaddr *)&caddr, &clen);
                if (connfd < 0)
                    continue;
 
    LOGS << "22.4" <<"conn " << connfd << LOGE;
                // 将新接收的连接的socket放到epoll上面
                struct epoll_event ev;
                ev.data.fd = connfd;
                ev.events = EPOLLIN|EPOLLET;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connfd, &ev);
    LOGS << "22.5" << LOGE;
            }
            else if (events[i].events & EPOLLIN)       // 有数据到达
            {
    LOGS << "22.6" << LOGE;
                //接收数据
                char *rcvDatas=NULL;
                int   rcvLen;  
                rcvLen = Rcv(events[i].data.fd,&rcvDatas);

    LOGS << "22.7" << LOGE;
                if (rcvLen == 0){
                    close(events[i].data.fd);
    LOGS << "22.8" << LOGE;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
    LOGS << "22.9" << LOGE;

                    LOGS << "SOCKET " << events[i].data.fd << "closed" << LOGE;
                    continue;
                }
                
                std::string str(rcvDatas,rcvLen);
                LOGS << str << LOGE;
                free(rcvDatas);

    LOGS << "22.10" << LOGE;
                std::string strSnd = "hello sockets";
                Snd(events[i].data.fd, strSnd.c_str(), strSnd.length());
    LOGS << "22.11" << LOGE;
            }
        }
    }
 
}

