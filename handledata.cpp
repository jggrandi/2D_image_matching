#include "handledata.h"

HandleData::HandleData(){}

HandleData::HandleData(ImageInfo img1, ImageInfo img2)
{
	if(!dataset1.loadFile(img1)) printf("Fail to load %s",img1.fileName);
	else printf("%s loaded!\n",img1.fileName );

	if(!dataset2.loadFile(img2)) printf("Fail to load %s",img2.fileName);
	else printf("%s loaded!\n",img2.fileName );
}

HandleData::~HandleData(){}

void HandleData::similarityCheck()
{
	q.checkSimilarity();
}


