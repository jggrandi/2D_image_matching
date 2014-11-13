#include "similarityassessment.h"



SimilarityAssessment::SimilarityAssessment()
{
    rank_size=10;
}

SimilarityAssessment::~SimilarityAssessment(){}


vector<twoInts> SimilarityAssessment::checkSimilarity(Handle3DDataset <imgT>dataset1, Handle3DDataset <imgT>dataset2, int option, bool gpuoptimization)
{

    switch(option)
    {
        case 0: checkWithPSNR (dataset1, dataset2, gpuoptimization); break;
        case 1: checkWithSSIM (dataset1, dataset2, gpuoptimization); break;
        case 2: checkWith2STEP(dataset1, dataset2, gpuoptimization); break;
        default: break;
    }

    return bestMatches;
}

void SimilarityAssessment::checkWithPSNR(Handle3DDataset <imgT>dataset1, Handle3DDataset <imgT>dataset2, bool gpu)
{
    if(gpu) { gd1 = splitDatasetGPU(dataset1); gd2 = splitDatasetGPU(dataset2);}
    else    { d1  = splitDatasetCPU(dataset1); d2  = splitDatasetCPU(dataset2);}

    DATAINFO imgInfoDataset1 = dataset1.getDatasetInfo(0);
    DATAINFO imgInfoDataset2 = dataset2.getDatasetInfo(0); 
    
    for(int i=0; i<imgInfoDataset1.resDepth; i++)
    {
        for(int j=0; j<imgInfoDataset2.resDepth; j++)
        {
            if(gpu) similarityResult = qualAssess.getPSNR_GPU_optimized(gd1[i],gd2[j]);    
            else    similarityResult = qualAssess.getPSNR(d1[i],d2[j]);    
            sr_raw.value = similarityResult.val[0];
            sr_raw.sliceNumber=j;
            sr.push_back(sr_raw);
        }

        ordenaRank(&sr,1); //upward ordenation

        sliceAndDistance.sliceNumber=i;
        sliceAndDistance.distanceToOptimal=sr[0].sliceNumber-i;
        sliceAndDistance.value=sr[0].value;
        bestMatches.push_back(sliceAndDistance);
        sr.clear();
    }
}

void SimilarityAssessment::checkWithSSIM(Handle3DDataset <imgT>dataset1, Handle3DDataset <imgT>dataset2, bool gpu)
{

    if(gpu) { gd1 = splitDatasetGPU(dataset1); gd2 = splitDatasetGPU(dataset2);}
    else    { d1  = splitDatasetCPU(dataset1); d2  = splitDatasetCPU(dataset2);}

    DATAINFO imgInfoDataset1 = dataset1.getDatasetInfo(0);
    DATAINFO imgInfoDataset2 = dataset2.getDatasetInfo(0); 

    for(int i=0; i<imgInfoDataset1.resDepth; i++)
    {
        for(int j=0; j<imgInfoDataset2.resDepth; j++)
        {
            if(gpu) similarityResult = qualAssess.getMSSIM_GPU_optimized(gd1[i],gd2[j]);    
            else    similarityResult = qualAssess.getMSSIM(d1[i],d2[j]);    
            sr_raw.value = similarityResult.val[0];
            sr_raw.sliceNumber=j;
            sr.push_back(sr_raw);
        }

        ordenaRank(&sr,0); //downward ordenation

        sliceAndDistance.sliceNumber=i;
        sliceAndDistance.distanceToOptimal=sr[0].sliceNumber-i;
        sliceAndDistance.value=sr[0].value;
        bestMatches.push_back(sliceAndDistance);
        sr.clear();
    }

}


