/* Quality assessment
	by: Jeronimo G. Grandi
	Jun,2013
*/  

#ifndef QUALITY_ASSESSMENT
#define QUALITY_ASSESSMENT

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/gpu/gpu.hpp> 

#include <handle3ddataset.h>
#include "utils.h"
using namespace std;
using namespace cv;


class QualityAssessment
{
public:
	QualityAssessment();
	~QualityAssessment();
	Scalar getPSNR(const Mat& i1, const Mat& i2);
	Scalar getMSSIM( const Mat& i1, const Mat& i2);
	Scalar getSURF( const Mat& i1, const Mat& i2);
	Scalar getPSNR_GPU_optimized(const gpu::GpuMat& I1, const gpu::GpuMat& I2, BufferPSNR& b);	
	Scalar getMSSIM_GPU_optimized( const gpu::GpuMat& i1, const gpu::GpuMat& i2, BufferMSSIM& b);	
	vector<twoInts> checkSimilarity(Handle3DDataset *dataset1, Handle3DDataset *dataset2);	
    vector<gpu::GpuMat> splitDataset(Handle3DDataset *dataset);
    void ordenaRank(vector<sliceRank> *srr);
private:
    BufferPSNR bufferPSNR;
    BufferMSSIM bufferMSSIM;
    Scalar mssimV,msurfV,mpsnrV;
    vector<Mat> datasetSlices;
	vector<gpu::GpuMat> datasetSlicesGPU;
    sliceRank sr_raw;
    vector<sliceRank> sr,sr_ranked;
    int rank_size;
    twoInts sliceAndDistance;
    vector<twoInts> bestMatches;
};





#endif // QUALITY_ASSESSMENT