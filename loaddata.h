/* Class for datasets loading and manipulation
	by: Jeronimo G. Grandi
	May,2013
*/  

#ifndef LOAD_DATA
#define LOAD_DATA

#include <fstream>
#include <cstdlib>
#include "imageinfo.h"


class LoadData
{
public:
	LoadData();
	//LoadData(ImageInfo img);
	~LoadData();
	bool loadFile(ImageInfo img);
	unsigned short** getDataset();
	ImageInfo getImageInfo();
private:
	ImageInfo *img;
	unsigned short** datasetRaw;

};


#endif // LOAD_DATA

