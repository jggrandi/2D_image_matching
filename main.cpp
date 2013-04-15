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

#define RANK_SIZE 10
#define QNT_DATASETS 2

using namespace std;
using namespace cv;

vector <vector <vector < unsigned short> > > datasetRaw[QNT_DATASETS];
vector <vector <vector < unsigned short> > > datasetNewPlane[QNT_DATASETS];
unsigned short **datasetWHShrink[QNT_DATASETS];
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

void ordenaRank(int slices, vector<sliceRank> &srr,int algorithm)
{
	for(int j=0; j<slices; j++) //bubble bunda pra gerar o ranking
	{
		for(int i=0; i<slices-1; i++)
		{
			if(algorithm != 2)
			{
				if(srr[i+1].value > srr[i].value)
					swap(srr[i+1], srr[i]);
			}
			else
			{
				if(srr[i+1].value < srr[i].value) //se o algoritmo é surf, a ordem de ordenação deve ser diferente
					swap(srr[i+1], srr[i]);

			}

		}
	}
}

sliceRank* calculaSimilaridade(int algorithm, int slices)
{
	int summ[10];

	for(int j=0;j<10;j++)
		summ[j]=0;
	
	Scalar mssimV,msurfV,mpsnrV;
	sliceRank sr_raw;
	vector<sliceRank> sr,sr_ranked,sr_ranked2;

	for(int i=0;i<slices;i++)
	{
		cout << i<<endl; //display slice atual a ser processado
		outFile << i<<";"; 
		for(int k=0;k<slices;k++)
		{	
			switch(algorithm)
			{
			case 0:
				{
					mpsnrV=getPSNR(datasetSlices[0][i],datasetSlices[1][k]); //compara dataset1xdataset2 con PSNR	
					if(mpsnrV.val[0]==0) 
						sr_raw.value=100;
					else
						sr_raw.value = mpsnrV.val[0];
					sr_raw.sliceNumber=k;
					sr.push_back(sr_raw);
					break;
				}
			case 1:
				{
					mssimV=getMSSIM(datasetSlices[0][i],datasetSlices[1][k]);//compara dataset1xdataset2 com SSIM
					sr_raw.value = mssimV.val[0];
					sr_raw.sliceNumber=k;
					sr.push_back(sr_raw);
					break;
				}
			case 2:
				{
					msurfV=getSURF(datasetSlices[0][i],datasetSlices[1][k]); //compara dataset1xdataset2 com SURF
					sr_raw.value = msurfV.val[0];
					sr_raw.sliceNumber=k;
					sr.push_back(sr_raw);
					break;
				}

			default:
				break;
			}


		}
		sr_ranked.clear();
		sr_ranked=sr;
		sr.clear();
		
		ordenaRank(slices,sr_ranked,algorithm);
		sr_ranked.resize(RANK_SIZE);
		
		for(int k=0; k<RANK_SIZE;k++)
		{
			mssimV=getMSSIM(datasetSlices[0][i],datasetSlices[1][sr_ranked[k].sliceNumber]);//compara dataset1xdataset2 com SSIM
			sr_raw.value = mssimV.val[0];
			sr_raw.sliceNumber=sr_ranked[k].sliceNumber;			
			sr_ranked2.push_back(sr_raw);
		}

		ordenaRank(RANK_SIZE,sr_ranked2,algorithm);
		

		for(int k=0; k<RANK_SIZE;k++)
		{
			printf("<R1> sN:%d, v:%f - <R2> sN:%d, v:%f\n",sr_ranked[k].sliceNumber,sr_ranked[k].value,sr_ranked2[k].sliceNumber,sr_ranked2[k].value);
		}

		for (int k=0; k<RANK_SIZE; k++)
		{

			int srr1=sr_ranked[k].sliceNumber-i;
			int srr2=sr_ranked2[k].sliceNumber-i;
			if(srr1<0) srr1=srr1*-1; 
			if(srr2<0) srr2=srr2*-1;
			if(srr1<=srr2)
			{
				if(sr_ranked[k].sliceNumber==i) //verifica se o algoritmo encontrou o slice na mesma posiçao do dataset analizado  
				{
					summ[k]++;	//incrementa summ para exibir qntos slices foram exatamente encontrados na mesma posiçao do dataset analizado
					break;				
				}
			}
			else if(srr1>srr2)
			{
				if(sr_ranked2[k].sliceNumber==i) //verifica se o algoritmo encontrou o slice na mesma posiçao do dataset analizado  
				{
					summ[k]++;
					break;
				}
			}
		}

		//for(int l=0; l < RANK_SIZE; l++)
		//{
			//cout<<"Slice:"<<sr[l].sliceNumber<<" Rank:"<<sr[l].value<<endl;
			//outFile<<sr_ranked[l].sliceNumber<<endl;
			//cout<<sr_ranked[l].sliceNumber<<endl;
		//}
		//outFile << timeAlgorithm <<endl;
		sr_ranked2.clear();
	}

	for(int ll=0; ll < RANK_SIZE; ll++)
	{
		//outFile <<endl<< summ[ll];
		cout <<endl<< summ[ll];
	}

	//return sr_ranked;
	return NULL;
}
void rankBuilder(int slices, int algorithm)
{
	sliceRank *sr_result;
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
		
	if(algorithm==3)//se algorithm == 3 os datasets são processados utilizando-se todas as técnicas 
	{
		for(int j=0, algorithm=0;j<3;j++,algorithm++)
		{
			timeAlgorithm = (double)getTickCount();
			sr_result=calculaSimilaridade(algorithm,slices);
			timeAlgorithm = ((double)getTickCount() - timeAlgorithm)/getTickFrequency();	
			printf("\n%f\n",timeAlgorithm);
		}
	}
	else
	{
		sr_result=calculaSimilaridade(algorithm,slices);
	}



	//outFile <<endl<< "Slice time: "<<time2<<endl;
//	cout <<endl<< "Slice time: "<<time2<<endl;
	timeTotal = ((double)getTickCount() - timeTotal)/getTickFrequency();
	//cout << "Time of MSSIM CPU (averaged for " << slices << " runs): " << time << " seconds."<<endl;
	outFile << endl<<"Total time: " << timeTotal <<endl;
//	cout << "Total time: " << time <<endl;
}


