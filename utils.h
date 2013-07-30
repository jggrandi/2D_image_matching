#ifndef UTILS
#define UTILS

#include <vector>

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

