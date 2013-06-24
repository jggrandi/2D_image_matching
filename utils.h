#ifndef UTILS
#define UTILS

#include <opencv2/gpu/gpu.hpp> 
#include <vector>


using namespace cv;

typedef struct options
{
	int verbose;
	int logdata;
	char *logfilename;
}OPT;


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



inline bool operator<(const sliceRank &i, const sliceRank &j){return i.value < j.value;}


#endif // UTILS

