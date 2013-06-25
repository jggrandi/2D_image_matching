
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <vector>

#include "handledata.h"
#include "systemmessages.h"
#include "utils.h"


int main(int argc, char *argv[])
{
	
	int option_index = 0;
	OPT opt;

	opt.logdata = 0;
	opt.verbose = 0;

	while ((option_index = getopt(argc,argv,"l:v")) != -1)
	switch (option_index)
	{
		case 'l':
			opt.logdata = 1;
			opt.logfilename = optarg;
		break;
		case 'v':
			opt.verbose = 1;
		break;
		default:
			printf("%s",usage );
		break;
	}

	argc -= optind;
    argv += optind;
	
	if (argc != 10)
	{
		printf("%s",usage);
		return 0;
	}
	
	ImageInfo img1Info, img2Info;

	img1Info.fileName=argv[0];
	img1Info.resWidth = atoi(argv[1]);
	img1Info.resHeight = atoi(argv[2]);
	img1Info.initStack = atoi(argv[3]);
	img1Info.endStack = atoi(argv[4]);
	img1Info.resDepth = img1Info.endStack - img1Info.initStack;
	img2Info.fileName=argv[5];
	img2Info.resWidth = atoi(argv[6]);
	img2Info.resHeight = atoi(argv[7]);
	img2Info.initStack = atoi(argv[8]);
	img2Info.endStack = atoi(argv[9]);
	img2Info.resDepth = img2Info.endStack - img2Info.initStack;

	HandleData handleData(img1Info,img2Info,opt);

	handleData.similarityCheck();

	//handleData.showData();

	printf("TCHAU\n"); 

    return 0;
}
