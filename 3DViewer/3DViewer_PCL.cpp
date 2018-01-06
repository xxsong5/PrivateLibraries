#include"3DViewer_PCL.h"



void Viewer_PCL::run()
{

	std::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
	if(viewer)
	{
		viewer->setBackgroundColor(255,255,255);
		viewer->initCameraParameters();
		if(mbsetCoordinate)viewer->addCoordinateSystem(1.0);
		while(!viewer->wasStopped())
		{
			viewer->spinOnce(100);
			{
				std::lock_guard<std::mutex> lck(mupdateMutex);
				std::lock_guard<std::mutex> lck0(mcameraMutex);
                viewer->setCameraPosition(mcamera[0],mcamera[1],mcamera[2],mcamera[3],mcamera[4],mcamera[5]);
				if(mbupdate)
				{
					preProcess();
					if(!viewer->updatePointCloud(mworldCloud,"sample"))
					{
					  pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> rgb(mworldCloud);
					  viewer->addPointCloud(mworldCloud,rgb,"sample");
					 // pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> rgb(mtmpCloud);
					 // viewer->addPointCloud(mtmpCloud,rgb,"sample");
					}
					viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE,mpointSize,"sample");
					mbupdate=false;
		    	}
			}
		//	std::this_thread::sleep_for(std::chrono::microseconds(100000));
		}
	}
	else
	  std::cout<<"pclvisualizer is NULL"<<std::endl;
}


void Viewer_PCL::setCurrentViewPort(float x_pos, float y_pos, float z_pos, float x_orig, float y_orig, float z_orig)
{
    std::lock_guard<std::mutex> lck(mcameraMutex);
    mcamera[0]=x_pos;
    mcamera[1]=y_pos;
    mcamera[2]=z_pos;
    mcamera[3]=x_orig;
    mcamera[4]=y_orig;
    mcamera[5]=z_orig;
}


void Viewer_PCL::preProcess()
{
	for(auto it=mtmpCloud->begin();it<mtmpCloud->end();++it)
	{
		mworldCloud->push_back(*it);
	}

}

void Viewer_PCL::filter()
{

}

void Viewer_PCL::finalFilter()
{

	pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGB>);

	std::lock_guard<std::mutex> lck(mupdateMutex);

	pcl::StatisticalOutlierRemoval<pcl::PointXYZRGB> sor;
	sor.setInputCloud(mworldCloud);
	sor.setMeanK(50);
	sor.setStddevMulThresh(1.0);
	sor.filter(*cloud);
	mworldCloud->clear();
	for(auto it=cloud->begin();it<cloud->end();++it)
	  mworldCloud->push_back(*it);
	mbupdate=true;

}


void Viewer_PCL::setPointSize(unsigned int size)
{
	std::lock_guard<std::mutex> lck(mupdateMutex);
	mpointSize=size;
	mbupdate=true;

}

void Viewer_PCL::setCameraIntrinsic(float fx, float fy, float cx, float cy)
{
	mfxi=1.0f/fx;
	mfyi=1.0f/fy;
	mcx=cx;
	mcy=cy;
}

