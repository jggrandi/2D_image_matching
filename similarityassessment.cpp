#include "similarityassessment.h"



SimilarityAssessment::SimilarityAssessment()
{
    rank_size=10;
}

SimilarityAssessment::~SimilarityAssessment(){}


vector<twoInts> SimilarityAssessment::checkSimilarity(Handle3DDataset dataset1, Handle3DDataset dataset2)
{
	vector<gpu::GpuMat> d1 = splitDataset(dataset1);
	vector<gpu::GpuMat> d2 = splitDataset(dataset2);
    DATAINFO imgInfoDataset1 = dataset1.getDatasetInfo();
    DATAINFO imgInfoDataset2 = dataset2.getDatasetInfo();
    
//    vector<sliceRank> & ref_sr = *sr;
//    vector<sliceRank> & ref_sr_ranked = *sr_ranked;

    for(int i=0; i<imgInfoDataset1.resDepth; i++)
    {
        for(int j=0; j<imgInfoDataset2.resDepth; j++)
        {
            mpsnrV = qualAssess.getPSNR_GPU_optimized(d1[i],d2[j]);    
//            if(mpsnrV.val[0]==0) 
  //              sr_raw.value=100;
    //        else
                sr_raw.value = mpsnrV.val[0];
            sr_raw.sliceNumber=j;
            sr.push_back(sr_raw);
        }

        ordenaRank(&sr);
        
        for(int j=0; j<rank_size; j++)
        {
            mssimV = qualAssess.getMSSIM_GPU_optimized(d1[i],d2[sr[j].sliceNumber]);//compara dataset1xdataset2 com SSIM
            sr_raw.value = mssimV.val[0];
            sr_raw.sliceNumber=sr[j].sliceNumber;            
            sr_ranked.push_back(sr_raw);
        }
        
        ordenaRank(&sr_ranked);

        sliceAndDistance.sliceNumber=i;
        sliceAndDistance.distanceToOptimal=sr_ranked[0].sliceNumber-i;
        bestMatches.push_back(sliceAndDistance);

        sr.clear();
        sr_ranked.clear();
    }
    return bestMatches;
}

vector<gpu::GpuMat> SimilarityAssessment::splitDataset(Handle3DDataset dataset)
{
	
    DATAINFO imgInfo = dataset.getDatasetInfo();	
    
	for( int i = 0; i < imgInfo.resDepth; i++ )
	{
        unsigned short** d = dataset.getDataset();
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

void SimilarityAssessment::ordenaRank(vector<sliceRank> *srr)
{
//  std::sort(srr.begin(),srr.end());
//  bubble bunda pra gerar o ranking
    for(unsigned int j=0; j<srr->size(); j++) 
        for(unsigned int i=0; i<srr->size()-1; i++)
            if((*srr)[i+1].value > (*srr)[i].value)
                std::swap((*srr)[i+1], (*srr)[i]);
}


