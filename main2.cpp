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

	ImageInfo img1, img2;
	
	img1.fileName=argv[1];
	img1.resWidth = atoi(argv[2]);
	img1.resHeight = atoi(argv[3]);
	img1.initStack = atoi(argv[4]);
	img1.endStack = atoi(argv[5]);
	img1.resDepth = img1.endStack - img1.initStack;
	img2.fileName=argv[6];
	img2.resWidth = atoi(argv[7]);
	img2.resHeight = atoi(argv[8]);
	img2.initStack = atoi(argv[9]);
	img2.endStack = atoi(argv[10]);
	img2.resDepth = img2.endStack - img2.initStack;

	HandleData handleData(img1,img2);

	handleData.similarityCheck();

    return 0;
}
