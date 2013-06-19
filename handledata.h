/* Class to handle datasets
	by: Jeronimo G. Grandi
	Jun,2013
*/  

#ifndef HANDLE_DATA
#define HANDLE_DATA

#include <fstream>
#include <cstdlib>

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/gpu/gpu.hpp> 

	
#include "imageinfo.h"
#include "loaddata.h"
#include "qualityassessment.h"

#define QNT_DATASETS 2

class HandleData
{
public:
	HandleData();
	HandleData(ImageInfo i_img1, ImageInfo i_img2);
	~HandleData();
	void similarityCheck();
private:
	ImageInfo *img1,*img2;
	LoadData dataset1,dataset2;
	QualityAssessment q;

};


#endif // HANDLE_DATA