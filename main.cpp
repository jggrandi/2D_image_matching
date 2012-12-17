#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <fstream>

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

//Mostra ranking com slices mais parecidos e registra tempo

using namespace std;
using namespace cv;

unsigned short **datasetRaw[2];
vector<Mat> datasetSlices[2];



double getPSNR ( const Mat& I1, const Mat& I2);
Scalar getMSSIM( const Mat& I1, const Mat& I2);

struct sliceRank
{
	int sliceNumber;
	float value;
};

void onTrackbar( int val, void* )
{
	imshow( "Dataset1", datasetSlices[0][val]);
	imshow( "Dataset2", datasetSlices[1][val]);
}


void rankBuilder(int slices)
{
	sliceRank sr[200];
	double time;
	Scalar mssimV;
	time = (double)getTickCount();
	//for(int k=0; k < slices; k++)
	//{
		for(int i=0; i < slices; i++)
		{
			mssimV = getMSSIM(datasetSlices[0][100],datasetSlices[1][i]);
			sr[i].value = mssimV.val[0];
			sr[i].sliceNumber=i;
		}

/*		for(int j=0; j<slices; j++) //buble bunda pra fazer o ranking dos 10 slices mais parecidos.
		{
			for(int i=0; i<slices-1; i++)
			{
				if(sr[i+1].value > sr[i].value)
				  swap(sr[i+1], sr[i]);
			}
		}
		cout <<"Slice de teste" <<k<<endl;
		for(int l=0; l < 10; l++)
		{
			cout<<"Slice:"<<sr[l].sliceNumber<<" Rank:"<<sr[l].value<<endl;
		}
		
*/
	//	cout <<k<<endl;
	//}
	time = ((double)getTickCount() - time)/getTickFrequency();
	cout << "Time of MSSIM CPU (averaged for " << slices << " runs): " << time << " seconds."<<endl;
}

int main(int argc, char *argv[])
{
	char* dataset[2];
	dataset[0]=argv[1];
	dataset[1]=argv[2];
	short unsigned int width = atoi(argv[3]);
	short unsigned int height = atoi(argv[4]);
	short unsigned int slices = atoi(argv[5]);

	FILE* inFile;
	fpos_t position;

	// allocate memory for the 3d dataset
	datasetRaw[0] = (unsigned short**)malloc(height * sizeof(unsigned short*));
	datasetRaw[1] = (unsigned short**)malloc(height * sizeof(unsigned short*));

	for (int i=0; i < height; i++)
	{
		datasetRaw[0][i] = (unsigned short*)malloc(sizeof(unsigned short) * (slices*width));
		datasetRaw[1][i] = (unsigned short*)malloc(sizeof(unsigned short) * (slices*width));
	}
	cout << sizeof(unsigned int)<<endl;
	for(int k=0; k<2;k++)
	{
		if( inFile = fopen( dataset[k], "rb" ) )
		{
			// read file into dataset matrix
			int rrr=width*height;
			for( int index=0; index<slices*rrr; index++) 
			{
				unsigned short value;
				fread( &value, 1, sizeof(unsigned short), inFile );
				int n_slice = (index%(rrr))/width;
				int n_j = index%width + (int)index/rrr;
				datasetRaw[k][n_slice][n_j] = value;
				if (index%rrr==0)
					printf("ALOHA PLAYBOY    %d  %d     %d\n",index,n_slice,n_j);
			}
			//for( int i = 0; i < slices; i++ )
			//{
			//	for( int j = 0; j < rrr; j++ )
			//	{

			//		unsigned short value;
			//		int n_slice = j/width;
			//		int n_j = j%width+i*slices;
			//		fread( &value, 1, sizeof(unsigned short), inFile );
			//		datasetRaw[k][n_slice][n_j] = value;
			//		//fgetpos(inFile, &position);					
			//	}
			//	//position=width*height-position;
			//	//fsetpos(inFile, &position);
			//}
			fclose(inFile);
		}
		else
		{
			cout << "Fail to load dataset "<<dataset[k]<<endl;
		}
	}


	// split the dataset into image planes for easy data access
	for( int k = 0; k < 2; k++ )
	{
		for( int i = 0; i < height; i++ )
		{
			Mat slice(slices,width,CV_16UC1,datasetRaw[k][i]);
			Mat plane;
			slice.convertTo(plane,CV_8UC3);
			datasetSlices[k].push_back(plane);
		}
	}
//	int to string
//		stringstream output;
//		output << i;
//		string sulfix = output.str();
//		const char * ss = sulfix.c_str();

	namedWindow("Trackbar",0);
	createTrackbar("TB","Trackbar",0,slices-1,onTrackbar);
	onTrackbar(0,0);
	
	rankBuilder(slices);

//	cout << "Slice mais parecido:"<<out[1] << " "<< setprecision(3)<<out[0]*100<<"%"<<endl;

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
