


#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

using namespace std;
using namespace cv;

unsigned short **datasetRaw;
vector<Mat> datasetSlices;


double getPSNR ( const Mat& I1, const Mat& I2);
Scalar getMSSIM( const Mat& I1, const Mat& I2);


void onTrackbar( int val, void* )
{
	imshow( "Slice Selection", datasetSlices[val]);
}


int main(int argc, char *argv[])
{
	char* originalFile=argv[1];
	short unsigned int width = atoi(argv[2]);
	short unsigned int height = atoi(argv[3]);
	short unsigned int slices = atoi(argv[4]);

	FILE* inFile;

	// allocate memory for the 3d dataset
	datasetRaw = (unsigned short**)malloc(slices * sizeof(unsigned short*));
	for (int i=0; i < slices; i++)
		datasetRaw[i] = (unsigned short*)malloc(sizeof(unsigned short) * (height*width));

	if( inFile = fopen( originalFile, "rb" ) )
	{
		// read file into dataset matrix
		int rrr=height*width;
		for( int i = 0; i < slices; i++ )
		{
			for( int j = 0; j < rrr; j++ )
			{
					unsigned short value;
					fread( &value, 1, sizeof(unsigned short), inFile );
					datasetRaw[i][j] = value;
			}
		}
		fclose(inFile);
	}
	else
	{
		cout << "FAIL"<<endl;
	}

    // Window
    //vector<const char*> WIN_RF;
	//namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE );
    //cvMoveWindow(WIN_RF, 10, 0);
	const int sl =slices;
	//Mat cube(slices, height*width, CV_16UC1,datasetRaw);
	//vector<Mat> datasetSlices;
	int d     = CV_8UC3;
	

	for( int i = 0; i < slices; i++ )
	{
		Mat slice(height,width,CV_16UC1,datasetRaw[i]);
		Mat plane;
		slice.convertTo(plane,d);
		datasetSlices.push_back(plane);
		//imshow( ss, datasetSlices[i]);
	}


//	imshow("110",datasetSlices[10]);
//	imshow("100",datasetSlices[100]);

/*	for (int i=0;i<slices;i++)
	{
		stringstream output;
		output << i;
		string sulfix = output.str();
		const char * ss = sulfix.c_str();
		imshow(ss, datasetSlices[i]);
	}
*/

	namedWindow("Trackbar",0);
	createTrackbar("TB","Trackbar",0,slices-1,onTrackbar);
	onTrackbar(0,0);

	double psnrV;
	Scalar mssimV;

        ///////////////////////////////// PSNR ////////////////////////////////////////////////////
//	psnrV = getPSNR(plane,plane2);					//get PSNR
//	cout << setiosflags(ios::fixed) << setprecision(3) << psnrV << "dB";

	//////////////////////////////////// MSSIM /////////////////////////////////////////////////
	//if (psnrV < psnrTriggerValue && psnrV)
	//{
	float out[2]={0,0};
	for(int i=0; i < datasetSlices.size(); i++)
	{
		mssimV = getMSSIM(datasetSlices[168],datasetSlices[i]);

//		cout << " MSSIM: "
//			<< " R " << setiosflags(ios::fixed) << setprecision(3) << mssimV.val[2] * 100 << "%"
//			<< " G " << setiosflags(ios::fixed) << setprecision(3) << mssimV.val[1] * 100 << "%"
//			<< " B " << setiosflags(ios::fixed) << setprecision(3) << mssimV.val[0] * 100 << "%";
		if(mssimV.val[0]>out[0])
		{
			out[0] = mssimV.val[0];
			out[1] = i;
		}
		
	}

	cout << "Slice mais parecido:"<<out[1]<<endl;

	////////////////////////////////// Show Image /////////////////////////////////////////////
//	imshow( WIN_RF, cube);
//	imshow( WIN_UT1, p2);

	waitKey(0);
	destroyAllWindows();
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
