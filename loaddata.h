/* Class for datasets loading and manipulation
	by: Jerônimo G. Grandi
	May,2013
*/  

#ifndef LOAD_DATA
#define LOAD_DATA

#include <vector>
#include <fstream>
#include "ImageInfo.h"


class LoadData
{
public:
	LoadData();
	LoadData(ImageInfo img1,ImageInfo img2);
	~LoadData();
	bool loadFile(ImageInfo img);
private:
	ImageInfo *img;
	unsigned short* datasetRaw;

};


#endif // LOAD_DATA

