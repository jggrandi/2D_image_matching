/* Quality assessment
	by: Jeronimo G. Grandi
	Jun,2013
*/  

#ifndef QUALITY_ASSESSMENT
#define QUALITY_ASSESSMENT



#include <fstream>
#include <cstdlib>

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/gpu/gpu.hpp> 

#include "systemmessages.h"
#include "imageinfo.h"
#include "loaddata.h"


using namespace cv;


struct BufferPSNR                                     // Optimized GPU versions
{   // Data allocations are very expensive on GPU. Use a buffer to solve: allocate once reuse later.
    gpu::GpuMat gI1, gI2, gs, t1,t2;
    gpu::GpuMat buf;
};


struct BufferMSSIM                                     // Optimized GPU versions
{   // Data allocations are very expensive on GPU. Use a buffer to solve: allocate once reuse later.
    gpu::GpuMat gI1, gI2, gs, t1,t2;
    gpu::GpuMat I1_2, I2_2, I1_I2;
    vector<gpu::GpuMat> vI1, vI2;
    gpu::GpuMat mu1, mu2;
    gpu::GpuMat mu1_2, mu2_2, mu1_mu2;
    gpu::GpuMat sigma1_2, sigma2_2, sigma12;
    gpu::GpuMat t3;
    gpu::GpuMat ssim_map;
    gpu::GpuMat buf;
};

struct sliceRank
{
    int sliceNumber;
    float value;
};

struct twoInts
{
    int sliceNumber;
    int distanceToOptimal;
};

inline bool operator<(const sliceRank &i, const sliceRank &j){return i.value < j.value;}

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
	void checkSimilarity(LoadData dataset1, LoadData dataset2);	
    vector<gpu::GpuMat> splitDataset(LoadData dataset);
    void ordenaRank(vector<sliceRank> &srr);
    friend bool operator<(const sliceRank &, const sliceRank &);
private:
    BufferPSNR bufferPSNR;
    BufferMSSIM bufferMSSIM;
    Scalar mssimV,msurfV,mpsnrV;
    unsigned short **datasetWHShrink;
    vector<Mat> datasetSlices;
	vector<gpu::GpuMat> datasetSlicesGPU;
    sliceRank sr_raw;
    std::vector<sliceRank> sr,sr_ranked,sr_ranked2;
    int rank_size;
    twoInts *sliceAndDistance;
};





#endif // QUALITY_ASSESSMENT