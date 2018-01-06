/*************************************************************************
	> File Name: readdir.cpp
	> Author: Xuxiansong
	> Mail: 2808595125@163.com 
	> Created Time: 2017年06月22日 星期四 16时21分35秒
 ************************************************************************/

#include <iostream>
#include "readdir.h"

using namespace std;

void traitsDir(std::string dir_path,std::vector<string>& vfull, isYES func)
{
    if(dir_path[dir_path.length()-1]!='/')dir_path+="/";

    DIR* dp;
    struct dirent *dir;

    if((dp=opendir(dir_path.c_str())) == NULL) {
        cerr<< "open dirent failed!\n";
        return;
    }

    while((dir=readdir(dp)) !=NULL){
        string name=dir->d_name;
        if(func(name))
          vfull.push_back(dir_path+name);
    }
}


//#include <sys/stat.h> 　
//#include <sys/types.h> 　　
//函数原型： 　　int mkdir(const char *pathname, mode_t mode); 　　
//函数说明： 　　mkdir()函数以mode方式创建一个以参数pathname命名的目录，mode定义新创建目录的权限。 　　
//返回值： 　　若目录创建成功，则返回0；否则返回-1，并将错误记录到全局变量errno中。 　　
//mode方式： 　　
//S_IRWXU 00700权限，代表该文件所有者拥有读，写和执行操作的权限 
//S_IRUSR(S_IREAD) 00400权限，代表该文件所有者拥有可读的权限 
//S_IWUSR(S_IWRITE) 00200权限，代表该文件所有者拥有可写的权限 
//S_IXUSR(S_IEXEC) 00100权限，代表该文件所有者拥有执行的权限 
//S_IRWXG 00070权限，代表该文件用户组拥有读，写和执行操作的权限 
//S_IRGRP 00040权限，代表该文件用户组拥有可读的权限 
//S_IWGRP 00020权限，代表该文件用户组拥有可写的权限 
//S_IXGRP 00010权限，代表该文件用户组拥有执行的权限 
//S_IRWXO 00007权限，代表其他用户拥有读，写和执行操作的权限 
//S_IROTH 00004权限，代表其他用户拥有可读的权限 
//S_IWOTH 00002权限，代表其他用户拥有可写的权限 
//S_IXOTH 00001权限，代表其他用户拥有执行的权限
int mkdir(const std::string& dir_path)
{
    return mkdir(dir_path.c_str(), S_IRWXU);
}












