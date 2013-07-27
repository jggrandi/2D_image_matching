#include "logdata.h"

LogData::LogData(){}

LogData::~LogData(){}

void LogData::handleLog(char *l_fileName, vector<twoInts> l_similarityResults, double l_runtime)
{
	fileName = l_fileName;
	printf("%s!\n",fileName);
	if(!createFile(fileName)) printf("File not created.\n");
	if(!saveDataIntoFile(fileName, l_similarityResults, l_runtime)) printf("File not salved\n");
	if(!closeFile(fileName)) printf("File not closed\n");
}

bool LogData::createFile(char *l_fileName)
{
	if(!(logFile = fopen(fileName, "w")))
		return false;
	return true;
}

bool LogData::saveDataIntoFile(char *l_fileName, vector<twoInts> l_similarityResults, double l_runtime)
{
	for(unsigned int i=0; i<l_similarityResults.size(); i++)
	{
		fprintf(logFile, "%d;%d;%f\n",l_similarityResults[i].sliceNumber,l_similarityResults[i].distanceToOptimal,l_similarityResults[i].value);
	}
	fprintf(logFile, "Runtime;%f\n",l_runtime );
	return true;
}

bool LogData::closeFile(char *l_fileName)
{
	fclose(logFile);
	return true;
}
