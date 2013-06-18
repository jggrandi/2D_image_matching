#include "loaddata.h"

LoadData::LoadData(){}

/*
LoadData::LoadData(ImageInfo img)
{
	if(!loadFile(img)) printf("Fail to load %s",img.fileName);
	else printf("%s loaded!\n",img.fileName );
}
*/
LoadData::~LoadData()
{
	free (datasetRaw);
}

bool LoadData::loadFile(ImageInfo img)
{

	FILE* inFile;

	// allocate memory for the 3d dataset
	datasetRaw = (unsigned short**)malloc(img.resDepth * sizeof(unsigned short*));
	for (int i=0; i < img.resDepth; i++)
		datasetRaw[i] = (unsigned short*)malloc(sizeof(unsigned short) * (img.resHeight*img.resWidth));

	if( inFile = fopen( img.fileName, "rb"))
	{
		// read file into dataset matrix
		int hXw=img.resHeight*img.resWidth;
		for( int i = 0; i < img.resDepth; i++ )
		{
			for( int j = 0; j < hXw; j++ )
			{
				unsigned short value;
				fread( &value, 1, sizeof(unsigned short), inFile );
				datasetRaw[i][j] = value;
			}
		}
		fclose(inFile);
		return 1;
	}
	else
	{

		return 0;
	}
}

unsigned short** LoadData::getDataset()
{
	return datasetRaw;
}

ImageInfo getImageInfo()
{
	return img;
}