void SimilarityAssessment::checkWith2STEP(Handle3DDataset <imgT>dataset1, Handle3DDataset <imgT>dataset2, bool gpu)
{
    if(gpu) { gd1 = splitDatasetGPU(dataset1); gd2 = splitDatasetGPU(dataset2);}
    else    { d1  = splitDatasetCPU(dataset1); d2  = splitDatasetCPU(dataset2);}

    DATAINFO imgInfoDataset1 = dataset1.getDatasetInfo(0);
    DATAINFO imgInfoDataset2 = dataset2.getDatasetInfo(0); 

    for(int i=0; i<imgInfoDataset1.resDepth; i++)
    {
        for(int j=0; j<imgInfoDataset2.resDepth; j++)
        {
            if(gpu) similarityResult = qualAssess.getPSNR_GPU_optimized(gd1[i],gd2[j]);    
            else    similarityResult = qualAssess.getPSNR(d1[i],d2[j]);    
            sr_raw.value = similarityResult.val[0];
            sr_raw.sliceNumber=j;
            sr.push_back(sr_raw);
        }

        ordenaRank(&sr,1);
        
        for(int j=0; j<rank_size; j++)
        {
            if(gpu) similarityResult = qualAssess.getMSSIM_GPU_optimized(gd1[i],gd2[sr[j].sliceNumber]);//compara dataset1xdataset2 com SSIM
            else    similarityResult = qualAssess.getMSSIM(d1[i],d2[sr[j].sliceNumber]);//compara dataset1xdataset2 com SSIM
            sr_raw.value = similarityResult.val[0];
            sr_raw.sliceNumber=sr[j].sliceNumber;            
            sr_ranked.push_back(sr_raw);
        }
        
        ordenaRank(&sr_ranked,0);

        sliceAndDistance.sliceNumber=i;
        sliceAndDistance.distanceToOptimal=sr_ranked[0].sliceNumber-i;
        sliceAndDistance.value=sr_ranked[0].value;
        bestMatches.push_back(sliceAndDistance);

        sr.clear();
        sr_ranked.clear();
    }
}




vector<gpu::GpuMat> SimilarityAssessment::splitDatasetGPU(Handle3DDataset <imgT>dataset)
{
	
    DATAINFO imgInfo = dataset.getDatasetInfo(0);	
    vector<gpu::GpuMat> datasetSlicesGPU;

	for( int i = 0; i < imgInfo.resDepth; i++ )
	{
        unsigned short** d = dataset.getDataset(0);
		Mat slice(imgInfo.resHeight,imgInfo.resWidth,CV_16UC1,d[i]);
		Mat plane;
		gpu::GpuMat planeGPU;
		slice.convertTo(plane,CV_8UC3);
		planeGPU.upload(plane);
		datasetSlicesGPU.push_back(planeGPU);
	}
	return datasetSlicesGPU;

}

vector<Mat> SimilarityAssessment::splitDatasetCPU(Handle3DDataset <imgT>dataset)
{
    
    DATAINFO imgInfo = dataset.getDatasetInfo(0);   
    vector<Mat> datasetSlices;

    for( int i = 0; i < imgInfo.resDepth; i++ )
    {
        unsigned short** d = dataset.getDataset(0);
        Mat slice(imgInfo.resHeight,imgInfo.resWidth,CV_16UC1,d[i]);
        Mat plane;
        gpu::GpuMat planeGPU;
        slice.convertTo(plane,CV_8UC3);
        datasetSlices.push_back(plane);
    }
    return datasetSlices;

}



void SimilarityAssessment::ordenaRank(vector<sliceRank> *srr, bool upward)
{
//  std::sort(srr.begin(),srr.end());
//  bubble bunda pra gerar o ranking
    for(unsigned int j=0; j<srr->size(); j++) 
    {
        for(unsigned int i=0; i<srr->size()-1; i++)
        {
            if(upward)
            {
                if((*srr)[i+1].value < (*srr)[i].value)
                    std::swap((*srr)[i+1], (*srr)[i]);
            }
            else
            {
                if((*srr)[i+1].value > (*srr)[i].value)
                    std::swap((*srr)[i+1], (*srr)[i]);
            }
        }
    }
}


