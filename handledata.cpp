#include "handledata.h"

HandleData::HandleData(){}

HandleData::HandleData(ImageInfo i_img1, ImageInfo i_img2)
{
	if(!dataset1.loadFile(i_img1)) printf("Fail to load %s",i_img1.fileName);
	else printf("%s loaded!\n",i_img1.fileName );

	if(!dataset2.loadFile(i_img2)) printf("Fail to load %s",i_img2.fileName);
	else printf("%s loaded!\n",i_img2.fileName );
}

HandleData::~HandleData(){}

void HandleData::similarityCheck()
{
	q.checkSimilarity(dataset1,dataset2);
}