void Viewer_PCL::PublishPC(const cv::Mat& Tcw, const cv::Mat& imDepth, const cv::Mat&imColor, bool isfilter)
{
	if(!(imDepth.type()==CV_32FC1||imDepth.type()==CV_32F)||Tcw.empty()){
	  return ;
	}
	if(mfxi<0) 
	{
	  std::cout<<"forget set cameraIntrinsic..........."<<std::endl;
	  return;
	}


	cv::Mat  Depth=imDepth;
	cv::Mat  Color=imColor;//Color CV_8UC..
	mtmpCloud->clear();
	cv::Mat Rwc(3,3,CV_32F);
	cv::Mat T(3,1,CV_32F);
	cv::Mat Twc_=cv::Mat::eye(4,4,CV_32F);
	Rwc=Tcw.rowRange(0,3).colRange(0,3).t();
	T=-Rwc*Tcw.rowRange(0,3).col(3);
    Rwc.copyTo(Twc_.rowRange(0,3).colRange(0,3));
    T.copyTo(Twc_.rowRange(0,3).col(3));
    

	std::lock_guard<std::mutex>lck(mupdateMutex);

	for(int i=1;i<imDepth.rows;++i)
	  for(int j=1;j<imDepth.cols;++j)
	  {

		  float depth=Depth.at<float>(i,j);
		  if(depth<0.001)continue;
		  const float xx=(j-mcx)*depth*mfxi;
		  const float yy=(i-mcy)*depth*mfyi;
		  cv::Mat X3D= (cv::Mat_<float>(3,1)<<xx,yy,depth);

       #ifdef __SSE__
          __m128 ma= _mm_set_ps(Twc_.at<float>(0,0),Twc_.at<float>(0,1),Twc_.at<float>(0,2),Twc_.at<float>(0,3));
          __m128 mb= _mm_set_ps(Twc_.at<float>(1,0),Twc_.at<float>(1,1),Twc_.at<float>(1,2),Twc_.at<float>(1,3));
          __m128 mc= _mm_set_ps(Twc_.at<float>(2,0),Twc_.at<float>(2,1),Twc_.at<float>(2,2),Twc_.at<float>(2,3));
          __m128 md= _mm_set_ps(xx,yy, depth, 1.0f);

          ma= _mm_mul_ps(ma,md);
          mb= _mm_mul_ps(mb,md);
          mc= _mm_mul_ps(mc,md);

          ma= _mm_hadd_ps(ma,ma);
          ma= _mm_hadd_ps(ma,ma);
          mb= _mm_hadd_ps(mb,mb);
          mb= _mm_hadd_ps(mb,mb);
          mc= _mm_hadd_ps(mc,mc);
          mc= _mm_hadd_ps(mc,mc);

          float x__,y__,z__;
          _mm_store_ss(&x__,ma);
          _mm_store_ss(&y__,mb);
          _mm_store_ss(&z__,mc);

          X3D.at<float>(0)=x__;
          X3D.at<float>(1)=y__;
          X3D.at<float>(2)=z__;

       #else

		  X3D=Rwc*X3D+T;

       #endif

		  pcl::PointXYZRGB point;
		  point.x=X3D.at<float>(0);
		  point.y=X3D.at<float>(1);
		  point.z=X3D.at<float>(2);

		  uint8_t r,g,b;
		  unsigned char* p;
		  p=Color.data+Color.step[0]*i+Color.step[1]*j;
		  if(Color.channels()==3)
		  {
		     r=*p;
		     g=*(p+1);
		     b=*(p+2);
		  }
		  else
		  {
			  r=g=b=*p;
		  }
		  uint32_t rgb=(static_cast<uint32_t>(r)<<16|
					  static_cast<uint32_t>(g)<<8| static_cast<uint32_t>(b));
		  point.rgb=*reinterpret_cast<float*>(&rgb);
		  mtmpCloud->points.push_back(point);

	  }
	mtmpCloud->width=(int)mtmpCloud->points.size();
	mtmpCloud->height=1;

	if(isfilter)
	  filter();

//	std::lock_guard<std::mutex>lck(mupdateMutex);
	mbupdate=true;

}



void Viewer_PCL::PublishPC(const std::vector<Viewer_PCL::PointXYZRGB> &vec,bool isfilter)
{
//	std::lock_guard<std::mutex>lck(mupdateMutex);
	mtmpCloud->clear();

}

void Viewer_PCL::PublishPC(char* pcdFiles, bool isfilter)
{
	pcl::io::loadPCDFile(pcdFiles, *mworldCloud);

    if(isfilter)
      finalFilter();

	std::lock_guard<std::mutex>lck(mupdateMutex);
	mbupdate=true;
}

void Viewer_PCL::PublishPC(pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr pointcloud)
{
	
	for(auto it=pointcloud->begin();it<pointcloud->end();++it)
	{
		mworldCloud->push_back(*it);
	}

	std::lock_guard<std::mutex>lck(mupdateMutex);
	mbupdate=true;

}


