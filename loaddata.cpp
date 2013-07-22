#include "loaddata.h"

LoadData::LoadData(){}

LoadData::~LoadData()
{
	//free (datasetRaw);
}

bool LoadData::loadFile(ImageInfo l_img)
{
	imgInfo = l_img;

	// allocate memory for the 3d dataset
	datasetRaw = (unsigned short**)malloc(l_img.resDepth * sizeof(unsigned short*));
	for (int i=0; i < l_img.resDepth; i++)
		datasetRaw[i] = (unsigned short*)malloc(sizeof(unsigned short) * (l_img.resHeight*l_img.resWidth));

	if(!(inFile = fopen( l_img.fileName, "rb")))
		return false;
	
	// read file into dataset matrix
	int hXw=l_img.resHeight*l_img.resWidth;
	for( int i = 0; i < l_img.resDepth; i++ )
	{
		for( int j = 0; j < hXw; j++ )
		{
			unsigned short value;
			fread( &value, 1, sizeof(unsigned short), inFile );
			datasetRaw[i][j] = value;
		}
	}
	fclose(inFile);
	return true;
}

unsigned short** LoadData::getDataset()
{
	return datasetRaw;
}

ImageInfo LoadData::getImageInfo()
{
	return imgInfo;
}