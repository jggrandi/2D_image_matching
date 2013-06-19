#include <iostream>
#include <string>
#include <cstdlib>

#include <vector>
#include <fstream>

#include "handledata.h"
#include "imageinfo.h"
#include "systemmessages.h"

using namespace std;


int main(int argc, char *argv[])
{

	if (argc != 11)
	{
		printUsage();
		return 0;
	}

	ImageInfo img1Info, img2Info;
	
	img1Info.fileName=argv[1];
	img1Info.resWidth = atoi(argv[2]);
	img1Info.resHeight = atoi(argv[3]);
	img1Info.initStack = atoi(argv[4]);
	img1Info.endStack = atoi(argv[5]);
	img1Info.resDepth = img1Info.endStack - img1Info.initStack;
	img2Info.fileName=argv[6];
	img2Info.resWidth = atoi(argv[7]);
	img2Info.resHeight = atoi(argv[8]);
	img2Info.initStack = atoi(argv[9]);
	img2Info.endStack = atoi(argv[10]);
	img2Info.resDepth = img2Info.endStack - img2Info.initStack;

	HandleData handleData(img1Info,img2Info);

	handleData.similarityCheck();

	printf("TCHAU\n"); 
    return 0;
}
