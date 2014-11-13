#include <cstdlib>
#include <unistd.h>
#include <vector>

//#include <handle3ddataset.h>
#include "handledata.h"
#include "utils.h"


int main(int argc, char *argv[])
{
	
	int option_index = 0;
	OPT opt;

	opt.logdata   = false;
	opt.verbose   = false;
	opt.simMetric = 2;
	opt.gpuOptimized = false; 

	while ((option_index = getopt(argc,argv,"m:l:vg")) != -1)
	switch (option_index)
	{
		case 'l': opt.logdata = true; opt.logfilename = optarg; break;
		case 'v': opt.verbose = true; 							break;
		case 'm': opt.simMetric = atoi(optarg);					break;
		case 'g': opt.gpuOptimized = true;						break;
		default : printf("%s",usage );						    break;
	}

	argc -= optind;
    argv += optind;
	
	if (argc != 10)
	{
		printf("%s",usage);
		return -1;
	}
	
	DATAINFO img1Info;
	DATAINFO img2Info;

	img1Info.fileName 	  = argv[0];
	img1Info.resWidth	  = atoi(argv[1]);
	img1Info.resHeight 	  = atoi(argv[2]);
	img1Info.initStack 	  = atoi(argv[3]);
	img1Info.endStack     = atoi(argv[4]);
	img1Info.resDepth     = img1Info.endStack - img1Info.initStack;
	
	img2Info.fileName	  = argv[5];
	img2Info.resWidth     = atoi(argv[6]);
	img2Info.resHeight    = atoi(argv[7]);
	img2Info.initStack    = atoi(argv[8]);
	img2Info.endStack     = atoi(argv[9]);
	img2Info.resDepth     = img2Info.endStack - img2Info.initStack;

	HandleData handleData;

	if(!handleData.loadData(img1Info,img2Info,opt))
		return -1;

	handleData.similarityCheck(opt.simMetric, opt.gpuOptimized); // SimMetric 0= PSNR, 1= SSIM, 2= 2STEP || GPUOptimized 0:off 1:on

	printf("Hard work done! ;)\n"); 

    return 0;
}