void Viewer_PCL::PublishPC_onlyOne(const cv::Mat & imColor, const cv::Mat& imDepth)
{
	cv::Mat  Depth=imDepth.clone();
	cv::Mat  Color=imColor.clone();//Color CV_8UC..

	if(!(imDepth.type()==CV_32FC1||imDepth.type()==CV_32F)){
      if(imDepth.type()==CV_16SC1 || imDepth.type()==CV_16S){
          Depth.convertTo(Depth, CV_32FC1, 0.001);
      }
      else{
        std::cout<<" depth image type is wrong ..."<<std::endl;
        return;
      }
	}
	if(mfxi<0) 
	{
	  std::cout<<"forget set cameraIntrinsic..........."<<std::endl;
	  return;
	}


	mtmpCloud->clear();

	std::lock_guard<std::mutex>lck(mupdateMutex);

	for(int i=1;i<imDepth.rows;++i)
	  for(int j=1;j<imDepth.cols;++j)
	  {

		  uint8_t r,g,b;
		  unsigned char* p;
		  float depth=Depth.at<float>(i,j);
		  if(depth<0.001)continue;
		  const float xx=(j-mcx)*depth*mfxi;
		  const float yy=(i-mcy)*depth*mfyi;

		  cv::Mat X3D= (cv::Mat_<float>(3,1)<<xx,yy,depth);

		  p=Color.data+Color.step[0]*i+Color.step[1]*j;
		  if(Color.channels()==3)
		  {
		     r=*p;
		     g=*(p+1);
		     b=*(p+2);
		  }
		  else
		  {
			  r=g=b=*p;
		  }

		  pcl::PointXYZRGB point;
		  point.x=X3D.at<float>(0);
		  point.y=X3D.at<float>(1);
		  point.z=X3D.at<float>(2);

		  uint32_t rgb=(static_cast<uint32_t>(r)<<16|
					  static_cast<uint32_t>(g)<<8| static_cast<uint32_t>(b));
		  point.rgb=*reinterpret_cast<float*>(&rgb);
		  mtmpCloud->points.push_back(point);

	  }
	mtmpCloud->width=(int)mtmpCloud->points.size();
	mtmpCloud->height=1;

    mworldCloud->clear();
	mbupdate=true;

}


void Viewer_PCL::PublishPC_onlyOne(const cv::Mat & Q_float, const cv::Mat & disparity, const cv::Mat & imgray_color){

	mtmpCloud->clear();
    float BaseLine_x=20; //cm
    float BaseLine_ix=1/BaseLine_x;
    float camera_fx=524;
    float camera_fy=524;
    float camera_cx=320;
    float camera_cx_=320;
    float camera_cy=240;

    if(!Q_float.empty()){

    }


    float C_= (camera_cx-camera_cx_)*BaseLine_x;

    for(int i=0; i< disparity.rows; ++i){
        
        float B_ = i-camera_cy;

        for(int j=0; j< disparity.cols; ++j){
            const unsigned char dis= disparity.at<unsigned char>(i, j);
            if(dis <=0)continue;
            float A_ = j-camera_cx;
            float D = C_ + BaseLine_ix*dis;

            float X= A_/D;
            float Y= B_/D;
            float Z= camera_fx/D;


		    uint8_t r,g,b;
		    const unsigned char* p=imgray_color.data+imgray_color.step[0]*i+imgray_color.step[1]*j;
		    if(imgray_color.channels()==3)
		    {
		       r=*p;
		       g=*(p+1);
		       b=*(p+2);
		    }
		    else
		    {
		        r=g=b=*p;
		    }

		    pcl::PointXYZRGB point;
		    point.x=X;
		    point.y=Y;
		    point.z=Z;
            
		    uint32_t rgb=(static_cast<uint32_t>(r)<<16|
		      		  static_cast<uint32_t>(g)<<8| static_cast<uint32_t>(b));
		    point.rgb=*reinterpret_cast<float*>(&rgb);
		    mtmpCloud->points.push_back(point);

        }

    }

	std::lock_guard<std::mutex>lck(mupdateMutex);
    mworldCloud->clear();
	mbupdate=true;
}


void Viewer_PCL::savePointCloud()
{
     pcl::io::savePCDFileASCII ("savedPointCloud.pcd", *mworldCloud);
}


void Viewer_PCL::PublishPCBackGround(const cv::Mat& Tcw, const cv::Mat& imDepth, const cv::Mat& imColor)
{
    if(!mIsBusy){
        std::lock_guard<std::mutex> lck(mCmutex);
        Tcw.copyTo(mTcw);
        imDepth.copyTo(mCDepth);
        imColor.copyTo(mCColor);
        mIsReady=1;
    }
}

void Viewer_PCL::work()
{
    while(true){
        if(mStop)break;
        if(mIsReady){
            mIsBusy=1;
            mIsReady=0;
            std::lock_guard<std::mutex> lck(mCmutex);
            PublishPC(mTcw, mCDepth, mCColor);
            mIsBusy=0;
        }
        else{
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }
}





