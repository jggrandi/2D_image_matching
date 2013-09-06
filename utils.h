#ifndef UTILS
#define UTILS

#include <vector>


#define TF_TABLE_SIZE (256)

#ifndef M_PI
#define M_PI (3.1415)
#endif

#define EPSILON (0.000001)
#define START_EPSILON (0.01)
#define START_EPSILON1 (0.009)

#define XSIZE 1.0  
#define YSIZE 1.0
#define ZSIZE 1.0

#define CLAMP(a,b,c) { b = ((b) < (a))? (a) : (((b) > (c))? (c): (b));}


typedef struct data
{
    char *inputFileName;
    char *outputFileName;
    char viewOrientation;
    int  resWidth;
    int  resHeight;
    int  resDepth;
    int  initStack;
    int  endStack;
}DATAINFO;


typedef struct options
{
	bool verbose;
	bool logdata;
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

struct Edge {
	public:
		Edge(int i, int j) {nV1=i; nV2=j;}
		Edge() {nV1=0; nV2=0;}
		int nV1, nV2;
};



static char usage[] =
"\n"
"  (C) 2013 Volume Rendering\n"
"\n"
"  usage: ./volumeRendering <imagedata> <res width> <res height> <stack init> <stack end> \n"
"\n"
"  ex: "
"\n"
"	./volumeRendering Bucky.32x32x32.raw 32 32 0 32"
"\n";


#endif // UTILS

