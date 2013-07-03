#include "qualityassessment.h"



QualityAssessment::QualityAssessment()
{
    rank_size=10;
}

QualityAssessment::~QualityAssessment()
{
}


vector<twoInts> QualityAssessment::checkSimilarity(Handle3DDataset *dataset1, Handle3DDataset *dataset2)
{
	vector<gpu::GpuMat> d1 = splitDataset(dataset1);
	vector<gpu::GpuMat> d2 = splitDataset(dataset2);
    DATAINFO imgInfoDataset1 = dataset1->getImageInfo();
    DATAINFO imgInfoDataset2 = dataset2->getImageInfo();
    
//    vector<sliceRank> & ref_sr = *sr;
//    vector<sliceRank> & ref_sr_ranked = *sr_ranked;
    
    for(int i=0; i<imgInfoDataset1.resDepth; i++)
    {
        for(int j=0; j<imgInfoDataset2.resDepth; j++)
        {
            mpsnrV = getPSNR_GPU_optimized(d1[i],d2[j],bufferPSNR);    
            if(mpsnrV.val[0]==0) 
                sr_raw.value=100;
            else
                sr_raw.value = mpsnrV.val[0];
            sr_raw.sliceNumber=j;
            sr.push_back(sr_raw);
        }

        ordenaRank(&sr);

        sr.clear();
        sr_ranked.clear();
        
        for(int j=0; j<rank_size; j++)
        {
            mssimV=getMSSIM_GPU_optimized(d1[i],d2[sr[j].sliceNumber],bufferMSSIM);//compara dataset1xdataset2 com SSIM
            sr_raw.value = mssimV.val[0];
            sr_raw.sliceNumber=sr[j].sliceNumber;            
            sr_ranked.push_back(sr_raw);
        }
        
        ordenaRank(&sr_ranked);

        sliceAndDistance.sliceNumber=i;
        sliceAndDistance.distanceToOptimal=sr_ranked[0].sliceNumber-i;
        bestMatches.push_back(sliceAndDistance);
    }
    return bestMatches;
}

vector<gpu::GpuMat> QualityAssessment::splitDataset(Handle3DDataset *dataset)
{
	
    DATAINFO imgInfo = dataset->getImageInfo();	
    
	for( int i = 0; i < imgInfo.resDepth; i++ )
	{
        unsigned short** d = dataset->getDataset();
		Mat slice(imgInfo.resHeight,imgInfo.resWidth,CV_16UC1,d[i]);
		Mat plane;
		gpu::GpuMat planeGPU;
		slice.convertTo(plane,CV_8UC3);
		//datasetSlices.push_back(plane);
		planeGPU.upload(plane);
		datasetSlicesGPU.push_back(planeGPU);
	}
	return datasetSlicesGPU;

}

void QualityAssessment::ordenaRank(vector<sliceRank> *srr)
{
//  std::sort(srr.begin(),srr.end());
//  bubble bunda pra gerar o ranking
    for(unsigned int j=0; j<srr->size(); j++) 
        for(unsigned int i=0; i<srr->size()-1; i++)
            if((*srr)[i+1].value > (*srr)[i].value)
                std::swap((*srr)[i+1], (*srr)[i]);
}


Scalar QualityAssessment::getPSNR(const Mat& i1, const Mat& i2)
{
    Mat s1;
    absdiff(i1, i2, s1);       // |i1 - i2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |i1 - i2|^2

    Scalar s = sum(s1);         // sum elements per channel

    double sse = s.val[0];// + s.val[1] + s.val[2]; // sum channels

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
//#define GB_R 0.5
//#define GB_S 1
#define GB_R 10.5 //used in tests
#define GB_S 31  //used in tests

Scalar QualityAssessment::getMSSIM( const Mat& i1, const Mat& i2)
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
    GaussianBlur(I1, mu1, Size(GB_S, GB_S), GB_R);
    GaussianBlur(I2, mu2, Size(GB_S, GB_S), GB_R);

    Mat mu1_2   =   mu1.mul(mu1);
    Mat mu2_2   =   mu2.mul(mu2);
    Mat mu1_mu2 =   mu1.mul(mu2);

    Mat sigma1_2, sigma2_2, sigma12;

    GaussianBlur(I1_2, sigma1_2, Size(GB_S, GB_S), GB_R);
    sigma1_2 -= mu1_2;

    GaussianBlur(I2_2, sigma2_2, Size(GB_S, GB_S), GB_R);
    sigma2_2 -= mu2_2;

    GaussianBlur(I1_I2, sigma12, Size(GB_S, GB_S), GB_R);
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

Scalar QualityAssessment::getSURF( const Mat& i1, const Mat& i2)
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

  Scalar v;
  v.val[0]=min_dist;
  v.val[1]=max_dist;
  return v;
}


