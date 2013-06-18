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

#define waitKey(){printf("Press a key and then press ENTER...\n"); int x; scanf("%d",x);}
#define printVar(e){cerr<<#e<<"="<<(e)<<endl;}
#define printUsage(){printf("Program usage:\n	./imageMatch [imagedata1] [width] [height] [stack init] [stack end] [imagedata2] [widht] [height] [stack init] [stack end]\n");}	


#endif // SYSTEM_MESSAGES