#ifndef LOG_DATA
#define LOG_DATA

#include <cstdio>
#include <handle3ddataset.h>
#include "utils.h"

class LogData
{
public:
	LogData();
	~LogData();
	bool createFile(char *l_fileName);
	bool saveDataIntoFile(char *l_fileName, vector<twoInts> l_similarityResults);
	bool closeFile(char *l_fileName);
	void handleLog(char *l_fileName, vector<twoInts> l_similarityResults, DATAINFO *img1, DATAINFO *img2);
private:
	char *fileName;
	FILE *logFile;
};


#endif //LOG_DATA