int loadDatasets(char** l_datasets,short unsigned int l_width, short unsigned int l_height, short unsigned int l_slices)
{

	// allocate memory for the 3d dataset
	for(int k=0; k<QNT_DATASETS;k++)
	{
		datasetRaw[k].resize(l_slices);
		for(int i=0;i<l_slices;i++)
		{
			datasetRaw[k][i].resize(l_width);
			for(int j=0;j<l_width;j++)
			{
				datasetRaw[k][i][j].resize(l_height);
			}
		}
	}

	for(int k=0; k<QNT_DATASETS;k++)
	{
		if(!(inFile = fopen( l_datasets[k], "rb" ) ))
		{
			printf("Problems when tried to open the dataset %d\n",l_datasets[k]);
			return -1;
		}
		for(int i=0; i<l_slices;i++)
		{
			for(int j=0;j<l_width;j++)
			{
				for(int l=0;l<l_height;l++)
				{
					unsigned short value;
					fread( &value, 1, sizeof(unsigned short), inFile );
					datasetRaw[k][i][j][l] = value;
				}
			}
		}
		fclose(inFile);
	}

	return 1;
}

int changePlane(short unsigned int c_width, short unsigned int c_height, short unsigned int c_slices)
{								   //slices						//height						//width

	for(int k=0; k<QNT_DATASETS;k++)
	{
		datasetNewPlane[k].resize(c_slices);
		for(int i=0;i<c_slices;i++)
		{
			datasetNewPlane[k][i].resize(c_width);
			for(int j=0;j<c_width;j++)
			{
				datasetNewPlane[k][i][j].resize(c_height);
			}
		}
	}

	for(int k=0; k<QNT_DATASETS;k++)
	{
		for(int i=0; i<c_slices;i++)
		{
			for(int j=0;j<c_width;j++)
			{
				for(int l=0;l<c_height;l++)
				{
					//datasetNewPlane[k][i][j][l] = datasetRaw[k][j][l][i];
					datasetNewPlane[k][i][j][l] = datasetRaw[k][j][i][l];
				}
			}
		}
	}

	return 1;
}


void splitDatasets(short unsigned int s_width,short unsigned int s_height,short unsigned int s_slices)
{									 //slices					//height					//width
	// split the dataset into image planes for easy data access
	datasetWHShrink[0] = (unsigned short**)malloc(s_slices * sizeof(unsigned short*));
	datasetWHShrink[1] = (unsigned short**)malloc(s_slices * sizeof(unsigned short*));

	for (int i=0; i < s_slices; i++)
	{
		datasetWHShrink[0][i] = (unsigned short*)malloc(sizeof(unsigned short) * (s_height*s_width));
		datasetWHShrink[1][i] = (unsigned short*)malloc(sizeof(unsigned short) * (s_height*s_width));
	}
	
	int rrr=s_width*s_height;
	
	for( int k = 0; k < QNT_DATASETS; k++ )
	{
		for( int i = 0; i < s_slices; i++ )
		{
			for(int j=0; j<s_width;j++)
			{
				for(int l=0; l<s_height;l++)
				{
					//int jj= (l+j)%s_width + (int)(l+j)/s_width;
					//datasetWHShrink[k][i][l * s_width + j]=datasetNewPlane[k][i][j][l];
					datasetWHShrink[k][i][j * s_width + l]=datasetRaw[k][i][j][l];
				}
			}
		}
	}
	for( int k = 0; k < QNT_DATASETS; k++ )
	{
		for( int i = 0; i < s_slices; i++ )
		{
			Mat slice(s_height,s_width,CV_16UC1,datasetWHShrink[k][i]);
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

	if(loadDatasets(dataset,width,height,slices)==-1)
		return -1;
	printf("Loading datasets done!\n");
	//if(changePlane(slices,width,height)==-1)
	//	return -1;
	printf("Change plane done!\n");
	splitDatasets(width,height,slices);


	

	namedWindow("Trackbar",0);
	createTrackbar("TB","Trackbar",0,slices-1,onTrackbar);
	onTrackbar(0,0);

	//rankBuilder(slices,algorithm);

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