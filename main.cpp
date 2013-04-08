#include <iostream>
#include <string>
#include <iomanip>

#include <vector>
#include <fstream>

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
//Test and logs v2

using namespace std;
using namespace cv;

unsigned short **datasetRaw[2];
vector<Mat> datasetSlices[2];

FILE* inFile;
ofstream outFile;

Scalar getPSNR ( const Mat& I1, const Mat& I2);
Scalar getMSSIM( const Mat& I1, const Mat& I2);
Scalar getSURF( const Mat& I1, const Mat& I2);

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

sliceRank* ordenaRank(int slices, sliceRank *sr,int algorithm)
{
	sliceRank *newSr = sr;
	for(int j=0; j<slices; j++) //bubble bunda pra gerar o ranking
	{
		for(int i=0; i<slices-1; i++)
		{
			if(algorithm != 2)
			{
				if(newSr[i+1].value > newSr[i].value)
					swap(newSr[i+1], newSr[i]);
			}
			else
			{
				if(newSr[i+1].value < newSr[i].value) //se o algoritmo é surf, a ordem de ordenação deve ser diferente
					swap(newSr[i+1], newSr[i]);

			}

		}
	}
	return newSr;
}

void calculaSimilaridade(int algorithm, int slices)
{
	int summ[]={0,0,0,0,0,0,0,0,0,0};
	
	Scalar mssimV,msurfV,mpsnrV;
	sliceRank *sr,*sr_ranked;
	sr = (sliceRank*) calloc (slices,sizeof(sliceRank));

	for(int i=0;i<slices;i++)
	{
		cout << i<<";"; //display slice atual a ser processado
		outFile << i<<";"; 
		for(int k=0;k<slices;k++)
		{	
			switch(algorithm)
			{
			case 0:
				{
					mpsnrV=getPSNR(datasetSlices[0][i],datasetSlices[1][k]); //compara dataset1xdataset2 con PSNR	
					if(mpsnrV.val[0]==0) 
						sr[k].value=100;
					else
						sr[k].value = mpsnrV.val[0];
					sr[k].sliceNumber=k;
					break;
				}
			case 1:
				{
					mssimV=getMSSIM(datasetSlices[0][i],datasetSlices[1][k]);//compara dataset1xdataset2 com SSIM
					sr[k].value = mssimV.val[0];
					sr[k].sliceNumber=k;
					break;
				}
			case 2:
				{
					msurfV=getSURF(datasetSlices[0][i],datasetSlices[1][k]); //compara dataset1xdataset2 com SURF
					sr[k].value = msurfV.val[0];
					sr[k].sliceNumber=k;
					break;
				}

			default:
				break;
			}

		}
		sr_ranked = ordenaRank(slices,sr,algorithm);

		cout<<sr_ranked[0].sliceNumber<<endl;
		
		for(int l=0; l < 10; l++)
		{
			//cout<<"Slice:"<<sr[l].sliceNumber<<" Rank:"<<sr[l].value<<endl;
			outFile<<sr[l].sliceNumber<<endl;
			cout<<sr_ranked[l].sliceNumber<<endl;
			if(i==sr_ranked[l].sliceNumber) //verifica se o algoritmo encontrou o slice na mesma posiçao do dataset analizado  
				summ[l]++;	//incrementa summ para exibir qntos slices foram exatamente encontrados na mesma posiçao do dataset analizado
		}
		//outFile << timeAlgorithm <<endl;
	}

	for(int ll=0; ll < 10; ll++)
	{
		outFile <<endl<< summ[ll];
		cout <<endl<< summ[ll];
	}
}
void rankBuilder(int slices, int algorithm)
{
	double timeAlgorithm;
	double timeTotal = (double)getTickCount();
	char nFile[50];
	stringstream output;//convert
	output << algorithm;//int 
	string sulfix = output.str();// to 
	const char* ss = sulfix.c_str();// char
	strcpy(nFile,ss);
	strcat(nFile,".csv");
	outFile.open(nFile, ios::out);
	if(outFile.fail())
	{
		printf( "Could not open output file %s.\n", nFile );
		exit(-1);
	}
	else
	{
		
		if(algorithm==3)
		{
			for(int j=0, algorithm=0;j<3;j++,algorithm++)
			{
				timeAlgorithm = (double)getTickCount();
				calculaSimilaridade(algorithm,slices);
				timeAlgorithm = ((double)getTickCount() - timeAlgorithm)/getTickFrequency();	
				printf("\n%f\n",timeAlgorithm);
			}
		}
		else
		{
			calculaSimilaridade(algorithm,slices);
		}


	}
	//outFile <<endl<< "Slice time: "<<time2<<endl;
//	cout <<endl<< "Slice time: "<<time2<<endl;
	timeTotal = ((double)getTickCount() - timeTotal)/getTickFrequency();
	//cout << "Time of MSSIM CPU (averaged for " << slices << " runs): " << time << " seconds."<<endl;
	outFile << endl<<"Total time: " << timeTotal <<endl;
//	cout << "Total time: " << time <<endl;
}


