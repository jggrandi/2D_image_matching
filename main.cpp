


#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

using namespace std;
using namespace cv;

double getPSNR ( const Mat& I1, const Mat& I2);
Scalar getMSSIM( const Mat& I1, const Mat& I2);


int main(int argc, char *argv[])
{

    Mat img1;
    Mat img2;
    Mat img3;

    img1 = imread( argv[1], -1 );
    img2 = imread( argv[2], -1 );
    img3 = imread( argv[3], -1 );

    const char* WIN_UT1 = "Under Test1";
    const char* WIN_UT2 = "Under Test2";
    const char* WIN_RF = "Reference";

    // Windows
    namedWindow(WIN_RF, CV_WINDOW_AUTOSIZE );
    namedWindow(WIN_UT1, CV_WINDOW_AUTOSIZE );
    namedWindow(WIN_UT2, CV_WINDOW_AUTOSIZE );
    cvMoveWindow(WIN_RF, 10       ,            0);		 //750,  2 (bernat =0)
    cvMoveWindow(WIN_UT1, 522,            0);		 //1500, 2
    cvMoveWindow(WIN_UT2, 1024,            0);		 //1500, 2

    double psnrV;
    Scalar mssimV;

        ///////////////////////////////// PSNR ////////////////////////////////////////////////////
	psnrV = getPSNR(img1,img2);					//get PSNR
	cout << setiosflags(ios::fixed) << setprecision(3) << psnrV << "dB";

	//////////////////////////////////// MSSIM /////////////////////////////////////////////////
	//if (psnrV < psnrTriggerValue && psnrV)
	//{
		mssimV = getMSSIM(img1,img2);

		cout << " MSSIM: "
			<< " R " << setiosflags(ios::fixed) << setprecision(2) << mssimV.val[2] * 100 << "%"
			<< " G " << setiosflags(ios::fixed) << setprecision(2) << mssimV.val[1] * 100 << "%"
			<< " B " << setiosflags(ios::fixed) << setprecision(2) << mssimV.val[0] * 100 << "%";
	//}
	int m1=mssimV.val[2];
	cout << endl;

	psnrV = getPSNR(img1,img3);					//get PSNR
	cout << setiosflags(ios::fixed) << setprecision(3) << psnrV << "dB";

	//////////////////////////////////// MSSIM /////////////////////////////////////////////////
	//if (psnrV < psnrTriggerValue && psnrV)
	//{
		mssimV = getMSSIM(img1,img3);

		cout << " MSSIM: "
			<< " R " << setiosflags(ios::fixed) << setprecision(2) << mssimV.val[2] * 100 << "%"
			<< " G " << setiosflags(ios::fixed) << setprecision(2) << mssimV.val[1] * 100 << "%"
			<< " B " << setiosflags(ios::fixed) << setprecision(2) << mssimV.val[0] * 100 << "%";
	//}

	cout << endl;
	int m2=mssimV.val[2];


	////////////////////////////////// Show Image /////////////////////////////////////////////
	imshow( WIN_RF, img1);
	imshow( WIN_UT1, img2);
	imshow( WIN_UT2, img3);

	//c = cvWaitKey(delay);
	//if (c == 27) break;
	if (m1>m2) cout << "IMG2 é mais parecida com imagem de referencia" <<endl;
	else cout << "IMG3 é mais parecida com imagem de referencia" <<endl;
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
