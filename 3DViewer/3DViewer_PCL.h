#include<iostream>
#include<pcl/common/common_headers.h>
#include<pcl/features/normal_3d.h>
#include<pcl/io/pcd_io.h>
#include<pcl/visualization/pcl_visualizer.h>
#include<pcl/console/parse.h>
#include<pcl/filters/statistical_outlier_removal.h>
#include<thread>
#include<mutex>
#include<chrono>
#include<string>
#include<vector>
#include<opencv/cv.h>
#include<opencv/highgui.h>

#pragma once
#ifndef XUXIANSONG_3DVIEWER
#define XUXIANSONG_3DVIEWER


class Viewer_PCL
{
 public:

	Viewer_PCL(unsigned int PointSize=0, bool SetCoordinate=0):mworldCloud(new pcl::PointCloud<pcl::PointXYZRGB>),mtmpCloud(new pcl::PointCloud<pcl::PointXYZRGB>),mpointSize(PointSize),mbupdate(0),mbsetCoordinate(SetCoordinate), mIsReady(0),mIsBusy(0),mStop(0)
	{
		//mworldCloud(new pcl::PointCloud<pcl::PointXYZRGB>);
		mfxi=-1.0f;
		mfyi=-1.0f;
		mcx=-1.0f;
		mcy=-1.0f;

        mcamera[0]= 0;
        mcamera[1]=-1;
        mcamera[2]= 0;
        mcamera[3]= 1;
        mcamera[4]=-1;
        mcamera[5]= 1;

		pthread=new std::thread(&Viewer_PCL::run,this);	
        pthread1=new std::thread(&Viewer_PCL::work,this);
	}

	~Viewer_PCL()
	{
        mStop=1;
		pthread->join();
		pthread1->join();
	}

	typedef struct PointXYZRGB
	{
		float X;
		float Y;
		float Z;
		int	  rgb; //()(r)(g)(b)
		
	}PointXYZRGB;


	void finalFilter();
	void setPointSize(unsigned int size);

	//this publishpc need set cameraIntrisic and imDepth is CV_32FC1 and imColor is CV_8UC... camera=Tcw*world
	void setCameraIntrinsic(float fx, float fy, float cx, float cy);
	void PublishPC(const cv::Mat& Tcw, const cv::Mat& imDepth, const cv::Mat& imColor, bool isfilter=0);
	void PublishPCBackGround(const cv::Mat& Tcw, const cv::Mat& imDepth, const cv::Mat& imColor);

    void setCurrentViewPort(float x_pos, float y_pos, float z_pos, float x_orig, float y_orig, float z_orig);

	//PointXYZRGB is a type defined in this field
	void PublishPC(const std::vector<Viewer_PCL::PointXYZRGB>& vec, bool isfilter=0);

	void PublishPC(pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr pointcloud);

    void PublishPC_onlyOne(const cv::Mat & Q, const cv::Mat & disparity, const cv::Mat & imgray);
    
    void PublishPC_onlyOne(const cv::Mat & color, const cv::Mat& depth);

    void PublishPC(char* pcdFiles, bool isfilter=0);

    


    void savePointCloud();

private:


	//run in other thread
	void run();

    // need to set  member variable: (mIsReady & mCColor & mCDepth & mTcw) to compute 3Ds at background.
    void work();

	// tansport mtmpCloud data to mworldCloud in run();
	void preProcess();

	//process data in mtmpCloud
	void filter();
	
  


	//pointcloud published in world 
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr mworldCloud;
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr mtmpCloud;
	unsigned int	mpointSize;
	bool			mbupdate;
	std::mutex		mupdateMutex; 

	std::thread*    pthread;
	std::thread*    pthread1;


	//camera inverse intrinsic 
	float mfxi,mfyi,mcx,mcy;

    bool  mbsetCoordinate;

    //need to set if want to compute 3Ds at background
    volatile bool mIsReady;
    volatile bool mIsBusy;
    volatile bool mStop;
    std::mutex mCmutex;
    cv::Mat mCColor;
    cv::Mat mCDepth;
    cv::Mat mTcw;

    std::mutex mcameraMutex;
    float mcamera[6];//camera posion and viewer vector

};


#endif

