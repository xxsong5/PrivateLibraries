#include "socket.h"


int Socket::Snd(SOCKET sockfd, const char* snd, size_t len)
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

int Socket::Rcv(SOCKET sockfd, char **rcv)
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
            *rcv = (char*)realloc(*rcv, totalLen);
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



int Socket::Snd(SOCKET socketfd, const std::string &strSnd)
{
    return Snd(socketfd, strSnd.c_str(), strSnd.length());
}


int Socket::Rcv(SOCKET fd, std::string &strRcv)
{
    char *str    = NULL;
    int   strLen = Socket::Rcv(fd, &str);

    if (strLen <= 0){
        free(str);
        return strLen;
    }

    strRcv       = std::string(str, strLen);
    free(str);

    return strLen;
}