Scalar QualityAssessment::getPSNR_GPU_optimized(const gpu::GpuMat& I1, const gpu::GpuMat& I2, BufferPSNR& b)
{
    b.gI1=I1;
    b.gI2=I2;

    b.gI1.convertTo(b.t1, CV_32F);
    b.gI2.convertTo(b.t2, CV_32F);

    gpu::absdiff(b.t1.reshape(1), b.t2.reshape(1), b.gs);
    gpu::multiply(b.gs, b.gs, b.gs);

    double sse = gpu::sum(b.gs, b.buf)[0];

    if( sse <= 1e-10) // for small values return zero
        return 0;
    else
    {
        double mse = sse /(double)(I1.channels());// * I1.total());
        //double psnr = 10.0*log10((255*255)/mse);

        Scalar psnr;
		psnr.val[0]= 10.0*log10((255*255)/mse);
        return psnr;

    }
}


Scalar QualityAssessment::getMSSIM_GPU_optimized( const gpu::GpuMat& i1, const gpu::GpuMat& i2, BufferMSSIM& b)
{
    const float C1 = 6.5025f, C2 = 58.5225f;
    /***************************** INITS **********************************/

    b.gI1=i1;
    b.gI2=i2;

    gpu::Stream stream;

    stream.enqueueConvert(b.gI1, b.t1, CV_32F);
    stream.enqueueConvert(b.gI2, b.t2, CV_32F);

    gpu::split(b.t1, b.vI1, stream);
    gpu::split(b.t2, b.vI2, stream);
    Scalar mssim;
    gpu::GpuMat buf;

    for( int i = 0; i < b.gI1.channels(); ++i )
    {
        gpu::multiply(b.vI2[i], b.vI2[i], b.I2_2, stream);        // I2^2
        gpu::multiply(b.vI1[i], b.vI1[i], b.I1_2, stream);        // I1^2
        gpu::multiply(b.vI1[i], b.vI2[i], b.I1_I2, stream);       // I1 * I2

        gpu::GaussianBlur(b.vI1[i], b.mu1, Size(31, 31), buf, 10.5, 0, BORDER_DEFAULT, -1, stream);
        gpu::GaussianBlur(b.vI2[i], b.mu2, Size(31, 31), buf, 10.5, 0, BORDER_DEFAULT, -1, stream);

        gpu::multiply(b.mu1, b.mu1, b.mu1_2, stream);
        gpu::multiply(b.mu2, b.mu2, b.mu2_2, stream);
        gpu::multiply(b.mu1, b.mu2, b.mu1_mu2, stream);

        gpu::GaussianBlur(b.I1_2, b.sigma1_2, Size(31, 31), buf, 10.5, 0, BORDER_DEFAULT, -1, stream);
        gpu::subtract(b.sigma1_2, b.mu1_2, b.sigma1_2, gpu::GpuMat(), -1, stream);
        //b.sigma1_2 -= b.mu1_2;  - This would result in an extra data transfer operation

        gpu::GaussianBlur(b.I2_2, b.sigma2_2, Size(31, 31), buf, 10.5, 0, BORDER_DEFAULT, -1, stream);
        gpu::subtract(b.sigma2_2, b.mu2_2, b.sigma2_2, gpu::GpuMat(), -1, stream);
        //b.sigma2_2 -= b.mu2_2;

        gpu::GaussianBlur(b.I1_I2, b.sigma12, Size(31, 31), buf, 10.5, 0, BORDER_DEFAULT, -1, stream);
        gpu::subtract(b.sigma12, b.mu1_mu2, b.sigma12, gpu::GpuMat(), -1, stream);
        //b.sigma12 -= b.mu1_mu2;

        //here too it would be an extra data transfer due to call of operator*(Scalar, Mat)
        gpu::multiply(b.mu1_mu2, 2, b.t1, 1, -1, stream); //b.t1 = 2 * b.mu1_mu2 + C1;
        gpu::add(b.t1, C1, b.t1, gpu::GpuMat(), -1, stream);
        gpu::multiply(b.sigma12, 2, b.t2, 1, -1, stream); //b.t2 = 2 * b.sigma12 + C2;
        gpu::add(b.t2, C2, b.t2, gpu::GpuMat(), -12, stream);

        gpu::multiply(b.t1, b.t2, b.t3, 1, -1, stream);     // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

        gpu::add(b.mu1_2, b.mu2_2, b.t1, gpu::GpuMat(), -1, stream);
        gpu::add(b.t1, C1, b.t1, gpu::GpuMat(), -1, stream);

        gpu::add(b.sigma1_2, b.sigma2_2, b.t2, gpu::GpuMat(), -1, stream);
        gpu::add(b.t2, C2, b.t2, gpu::GpuMat(), -1, stream);


        gpu::multiply(b.t1, b.t2, b.t1, 1, -1, stream);     // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))
        gpu::divide(b.t3, b.t1, b.ssim_map, 1, -1, stream);      // ssim_map =  t3./t1;

        stream.waitForCompletion();

        Scalar s = gpu::sum(b.ssim_map, b.buf);
        mssim.val[i] = s.val[0] / (b.ssim_map.rows * b.ssim_map.cols);

    }

    return mssim;
}

