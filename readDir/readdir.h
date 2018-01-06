/*************************************************************************
	> File Name: readdir.h
	> Author: Xuxiansong
	> Mail: 2808595125@163.com 
	> Created Time: 2017年06月22日 星期四 16时20分50秒
 ************************************************************************/

#include <iostream>
#include <dirent.h>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

typedef bool(*isYES)(const std::string& );

void traitsDir(std::string dir_path, std::vector<std::string>& vfull, isYES func= [](const std::string&)->bool{return 1;});

int mkdir(const std::string& dir_path);




