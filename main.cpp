


#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

using namespace std;
using namespace cv;

unsigned short *image2d;
Mat imagecv;

int main(int argc, char *argv[])
{
	char* originalFile=argv[1];
	short unsigned int width = atoi(argv[2]);
	short unsigned int height = atoi(argv[3]);
	short unsigned int slices = atoi(argv[4]);

	FILE* inFile;
	
	
	// allocate memory for the 2d image	
	image2d = (unsigned short*)malloc(sizeof(unsigned short*) * (height*width));
//	for (int i=0; i < height; i++)
//		image2d[i] = (short*)malloc(sizeof(short) * width);

	if( inFile = fopen( originalFile, "rb" ) )
	{
		// read file into image matrix
		int rrr=height*width;
		for( int i = 0; i < rrr; i++ )
		{
				unsigned short value;
				fread( &value, 1, sizeof(unsigned short), inFile );
				image2d[i] = value;
		}
		fclose(inFile);
	}
	else
	{
		cout << "FAIL"<<endl;
	}

	Mat plane(height,width, CV_16UC1,image2d);
	

    
    // Window
    const char* WIN_RF = "Reference";
	namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE );

    cvMoveWindow(WIN_RF, 10, 0);
	imshow(WIN_RF,plane);

	cout << endl;

	  waitKey(0);
    return 0;
}

