/*************************************************************************
	> File Name: scoped_time.cpp
	> Author: Xuxiansong
	> Mail: 2808595125@163.com 
	> Created Time: Wed 22 Feb 2017 03:34:02 PM CST
 ************************************************************************/

#include <iostream>
#include <sys/time.h>
#include <string>


class Scoped_time 
{
public:
	
	Scoped_time(const std::string & str=std::string("scoped_time: ")):mstr(str){gettimeofday(&mstart,NULL);}
	Scoped_time(const Scoped_time& )=delete;
	~Scoped_time(){
		gettimeofday(&mend,NULL);
		std::cout<< mstr<<"\t"<< ((mend.tv_sec-mstart.tv_sec)*1000000L + mend.tv_usec - mstart.tv_usec)/1000.0 <<" ms" <<std::endl;
	}

	double getCurrentTime_sec(){
		struct timeval tv;
		gettimeofday(&tv,NULL);
		long ms= tv.tv_sec*1000 + tv.tv_usec/1000;
		return ms*0.001;
	}

	double getCurrentTime_ms(){
		struct timeval tv;
		gettimeofday(&tv,NULL);
		long ms= tv.tv_sec*1000 + tv.tv_usec/1000;
		return ms;
	}

private:
	std::string mstr;
	struct timeval mstart, mend;

};
