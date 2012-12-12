


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
unsigned short *image2d2;
Mat imagecv;

double getPSNR ( const Mat& I1, const Mat& I2);
Scalar getMSSIM( const Mat& I1, const Mat& I2);


int main(int argc, char *argv[])
{
	char* originalFile=argv[1];
	char* originalFile2=argv[2];
	short unsigned int width = atoi(argv[3]);
	short unsigned int height = atoi(argv[4]);
	short unsigned int slices = atoi(argv[5]);

	FILE* inFile;
	
	
	// allocate memory for the 2d image	
	image2d = (unsigned short*)malloc(sizeof(unsigned short*) * (height*width));
	image2d2 = (unsigned short*)malloc(sizeof(unsigned short*) * (height*width));
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

	if( inFile = fopen( originalFile2, "rb" ) )
	{
		// read file into image matrix
		int rrr=height*width;
		for( int i = 0; i < rrr; i++ )
		{
				unsigned short value;
				fread( &value, 1, sizeof(unsigned short), inFile );
				image2d2[i] = value;
		}
		fclose(inFile);
	}
	else
	{
		cout << "FAIL"<<endl;
	}


	Mat plane(height,width, CV_16UC1,image2d);
	Mat plane2(height,width, CV_16UC1,image2d2);
	
<<<<<<< HEAD
	int d     = CV_8UC3;

    Mat p1, p2;

    plane.convertTo(p1, d);
	plane2.convertTo(p2, d); 
	//cout << plane <<endl;
	//cout << p1 <<endl;
=======

    
>>>>>>> c9cc126a396511b1d5d06320acf44b13db3e7d15
    // Window
    const char* WIN_RF = "Reference";
	const char* WIN_UT1 = "Under Test1";

	namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE );
	namedWindow(WIN_UT1, CV_WINDOW_AUTOSIZE );
    cvMoveWindow(WIN_RF, 10, 0);
<<<<<<< HEAD
	cvMoveWindow(WIN_UT1, 522,0);
=======
	cvMoveWindow(WIN_UT1, 522,            0);		 //1500, 2
>>>>>>> c9cc126a396511b1d5d06320acf44b13db3e7d15

	double psnrV;
	Scalar mssimV;

        ///////////////////////////////// PSNR ////////////////////////////////////////////////////
	psnrV = getPSNR(plane,plane2);					//get PSNR
	cout << setiosflags(ios::fixed) << setprecision(3) << psnrV << "dB";

	//////////////////////////////////// MSSIM /////////////////////////////////////////////////
	//if (psnrV < psnrTriggerValue && psnrV)
	//{
		mssimV = getMSSIM(plane,plane2);

		cout << " MSSIM: "
			<< " R " << setiosflags(ios::fixed) << setprecision(2) << mssimV.val[2] * 100 << "%"
			<< " G " << setiosflags(ios::fixed) << setprecision(2) << mssimV.val[1] * 100 << "%"
			<< " B " << setiosflags(ios::fixed) << setprecision(2) << mssimV.val[0] * 100 << "%";
	//}



	////////////////////////////////// Show Image /////////////////////////////////////////////
<<<<<<< HEAD
	imshow( WIN_RF, p1);
	imshow( WIN_UT1, p2);
=======
	imshow( WIN_RF, plane);
	imshow( WIN_UT1, plane2);


	imshow(WIN_RF,plane);
	imshow(WIN_UT1,plane2);
>>>>>>> c9cc126a396511b1d5d06320acf44b13db3e7d15

	cout << endl;

	  waitKey(0);
    return 0;
}

double getPSNR(const Mat& I1, const Mat& I2)
{
    Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2

    Scalar s = sum(s1);         // sum elements per channel

    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

    if( sse <= 1e-10) // for small values return zero
        return 0;
    else
    {
        double  mse =sse /(double)(I1.channels() * I1.total());
        double psnr = 10.0*log10((255*255)/mse);
        return psnr;
    }
}

Scalar getMSSIM( const Mat& i1, const Mat& i2)
{
    const double C1 = 6.5025, C2 = 58.5225;
    /***************************** INITS **********************************/
    int d     = CV_32F;

    Mat I1, I2;
    i1.convertTo(I1, d);           // cannot calculate on one byte large values
    i2.convertTo(I2, d);

    Mat I2_2   = I2.mul(I2);        // I2^2
    Mat I1_2   = I1.mul(I1);        // I1^2
    Mat I1_I2  = I1.mul(I2);        // I1 * I2

    /*************************** END INITS **********************************/

    Mat mu1, mu2;   // PRELIMINARY COMPUTING
    GaussianBlur(I1, mu1, Size(11, 11), 1.5);
    GaussianBlur(I2, mu2, Size(11, 11), 1.5);

    Mat mu1_2   =   mu1.mul(mu1);
    Mat mu2_2   =   mu2.mul(mu2);
    Mat mu1_mu2 =   mu1.mul(mu2);

    Mat sigma1_2, sigma2_2, sigma12;

    GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
    sigma1_2 -= mu1_2;

    GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
    sigma2_2 -= mu2_2;

    GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;

    ///////////////////////////////// FORMULA ////////////////////////////////
    Mat t1, t2, t3;

    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

    Mat ssim_map;
    divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

    Scalar mssim = mean( ssim_map ); // mssim = average of ssim map
    return mssim;
}
