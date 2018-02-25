#pragma once

#ifndef __linux__
#else
#define SOCKET int
#define SOCKET_ERROR -1
#endif

#define SOCKET_NO_PAIR  -99

enum Status  
{  
    SUCCESS,  
    CONNECT_PROXY_FAIL,  
    NOT_CONNECT_PROXY,  
    CONNECT_SERVER_FAIL, 
    CREATE_LISTENER_FAIL
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