/* Quality assessment
	by: Jeronimo G. Grandi
	Jun,2013
*/  

#ifndef SIMILARITYASSESSMENT
#define SIMILARITYASSESSMENT

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/gpu/gpu.hpp> 

#include <handle3ddataset.h>
#include <qualityassessment.h>

#include "utils.h"

using namespace cv;


class SimilarityAssessment
{
public:
	SimilarityAssessment();
	~SimilarityAssessment();
	vector<twoInts> checkSimilarity(Handle3DDataset <imgT>dataset1, Handle3DDataset <imgT>dataset2, int option, bool gpuoptimization);
    void checkWithPSNR( Handle3DDataset <imgT>dataset1, Handle3DDataset <imgT>dataset2, bool gpu);
    void checkWithSSIM( Handle3DDataset <imgT>dataset1, Handle3DDataset <imgT>dataset2, bool gpu);
    void checkWith2STEP(Handle3DDataset <imgT>dataset1, Handle3DDataset <imgT>dataset2, bool gpu);
    vector<gpu::GpuMat> splitDatasetGPU(Handle3DDataset <imgT>dataset);
    vector<Mat>         splitDatasetCPU(Handle3DDataset <imgT>dataset);
    void ordenaRank(vector<sliceRank> *srr, bool upward);
private:
    int rank_size;
    Scalar similarityResult;
    sliceRank sr_raw;
    vector<sliceRank> sr,sr_ranked;
    twoInts sliceAndDistance;
    vector<twoInts> bestMatches;
    QualityAssessment qualAssess;
    vector<Mat> d1, d2;
    vector<gpu::GpuMat>  gd1, gd2;
};





#endif // QUALITY_ASSESSMENT