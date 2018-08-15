#ifndef PPMFILE_H_
#define PPMFILE_H_

typedef struct strPPMFile * PPMFile;

PPMFile ppmFile_create(char * name);//reservar memoria para instancia del PPMFile, poblar la instancia con el contenido del file
void ppmFile_destroy(PPMFile pf);//liberar la memoria asociada a la instancia del PPMFile cuya dirección recibe
void ppmFile_blackAndWhite(PPMFile pf);
void ppmFile_grayscale(PPMFile pf);
void ppmFile_compress(PPMFile pf);
void ppmFile_dithering(PPMFile pf);

#endif /* PPMFILE_H_ */
