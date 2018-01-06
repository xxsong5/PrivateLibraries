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

    if(argc !=3 ) {
        cerr << " need color and assi-dispartiy image\n";
        exit(0);
    }

    Viewer_PCL viewer;
//
//    cv::Mat color = cv::imread(argv[1]);
//    cv::Mat disparity= cv::imread(argv[2], CV_LOAD_IMAGE_UNCHANGED);
//
//    cv::Mat Q;
//    viewer.PublishPC_onlyOne(Q,disparity,color);
//
    while(1);

    //system("pause");

    return 0;
}



