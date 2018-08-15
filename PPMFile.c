#include "PPMFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct{
	unsigned char r, g, b;
}rgbColor;		//Pixel

struct strPPMFile {
	char * name;		//Arreglo dinámico de caracteres para el nombre del archivo original: name.
	int nameLength;		//Entero que almacene la longitud del nombre: nameLength.
	int width;		//Entero que almacene la anchura de la imagen original: width.
	int height;		//Entero que almacene la altura de la imagen original: height.
	int max;		//Entero que almacene el máximo de la imagen original: max.
	rgbColor ** image;		//Matriz dinámica de pixeles*: image.
};

rgbColor ** createImage(int width, int height){
	rgbColor ** image = (rgbColor **) malloc (width * sizeof(rgbColor*));
	for (int i = 0; i < width; i++)
		image[i] = (rgbColor*) malloc (height * sizeof(rgbColor));
	return image;
}

void freeImage(rgbColor ** image, int width){
	for (int i = 0; i < width; i++)
		free (image[i]);
	free (image);
}

void saveImage(char * name, int width, int height, int max, rgbColor ** image){
	FILE* f = fopen(name, "w");

	fprintf(f, "P3\n");
	fprintf(f, "%d %d\n%d\n", width, height, max);

	int r,g,b;
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			r = image[i][j].r;
			g = image[i][j].g;
			b = image[i][j].b;
			fprintf(f, "%d %d %d\n", r, g, b);
		}

	fclose(f);
}

PPMFile ppmFile_create(char * name){

	PPMFile pf = (PPMFile) malloc (sizeof (struct strPPMFile) );
	if (pf==NULL) return NULL;

	FILE* f = fopen(name, "r");
	if (f==NULL)
	{
		free(pf); return NULL;
	}

	pf->nameLength = strlen(name);	//calcula length del nombre del archivo
	pf->name = (char*) malloc (pf->nameLength+1);	//reserva la memoria para name
	strcpy(pf->name, name);

	fgetc(f); fgetc(f); fgetc(f);	//se traga P3\n
	fscanf(f, "%d %d\n%d\n", &pf->width, &pf->height, &pf->max);

	pf->image = createImage(pf->width, pf->height);
	int r,g,b;
	for (int i = 0; i < pf->width; i++)
		for (int j = 0; j < pf->height; j++)
		{
			fscanf(f, "%d %d %d\n", &r, &g, &b);
			pf->image[i][j].r = r;
			pf->image[i][j].g = g;
			pf->image[i][j].b = b;
		}

	fclose(f);
	return pf;	//regresa el apuntador a la instancia del TDA
}

void ppmFile_destroy(PPMFile pf){
	freeImage(pf->image, pf->width);
	free (pf->name);
	free (pf);
}

void ppmFile_blackAndWhite(PPMFile pf){
	char * newName = (char*) malloc (pf->nameLength + 4);	//reserva la memoria para name
	strcpy(newName,"bw_");
	strcat(newName, pf->name);

	rgbColor ** newImage = createImage(pf->width, pf->height);
	for (int i=0; i<pf->width;i++)
		for (int j=0; j<pf->height;j++)
		{
			unsigned char bw = (int) round(((pf->image[i][j].r+pf->image[i][j].g+pf->image[i][j].b)/3.0)/255);	//el promedio de los 3 comp convertido a 0 o 1.
			newImage[i][j].r = bw;
			newImage[i][j].g = bw;
			newImage[i][j].b = bw;
		}

	saveImage(newName, pf->width, pf->height, 1, newImage);
	freeImage (newImage, pf->width);
	free (newName);
}

void ppmFile_grayscale(PPMFile pf){
	char * newName = (char*) malloc (pf->nameLength + 4);	//reserva la memoria para name
	strcpy(newName,"gs_");
	strcat(newName, pf->name);

	rgbColor ** newImage = createImage(pf->width, pf->height);
	for (int i=0; i<pf->width;i++)
		for (int j=0; j<pf->height;j++)
		{
			unsigned char gs = (int) round((pf->image[i][j].r+pf->image[i][j].g+pf->image[i][j].b)/3.0);	//el promedio de las 3 comp
			newImage[i][j].r = gs;
			newImage[i][j].g = gs;
			newImage[i][j].b = gs;
		}

	saveImage(newName, pf->width, pf->height, 255, newImage);
	freeImage (newImage, pf->width);
	free (newName);
}

