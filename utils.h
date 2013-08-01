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

#define FILENAME ("../datasets_for_tests/Engine256.raw")
#define XMAX (256)
#define YMAX (256)
#define ZMAX (256)

#define XSIZE 1.0  
#define YSIZE 1.0
#define ZSIZE 1.0

#define CLAMP(a,b,c) { b = ((b) < (a))? (a) : (((b) > (c))? (c): (b));}

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
"  (C) 2013 Image Matching\n"
"\n"
"  usage: ./imageMatch [options] <imagedata1> <width> <height> <stack init> <stack end> <imagedata2> <widht> <height> <stack init> <stack end>\n"
"\n"
"  Common options:\n"
"\n"
"      -l <file>    : log data to a file\n"
"      -v   : verbose mode\n"
"\n";


#endif // UTILS

