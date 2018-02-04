#include "client.h"


ClientStatus Client::CreateLisener(u_short localPort, string localIp)
{
    SOCKET fd;
 
    //创建一个inet类型的socket
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
        return ClientStatus::CREATE_LISTENER_FAIL;
 
 
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
        return ClientStatus::CREATE_LISTENER_FAIL;
    }
    
 
    // 开始侦听该端口
    if (listen(fd, 32) != 0)
    {
        close(fd);
        return ClientStatus::CREATE_LISTENER_FAIL;
    }
 
    m_lisenSocket = fd;

    return ClientStatus::SUCCESS;
}


ClientStatus Client::Connect(string ServerIp, u_short ServerPort)
{

}


bool Client::Transmitting()
{
    //1. 创建epoll描述符, 可接受5000个连接
    epoll_fd = epoll_create(5000);
 
    //2. 创建、绑定和侦听socket; 并且将它放到epoll上面开始侦听数据到达和connect连接
    listen_fd = create_listener(port);
　　epoll_event ev;
    ev.data.fd = cfd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cfd, &ev);//检查cfd上面的数据到达
 
 
    //3.创建线程、并脱离创建者；在线程中检索epoll上面的触发的socket
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    pthread_create(&tid, &attr, epoll_function, NULL);
}




int Client::Snd(SOCKET sockfd, const char* snd, size_t len)
{
    int tmp;
    size_t total = len;
    const char *p = snd;
 
    while(1)
    {
        /* window: 一般是0
         * linux: 最好设置为MSG_NOSIGNAL；表示出错后不向系统发信号，否则程序会退出！
         */
    #ifndef __linux__
        tmp = send(sockfd, p, total, 0);
    #else
        tmp = send(sockfd, p, total, MSG_NOSIGNAL);
    #endif

        if(tmp < 0)
        {
            //当进程收到信号会中断正在进行的系统调用、区处理信号，处理完系统返回-1且errno==Eintr;
            //所以可continue继续执行
            if(errno == EINTR)
                continue;
 
 
            // Eagain表示写缓冲队列已满, usleep后继续发送
            if(errno == EAGAIN)
            {
                usleep(1000);
                continue;
            }
 
 
            return -1;
        }
 
 
        if((size_t)tmp == total)
            return len;
 
 
        total -= tmp;
        p += tmp;
    }
 
 
    return tmp;
}

int Client::Rcv(SOCKET sockfd, char **rcv)
{
    size_t totalRcved = 0;
    size_t totalLen = 2048;//2kb
    *rcv = (char*)malloc(totalLen);

    char   *crcv = *rcv;
    size_t  clen = totalLen;
    

    while(true)
    {
        //接收数据，默认接收sizeof(buf)大小，实际接收buflen大小
        int buflen = recv(sockfd, crcv, clen, 0);
        if(buflen < 0)
        {
            // 由于是非阻塞的模式,所以当errno为EAGAIN时,表示当前缓冲区已无数据可读;在这里表示该事件已做处理
            if(errno == EAGAIN)
                continue;
            else
                return -1;//表示有错误发生
        }
        else if(buflen == 0)
        {
            // 这里表示对端的socket已正常关闭.
            totalRcved += buflen;
            return totalRcved;
        }else  if(buflen == totalLen)
        {
            // 需要再次读取
            totalRcved += buflen;
            totalLen    = totalRcved + 1024;
            *rcv = (char*)realloc(rcv, totalLen);
            crcv = *rcv + totalRcved;
            clen = 1024;
        } else
        {
            totalRcved += buflen;
            break;
        }
    }

    return totalRcved;
}



void* Client::epoll_func(void *p)
{
    while ( 1)
    {
        // 等待epoll上面的事件触发，然后将他们的fd检索出来
        struct epoll_event events[EPOLL_EVENT_MAX];
        int evn_cnt = epoll_wait(epfd, events, EVENTSIZE , -1);
 
        // 循环处理检索出来的events队列中的每个fd
        for (int i=0; i<wait_cnt; i++)
        {
            if (events[i].data.fd == listen_fd)   // 有客户端connect过来
            {
                struct sockaddr_in caddr;
                socklen_t clen = sizeof(caddr);
 
                // 接收连接
                int connfd = accept(listen_, (sockaddr *)&caddr, &clen);
                if (connfd < 0)
                    continue;
 
                // 将新接收的连接的socket放到epoll上面
                struct epoll_event ev;
                ev.data.fd = connfd;
                ev.events = EPOLLIN|EPOLLET;
                epoll_ctl(epoll_, EPOLL_CTL_ADD, connfd, &ev);
            }
            else if (events[i].events & EPOLLIN)       // 有数据到达
            {
                //接收数据
                recv(event[i].data.fd, buffer, sizeof(buffer), 0);
            }
        }
    }
}