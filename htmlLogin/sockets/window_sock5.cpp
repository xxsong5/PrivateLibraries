#define UNICODE  
#define _UNICODE  
#include <stdio.h>    
#include <string>  
#include<iostream>  
#include <WINSOCK2.H>    
#include<Windows.h>  
#pragma comment(lib,"ws2_32.lib")  
using namespace std;  
  
  
  
//SOCKET   
//LH代理服务器程序下开通的用户账号的IP地址< / param>  
//LH代理服务器程序下开通的用户账号的端口  
//LH代理服务器程序下开通的用户账号的账号  
//LH代理服务器程序下开通的用户账号的密码  
//设置要代理的访问的目标类型 1=IP4  2=域名  此访问类型是告诉lh服务器要代理连接到的目标地址  
//根据上方输入内容  IP4地址 或  域名  
//上方配套的连接地址对应的端口  
int SetConnection(SOCKET &ClientA , char* LhS5Ip , unsigned short LhS5Port , string LhS5UserName , string LhS5UserPassword , int ConnType_ , char* TragetAddress , unsigned short TragetProt)  
    {  
  
  
    unsigned int vindex=0;  
  
    SOCKADDR_IN addrSrv;  
    addrSrv.sin_addr.S_un.S_addr=inet_addr(LhS5Ip);//服务器端的地址    
    addrSrv.sin_family=AF_INET;  
    addrSrv.sin_port=htons(LhS5Port);  
    connect(ClientA , (SOCKADDR*)&addrSrv , sizeof(SOCKADDR));  
  
    unsigned int SizeLeng=6 + LhS5UserName.length() + LhS5UserPassword.length();  
    if (ConnType_ == 2) {  
        SizeLeng=SizeLeng+ 1+ strlen(TragetAddress);  
        }else{  
        SizeLeng+=4;  
        }  
  
  
    char *SendBuf =new char[SizeLeng];  
    SendBuf [0]=85;  
  
    SendBuf [1]=LhS5UserName.length();  
  
    vindex=2;  
  
    if (LhS5UserName.length() != 0) {  
        LhS5UserName._Copy_s(&SendBuf [vindex] , LhS5UserName.length() , LhS5UserName.length() , 0);  
        vindex+=LhS5UserName.length();  
        };  
  
  
    SendBuf [vindex]=LhS5UserPassword.length();  
    vindex+=1;  
    if (LhS5UserPassword.length() != 0) {  
        LhS5UserPassword._Copy_s(&SendBuf [vindex] , LhS5UserPassword.length() , LhS5UserPassword.length() , 0);  
        vindex+=LhS5UserPassword.length();  
        };  
  
    sockaddr_in sin;  
    if (ConnType_ == 1) {  
        SendBuf [vindex]=1;  
        vindex+=1;  
        sin.sin_addr.S_un.S_addr=inet_addr(TragetAddress);  
        memcpy(&SendBuf [vindex] , &sin.sin_addr.S_un.S_addr , 4);  
        vindex+=4;  
        }  
    else {  
        SendBuf [vindex]=2;  
        vindex+=1;  
        SendBuf [vindex]=strlen(TragetAddress);  
        vindex+=1;  
        for (size_t i=0; i < strlen(TragetAddress); i++) {  
            SendBuf [vindex]=TragetAddress[i];  
            vindex+=1;  
            }  
        }  
  
    sin.sin_port=htons(TragetProt); // 端口号  
    memcpy(&SendBuf [vindex] , &sin.sin_port , 2);  
  
    //for (size_t i=0; i < SizeLeng; i++) {  
    //  printf("%d \n" , SendBuf [i]);  
    //  }  
  
    send(ClientA , SendBuf , SizeLeng , 0);  
  
  
    char recvBuf [2];  
    recv(ClientA , recvBuf , 2 , 0);  
    if (recvBuf [1] == 0) {  
        return 0;  
        }  
    else {  
            return -1;  
        };  
    };  
  
  
void main()  
    {  
  
    //蓝恒网络Socket5客户端连接演示  
    //Http://www.lanheng.net  
    //下面仅用户测试使用  实际情况用户需根据自行情况进行相应修改  
  
    setlocale(LC_ALL , "chs");  
    wcout.imbue(locale("chs"));  
    int Test;  
  
    WORD wVersionRequested;  
    WSADATA wsaData;  
    int err;  
  
    wVersionRequested=MAKEWORD(1 , 1);  
  
    err=WSAStartup(wVersionRequested , &wsaData);  
    if (err != 0) {  
        return;  
        }  
  
    if (LOBYTE(wsaData.wVersion) != 1 ||  
        HIBYTE(wsaData.wVersion) != 1) {  
        WSACleanup();  
        return;  
        }  
    SOCKET sockClient=socket(AF_INET , SOCK_STREAM , 0);  
  
    // 下面以来接代理访问土豆网为例可直接使用IP地址或域名方式进行代理  
    int reslt=SetConnection(sockClient , "192.168.1.6" , 60000 , "U602414" , "P257638" , 1 , "123.126.98.146" , 80);  
//int reslt=SetConnection(sockClient , "192.168.1.6" , 60000 , "U602414" , "P257638" , 2 , "www.tudou.com" , 80);  
    if (reslt!=0)  
    {  
    // -1 本地连接到代理服务器或发送数据到代理服务器失败 请检查本机连接是否正常  
    // 0=成功  
    // 1=身份失败  
    // 2=服务器连接用户要代理的目标地址失败  
    // 3=服务器解析用户发送的域名地址失败  
    // 255=失败  
    closesocket(sockClient);  
    WSACleanup();  
    std::cin >> Test;  
    return ;  
    }  
      
  
  
 string Head="GET /  HTTP/1.0\r\nHost: www.tudou.com\r\n\r\n";  
    char *dest =new char [Head.length()];  
    for (int i=0; i < Head.length(); i++) {  
    //printf("%d \n" , Head.substr(i , 1).c_str() [0]);  
        dest [i]=(int)Head.substr(i , 1).c_str() [0];;  
        }  
    send(sockClient , dest , Head.length(), 0);  
  
  
    Sleep(1000);  
    char vdata [5000];  
    int VCount=recv(sockClient , vdata , 5000 , 0);  
     //返回访问网址的信息  费用内容长度可使用 Content-Length中的数量进行读取'  
    for (int i=0; i < VCount; i++) {  
        printf("%c" , vdata[i]);  
        };  
  
  
    closesocket(sockClient);  
    WSACleanup();  
  
  
    std::cin >> Test;  
    };  