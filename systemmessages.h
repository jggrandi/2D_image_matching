/* Handle messages and stuff
	by: Jeronimo G. Grandi
	Jun,2013
*/  

#ifndef SYSTEM_MESSAGES
#define SYSTEM_MESSAGES

#include <fstream>
#include <cstdlib>
#include <iostream>

using namespace std;

static char usage[] =
"\n"
"  (C) 2013 Image Matching\n"
"\n"
"  usage: ./imageMatch [options] <imagedata1> <width> <height> <stack init> <stack end> <imagedata2> <widht> <height> <stack init> <stack end>\n"
"\n"
"  Common options:\n"
"\n"
"      -l <file>	: log data to a file\n"
"      -v 	: verbose mode\n"
"\n";

#define waitKey(){printf("Press a key and then press ENTER...\n"); int x; scanf("%d",x);}
#define printVar(e){cerr<<#e<<"="<<(e)<<endl;}	





#endif // SYSTEM_MESSAGES