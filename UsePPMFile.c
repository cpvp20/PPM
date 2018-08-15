#include <stdio.h>
#include <stdlib.h>
#include "PPMFile.h"

int main(){
	char fileName[400];
	scanf("%s", fileName);
	PPMFile pf = ppmFile_create(fileName);

	while(pf==NULL)
	{
		scanf("%s", fileName);
		pf = ppmFile_create(fileName);
	}

	int o;
	do
	{
		scanf("%d", &o);
		if (o==1) ppmFile_blackAndWhite(pf);
		if (o==2) ppmFile_grayscale(pf);
		if (o==3) ppmFile_compress(pf);
		if (o==4) ppmFile_dithering(pf);

	}while(o>=1 && o<=4);

	ppmFile_destroy(pf);
	return 0;
}

