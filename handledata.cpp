#include "handledata.h"

HandleData::HandleData(){}

HandleData::~HandleData(){}

int HandleData::loadData(DATAINFO h_img1, DATAINFO h_img2, OPT h_options)
{
	oopt = h_options;

	dataset1 = Handle3DDataset(h_img1);
	dataset2 = Handle3DDataset(h_img2);

	if(!dataset1.loadFile()) 
	{
		printf("Failed to load %s\n",h_img1.inputFileName);
		return 1;
	}
	else
		printf("%s loaded!\n",h_img1.inputFileName );

	if(!dataset2.loadFile())
	{
		printf("Failed to load %s\n",h_img2.inputFileName);
		return 1;
	}
	else 
		printf("%s loaded!\n",h_img2.inputFileName );
	
	return 0;
}

void HandleData::similarityCheck()
{
	if(oopt.logdata)
	{
		double timeCounter= (double)getTickCount();
		similarityResults = q.checkSimilarity(dataset1,dataset2);
		timeCounter = ((double)getTickCount() - timeCounter)/getTickFrequency();
		logData.handleLog(oopt.logfilename, similarityResults,timeCounter);
	}
	else
		similarityResults = q.checkSimilarity(dataset1,dataset2);
}
/*
void HandleData::showData()
{
	namedWindow("Trackbar1",0);
	createTrackbar("TB1","Trackbar1",0,img1->endStack-img1->initStack,onTrackbar);
	onTrackbar(0,0);
	namedWindow("Trackbar2",0);
	createTrackbar("TB2","Trackbar2",0,img2->endStack-img2->initStack,onTrackbar);
	onTrackbar(0,0);

}

*/



