#include "logdata.h"

LogData::LogData(){}

LogData::~LogData(){}

void LogData::handleLog(char *l_fileName, vector<twoInts> l_similarityResults, DATAINFO *img1, DATAINFO *img2)
{
	fileName = l_fileName;
	printf("%s!\n",fileName);
	if(!createFile(fileName)) printf("File not created.\n");
	if(!saveDataIntoFile(fileName, l_similarityResults)) printf("File not salved\n");
	if(!closeFile(fileName)) printf("File not closed\n");
}

bool LogData::createFile(char *l_fileName)
{
	if(!(logFile = fopen(fileName, "w")))
		return false;
	//fprintf(logFile, "%s,[%dx%dx%d],%d",img1->fileName );
	return true;
}

bool LogData::saveDataIntoFile(char *l_fileName, vector<twoInts> l_similarityResults)
{
	for(unsigned int i=0; i<l_similarityResults.size(); i++)
	{
		fprintf(logFile, "%d;%d\n",l_similarityResults[i].sliceNumber,l_similarityResults[i].distanceToOptimal);
	}
	return true;
}

bool LogData::closeFile(char *l_fileName)
{
	fclose(logFile);
	return true;
}