void loadDatasets(char** l_datasets,short unsigned int l_width, short unsigned int l_height, short unsigned int l_slices)
{

	// allocate memory for the 3d dataset
	datasetRaw[0] = (unsigned short**)malloc(l_slices * sizeof(unsigned short*));
	datasetRaw[1] = (unsigned short**)malloc(l_slices * sizeof(unsigned short*));

	for (int i=0; i < l_slices; i++)
	{
		datasetRaw[0][i] = (unsigned short*)malloc(sizeof(unsigned short) * (l_height*l_width));
		datasetRaw[1][i] = (unsigned short*)malloc(sizeof(unsigned short) * (l_height*l_width));
	}
	for(int k=0; k<2;k++)
	{
		if( inFile = fopen( l_datasets[k], "rb" ) )
		{
			// read file into dataset matrix
			int rrr=l_width*l_height;
			for( int i = 0; i < l_slices; i++ )
			{
				for( int j = 0; j < rrr; j++ )
				{
					unsigned short value;
					fread( &value, 1, sizeof(unsigned short), inFile );
					datasetRaw[k][i][j] = value;
				}
			}
			fclose(inFile);
		}
		else
		{
			cout << "Fail to load dataset "<<l_datasets[k]<<endl;
		}
	}
}

void splitDatasets(short unsigned int s_width,short unsigned int s_height,short unsigned int s_slices)
{
	// split the dataset into image planes for easy data access
	for( int k = 0; k < 2; k++ )
	{
		for( int i = 0; i < s_slices; i++ )
		{
			Mat slice(s_height,s_width,CV_16UC1,datasetRaw[k][i]);
			Mat plane;
			slice.convertTo(plane,CV_8UC3);
			datasetSlices[k].push_back(plane);
		}
	}
}

int main(int argc, char *argv[])
{
	char* dataset[2];
	dataset[0]=argv[1];
	dataset[1]=argv[2];
	short unsigned int width = atoi(argv[3]);
	short unsigned int height = atoi(argv[4]);
	short unsigned int slices = atoi(argv[5]);
	short unsigned int algorithm = atoi(argv[6]);

	loadDatasets(dataset,width,height,slices);
	splitDatasets(width,height,slices);


	printf("Loading datasets done!\n");

	namedWindow("Trackbar",0);
	createTrackbar("TB","Trackbar",0,slices-1,onTrackbar);
	onTrackbar(0,0);

	rankBuilder(slices,algorithm);

//	cout << "Slice mais parecido:"<<out[1] << " "<< setprecision(3)<<out[0]*100<<"%"<<endl;

	waitKey(0);
	destroyAllWindows();
    return 0;
}

Scalar getPSNR(const Mat& i1, const Mat& i2)
{
    Mat s1;
    absdiff(i1, i2, s1);       // |i1 - i2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |i1 - i2|^2

    Scalar s = sum(s1);         // sum elements per channel

    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

    if( sse <= 1e-10) // for small values return zero
        return 0;
    else
    {
        double  mse =sse /(double)(i1.channels() * i1.total());
        Scalar psnr;
		psnr.val[0]= 10.0*log10((255*255)/mse);
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

Scalar getSURF( const Mat& i1, const Mat& i2)
{

  //-- Step 1: Detect the keypoints using SURF Detector
  int minHessian = 5000;

  SurfFeatureDetector detector( minHessian );

  std::vector<KeyPoint> keypoints_1, keypoints_2;

  detector.detect( i1, keypoints_1 );
  detector.detect( i2, keypoints_2 );

  SurfDescriptorExtractor extractor; //Calculate descriptors (feature vectors)

  Mat descriptors_1, descriptors_2;

  extractor.compute( i1, keypoints_1, descriptors_1 );
  extractor.compute( i2, keypoints_2, descriptors_2 );

  
  FlannBasedMatcher matcher; //Matching descriptor vectors using FLANN matcher
  std::vector< DMatch > matches;
  matcher.match( descriptors_1, descriptors_2, matches );

  double max_dist = 0; double min_dist = 100;

  //Calculation of max and min distances between keypoints
  for( int i = 0; i < descriptors_1.rows; i++ )
  { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }

  //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
  //-- PS.- radiusMatch can also be used here.
  std::vector< DMatch > good_matches;

  /*
  for( int i = 0; i < descriptors_1.rows; i++ )
  { if( matches[i].distance < 2*min_dist )
    { good_matches.push_back( matches[i]); }
  }

  //-- Draw only "good" matches
  Mat img_matches;

  drawMatches( i1, keypoints_1, i2, keypoints_2,
	  good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

  //-- Show detected matches
  imshow( "Good Matches", img_matches );

  for( int i = 0; i < good_matches.size(); i++ )
  { printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx ); }
   printf("-- Max dist : %f \n", max_dist );
  printf("-- Min dist : %f \n", min_dist );
*/

  Scalar v;
  v.val[0]=min_dist;
  v.val[1]=max_dist;
  return v;
}