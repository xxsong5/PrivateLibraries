/*************************************************************************
	> File Name: main.cpp
	> Author: Xuxiansong
	> Mail: toatom@163.com 
	> Created Time: Tue 11 Apr 2017 12:25:45 PM CST
 ************************************************************************/

#include <iostream>
#include "3DViewer_PCL.h"
#include <opencv2/opencv.hpp>


using namespace std;
int main( int argc, char** argv){

    if(argc !=2 ) {
        cerr << " need pcd files\n";
        exit(0);
    }

    Viewer_PCL viewer;


    viewer.PublishPC(argv[1],0);


    system("pause");

    return 0;
}



