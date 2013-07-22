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
#include <qualityassessment_utils.h>
    
#include "utils.h"

using namespace cv;


class SimilarityAssessment
{
public:
	SimilarityAssessment();
	~SimilarityAssessment();
	vector<twoInts> checkSimilarity(Handle3DDataset *dataset1, Handle3DDataset *dataset2);	
    vector<gpu::GpuMat> splitDataset(Handle3DDataset *dataset);
    void ordenaRank(vector<sliceRank> *srr);
private:
    Scalar mssimV,msurfV,mpsnrV;
    vector<Mat> datasetSlices;
	vector<gpu::GpuMat> datasetSlicesGPU;
    sliceRank sr_raw;
    vector<sliceRank> sr,sr_ranked;
    int rank_size;
    twoInts sliceAndDistance;
    vector<twoInts> bestMatches;
    QualityAssessment qualAssess;
};





#endif // QUALITY_ASSESSMENT