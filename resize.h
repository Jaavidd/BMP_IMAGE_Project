#ifndef resize
#define resize
#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#define ATTEMPT_LIMIT2 10

float array[11];


typedef struct Pixels  {  //pixels structure rgba
   unsigned char red ; 
   unsigned char green ; 
   unsigned char blue ; 
   unsigned char alfa;
} Pixels;



typedef struct FILE_INFORMATION  // Important data from a bmp image.
{
	unsigned int size, width, height, offset,dib; // offset is the byte from which the pixels start. dib contains the bmp subformat.
	unsigned short first_header, nColorPlanes;
  unsigned short nBitsPerPixel;
  char  *unnecessary; // unnecessary  is the array containing remainig bytes of DIB till the offset is reached.
  unsigned char *pixels,*header,unnecessary1;
} FILE_INFORMATION;




struct FILE_INFORMATION* read_from_file(FILE* fSource,struct FILE_INFORMATION* bmp); // This function reads a binary stream ("rb"), stores it as a BMP structure and returns its pointer.

void resize_strip(FILE* fTarget,FILE *fSource, struct FILE_INFORMATION* bmp);//resize fSource and write it to new file fTarger,with name xxxResized.bmp

void WritePixels(struct FILE_INFORMATION* bmp,FILE* fSource,struct Pixels* NewPixels ); //write new pixels to new image

float MaximumPSF();  // to find maximum psf value in array

int readPsfArray(char *fileName); // to read psf values from input file
#endif
