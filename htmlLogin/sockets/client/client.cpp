#include "client.h"

using namespace std;


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