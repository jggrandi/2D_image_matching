#include <loaddata.h>

LoadData::LoadData(){}

LoadData::LoadData(ImageInfo img1, ImageInfo img2)
{
	if(!loadFile(img1)) printf("Fail to load %s",img1.fileName);
	if(!loadFile(img2)) printf("Fail to load %s",img2.fileName);
}

bool LoadData::loadFile(ImageInfo img)
{

	FILE* inFile;

	// allocate memory for the 3d dataset
	datasetRaw = (unsigned short**)malloc(img.resDepth * sizeof(unsigned short*));
	for (int i=0; i < img.resDepth; i++)
		datasetRaw[i] = (unsigned short*)malloc(sizeof(unsigned short) * (img.resHeight*img.resWidht));

	if( inFile = fopen( img.fileName, "rb" ) )
	{
		// read file into dataset matrix
		int hXw=img.resHeight*img.resWidth;
		for( int i = 0; i < img.resDepth; i++ )
		{
			for( int j = 0; j < rrr; j++ )
			{
					unsigned short value;
					fread( &value, 1, sizeof(unsigned short), inFile );
					datasetRaw[i][j] = value;
			}
		}
		fclose(inFile);
		return 0;
	}
	else
	{
		return 1;
	}

}

