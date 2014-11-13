#ifndef UTILS
#define UTILS

#include <opencv2/gpu/gpu.hpp> 
#include <vector>


using namespace cv;

typedef unsigned short imgT;

typedef struct options
{
	bool verbose;
	bool logdata;
    bool gpuOptimized;
    int  simMetric;
	char *logfilename;
}OPT;


struct sliceRank
{
    int sliceNumber;
    float value;
};

struct twoInts
{
    int sliceNumber;
    int distanceToOptimal;
    float value;
};


static char usage[] =
"\n"
"  (C) 2013 Image Matching\n"
"\n"
"  usage: ./imageMatch [options] <imagedata1> <width> <height> <stack init> <stack end> <imagedata2> <widht> <height> <stack init> <stack end>\n"
"\n"
"  Common options:\n"
"\n"
"      -m <0,1,2>   : 0=PSNR, 1=SSIM, 2=2STEP \n"
"      -l <file>    : log data to a file\n"
"      -v   : verbose mode\n"
"\n";


#endif // UTILS

