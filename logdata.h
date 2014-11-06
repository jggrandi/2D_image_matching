#ifndef LOG_DATA
#define LOG_DATA

#include <cstdio>
//#include <handle3ddataset.h>
#include "utils.h"

class LogData
{
public:
	LogData();
	~LogData();
	void handleLog(char *l_fileName, vector<twoInts> l_similarityResults, double l_runtime);
	bool createFile(char *l_fileName);
	bool saveDataIntoFile(char *l_fileName, vector<twoInts> l_similarityResults, double l_runtime);
	bool closeFile(char *l_fileName);
private:
	char *fileName;
	FILE *logFile;
};


#endif //LOG_DATA