#include "handledata.h"

HandleData::HandleData(){}

HandleData::HandleData(DATAINFO h_img1, DATAINFO h_img2, OPT h_options)
{
	oopt = h_options;

	dataset1(h_img1);
	dataset2(h_img2);

	if(!dataset1.loadFile()) {printf("Fail to load %s\n",h_img1.fileName);return;}
	else printf("%s loaded!\n",h_img1.fileName );

	if(!dataset2.loadFile()){printf("Fail to load %s\n",h_img2.fileName);return;}
	else printf("%s loaded!\n",h_img2.fileName );
}

HandleData::~HandleData(){}

void HandleData::similarityCheck()
{
	similarityResults = q.checkSimilarity(dataset1,dataset2);

	if(oopt.logdata)
	{
		logData.handleLog(oopt.logfilename, similarityResults, img1, img2);
	}
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



