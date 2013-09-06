#include <cstdlib>
#include <unistd.h>
#include <vector>


#include "core.h"
#include "utils.h"


int main(int argc, char **argv)
{

	int option_index = 0;
	
	if (argc != 6)
	{
		printf("%s",usage);
		return -1;
	}
	
	Core appCore(argc, argv);
	appCore.run();
	printf("Hard work done! ;)\n"); 

    return 0;
}