void ppmFile_compress(PPMFile pf){
	char * newName = (char*) malloc (pf->nameLength + 5);	//reserva la memoria para name
	strcpy(newName,"cmp_");
	strcat(newName, pf->name);

	rgbColor ** newImage = createImage(pf->width, pf->height);
	unsigned char R, G, B;
	for (int i=0; i<pf->width;i++)
		for (int j=0; j<pf->height;j++)
		{
			R = (int) round(pf->image[i][j].r/255.0);	//cada componente convertirlo a 0 o 1
			G = (int) round(pf->image[i][j].g/255.0);	//simplemente: esta o no esta.
			B = (int) round(pf->image[i][j].b/255.0);
			newImage[i][j].r = R;
			newImage[i][j].g = G;
			newImage[i][j].b = B;
		}

	saveImage(newName, pf->width, pf->height, 1, newImage);
	freeImage (newImage, pf->width);
	free (newName);
}
void ppmFile_dithering(PPMFile pf){
	char * newName = (char*) malloc (pf->nameLength + 5);	//reserva la memoria para name
	strcpy(newName,"dth_");
	strcat(newName, pf->name);

	rgbColor ** newImage = createImage(pf->width, pf->height);
	for (int i=0; i<pf->width-1;i+=2)
		for (int j=0; j<pf->height-1;j+=2)	//recorrer imagen en grupos de 4 pixeles
		{
			float pR = (float) ((pf->image[i][j].r + pf->image[i][j+1].r + pf->image[i+1][j].r + pf->image[i+1][j+1].r)/4.0)/255.0;	//Por cada grupo de 4 pix, calcular el promedio de cada color
			float pG = (float) ((pf->image[i][j].g + pf->image[i][j+1].g + pf->image[i+1][j].g + pf->image[i+1][j+1].g)/4.0)/255.0;	// con eso se deciden los 4 pixeles de la imagen nueva,
			float pB = (float) ((pf->image[i][j].b + pf->image[i][j+1].b + pf->image[i+1][j].b + pf->image[i+1][j+1].b)/4.0)/255.0;	// prendiendo los foquitos conforme a sus "máscaras"

			newImage[i][j].r = 0;	//RED empieza con todos los foquitos apagados y los vas prendiendo
			newImage[i][j+1].r = 0;
			newImage[i+1][j].r = 0;
			newImage[i+1][j+1].r = 0;

			if (pR > .125) newImage[i][j].r = 1;	//la máscara correspondiente es la de 25%  --prendes un foquito
			if (pR > .375) newImage[i+1][j+1].r = 1;	//la máscara correspondiente es la de 50% --prendes un segundo foquito
			if (pR > .625) newImage[i][j+1].r = 1;	//la máscara correspondiente es la de 75% --prendes un tercer foquito
			if (pR > .875)	newImage[i+1][j].r = 1;	//la máscara correspondiente es la de 100% --prendes todos los foquitos

			newImage[i][j].g = 0;	//GREEN empieza con todos los foquitos apagados y los vas prendiendo
			newImage[i][j+1].g = 0;
			newImage[i+1][j].g = 0;
			newImage[i+1][j+1].g = 0;

			if (pG > .125) newImage[i][j].g = 1;	//la máscara correspondiente es la de 25%  --prendes un foquito
			if (pG > .375) newImage[i+1][j+1].g = 1;	//la máscara correspondiente es la de 50% --prendes un segundo foquito
			if (pG > .625) newImage[i][j+1].g = 1;	//la máscara correspondiente es la de 75% --prendes un tercer foquito
			if (pG > .875)	newImage[i+1][j].g = 1;	//la máscara correspondiente es la de 100% --prendes todos los foquitos

			newImage[i][j].b = 0;	//BLUE empieza con todos los foquitos apagados y los vas prendiendo
			newImage[i][j+1].b = 0;
			newImage[i+1][j].b = 0;
			newImage[i+1][j+1].b = 0;

			if (pB > .125) newImage[i][j].b = 1;	//la máscara correspondiente es la de 25%  --prendes un foquito
			if (pB > .375) newImage[i+1][j+1].b = 1;	//la máscara correspondiente es la de 50% --prendes un segundo foquito
			if (pB > .625) newImage[i][j+1].b = 1;	//la máscara correspondiente es la de 75% --prendes un tercer foquito
			if (pB > .875)	newImage[i+1][j].b = 1;	//la máscara correspondiente es la de 100% --prendes todos los foquitos
		}

	saveImage(newName, pf->width, pf->height, 1, newImage);
	freeImage (newImage, pf->width);
	free (newName);
}


