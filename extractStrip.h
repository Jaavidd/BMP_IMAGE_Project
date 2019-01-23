#ifndef __EXTRACTSTRIP_H__
#define __EXTRACTSTRIP_H__

#include <stdio.h>
#include <stdlib.h>

#define ATTEMPT_LIMIT 10 // The limit for attempts to dynamically allocate memory.

struct Pixel
{
	char* bytes; // A pixel will contain bytes (red, green, blue, alpha etc.). Different formats have different number of bytes per pixel.
};

typedef struct BMP // Important data from a bmp image.
{
	unsigned int size, width, height, offset, dib; // offset is the byte from which the pixels start. dib contains the bmp subformat.
	short unsigned int header, nBitsPerPixel, nColorPlanes;
	struct Pixel** image; // image is the table of pixels.
	char unnecessary1[4], *unnecessary2; // unnecessary1 is the array of bytes used by other programs. unnecessary2 is the array containing remainig bytes of DIB till the offset is reached.
} *BMP;

struct BMP* BMP_read(FILE* file); // This function reads a binary stream ("rb"), stores it as a BMP structure and returns its pointer.
void BMP_write(struct BMP* bmp, FILE* file); // This function writes the data of a BMP structure into a binary stream ("wb").
struct BMP* BMP_modify(struct BMP* bmpInput, unsigned int stripWidth); // This function extracts the middle strip of size stripWidth from a BMP structure, stores it as another BMP structure and returns the pointer to the latter one.
void BMP_delete(struct BMP* bmp); // This function frees the memory dynamically allocated for a BMP structure.

#endif
