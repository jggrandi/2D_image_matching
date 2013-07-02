/* Class to handle datasets
	by: Jeronimo G. Grandi
	Jun,2013
*/  

#ifndef HANDLE_DATA
#define HANDLE_DATA

#include <fstream>
#include <cstdlib>
#include <cstdio>


#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/gpu/gpu.hpp> 

#include <handle3ddataset.h>
#include "qualityassessment.h"
#include "utils.h"
#include "logdata.h"

using namespace cv;


class HandleData
{
public:
	HandleData();
	HandleData(DATAINFO h_img1, DATAINFO h_img2, OPT h_options);
	~HandleData();
	void similarityCheck();
	void showData();
	//friend void onTrackbar( int, void* );
private:
	DATAINFO *img1,*img2;
	Handle3DDataset dataset1,dataset2;
	QualityAssessment q;
	OPT oopt;
	LogData logData;
	vector<twoInts> similarityResults;
};


#endif // HANDLE_DATA