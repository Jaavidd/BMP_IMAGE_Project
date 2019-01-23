#include "extractStrip.h"

// This function frees the space allocated for dib bytes till the offset and the memory allocated for the BMP structure itself.
void BMP_clearStruct(struct BMP* bmp)
{
	free(bmp->unnecessary2);
	free(bmp);
}

// This function frees the space allocated for lines/rows of pixel table of a BMP structure till a given line.
void BMP_clearMatrixUntilLine(struct BMP* bmp, unsigned int line)
{
	for(unsigned int i=0; i<line; i++)
	{
		for(unsigned int j=0; j< bmp->width; j++)
			free(bmp->image[i][j].bytes);
		free(bmp->image[i]);
	}
	free(bmp->image);
}

// This function frees the space allocated for pixels of a given line/row of a BMP structure pixel table till a given pixel.
void BMP_clearLineUntilPixel(struct BMP* bmp, unsigned int line, unsigned int pixel)
{
	for(unsigned int j=0; j<pixel; j++)
		free(bmp->image[line][j].bytes);
	free(bmp->image[line]);
}

void BMP_delete(struct BMP* bmp)
{
	BMP_clearMatrixUntilLine(bmp, bmp->height); // All of the space allocated for a BMP structure pixel table is freed.
	BMP_clearStruct(bmp); // The space allocated for the BMP structure is freed.
}

struct BMP* BMP_read(FILE* file)
{
	struct BMP* bmp;
	for(int counter=0; counter<=ATTEMPT_LIMIT && (bmp=(struct BMP*)malloc(sizeof(struct BMP)))==NULL; counter++) // At code blocks like this a memory allocation attempt is repeated until the ATTEMPT_LIMIT is reached or some memory is finally allocated.
	{
		if(counter==ATTEMPT_LIMIT)
		{
			printf("Failed to load the image into main memory.\nMake sure there is some available space in the main memory.\n");
			return NULL; // In case of an allocation failure NULL pointer is returned to signal an error.
		}
	}
	fread(&(bmp->header), sizeof(short unsigned int), 1, file);
	fread(&(bmp->size), sizeof(unsigned int), 1, file);
	fread(bmp->unnecessary1, sizeof(char), 4, file);
	fread(&(bmp->offset), sizeof(unsigned int), 1, file);
	fread(&(bmp->dib), sizeof(unsigned int), 1, file);
	fread(&(bmp->width), sizeof(unsigned int), 1, file);
	fread(&(bmp->height), sizeof(unsigned int), 1, file);
	fread(&(bmp->nColorPlanes), sizeof(short unsigned int), 1, file);
	fread(&(bmp->nBitsPerPixel), sizeof(short unsigned int), 1, file);
	for(int counter=0; counter<=ATTEMPT_LIMIT && (bmp->unnecessary2=(char*)malloc(sizeof(char)*(bmp->offset-30)))==NULL; counter++)
	{
		if(counter==ATTEMPT_LIMIT)
		{
			printf("Failed to load the image into main memory.\nMake sure the file is not damaged and there is some available space in the main memory.\n");
			free(bmp); // In case of an allocation failure all the previously dynamically allocated memory for a BMP structure is freed.
                	return NULL;
		}
	}
	fread(bmp->unnecessary2, sizeof(char), (bmp->offset-30), file); // Remaining bytes of dib are read until the pixel table is reached.
	for(int counter=0; counter<=ATTEMPT_LIMIT && (bmp->image=(struct Pixel**)malloc(sizeof(struct Pixel*)*bmp->height))==NULL; counter++)
	{
		if(counter==ATTEMPT_LIMIT)
		{
			printf("Failed to load the image into main memory.\nMake sure there is some available space in the main memory.\n");
			BMP_clearStruct(bmp);
                	return NULL;
		}
	}
	for(unsigned int i=0; i< bmp->height; i++)
	{
		for(int counter=0; counter<=ATTEMPT_LIMIT && (bmp->image[i]=(struct Pixel*)malloc(sizeof(struct Pixel)*bmp->width))==NULL; counter++)
		{
			if(counter==ATTEMPT_LIMIT)
			{
				printf("Failed to load the image into main memory.\nMake sure there is some available space in the main memory.\n");
				BMP_clearMatrixUntilLine(bmp, i);
				BMP_clearStruct(bmp);
                		return NULL;
			}
		}
		for(unsigned int j=0; j< bmp->width; j++)
		{
			for(int counter=0; counter<=ATTEMPT_LIMIT && ((bmp->image[i][j]).bytes=(char*)malloc(sizeof(char)*(bmp->nBitsPerPixel/8)))==NULL; counter++)
			{
				if(counter==ATTEMPT_LIMIT)
				{
					printf("Failed to load the image into main memory.\nMake sure there is some available space in the main memory.\n");
					BMP_clearLineUntilPixel(bmp, i, j);
					BMP_clearMatrixUntilLine(bmp, i);
					BMP_clearStruct(bmp);
					return NULL;
				}
			}
			fread((bmp->image[i][j]).bytes, sizeof(char), bmp->nBitsPerPixel/8, file); // For each pixel, nBitsPerPixel divided by 8 bytes are read.
		}
		fseek(file, (4-(((bmp->width)*(bmp->nBitsPerPixel/8))%4))%4, SEEK_CUR); // The padding is skipped.
	}
	return bmp; // Pointer to a resulting BMP structure is returned.
}

void BMP_write(struct BMP* bmp, FILE* file)
{
	fwrite(&(bmp->header), sizeof(short unsigned int), 1, file);
        fwrite(&(bmp->size), sizeof(unsigned int), 1, file);
        fwrite(bmp->unnecessary1, sizeof(char), 4, file);
        fwrite(&(bmp->offset), sizeof(unsigned int), 1, file);
        fwrite(&(bmp->dib), sizeof(unsigned int), 1, file);
        fwrite(&(bmp->width), sizeof(unsigned int), 1, file);
        fwrite(&(bmp->height), sizeof(unsigned int), 1, file);
        fwrite(&(bmp->nColorPlanes), sizeof(short unsigned int), 1, file);
        fwrite(&(bmp->nBitsPerPixel), sizeof(short unsigned int), 1, file);
        fwrite(bmp->unnecessary2, sizeof(char), (bmp->offset-30), file);
        for(unsigned int i=0; i< bmp->height; i++)
        {
		for(unsigned int j=0; j< bmp->width; j++)
			fwrite((bmp->image[i][j]).bytes, sizeof(char), (bmp->nBitsPerPixel/8), file);
		char null=0;
                for(unsigned int k=0; k< (4-(((bmp->width)*(bmp->nBitsPerPixel/8))%4))%4; k++) // A row of pixels is comptleted with a null padding.
                	fwrite(&null, sizeof(char), 1, file);
	}
}

struct BMP* BMP_modify(struct BMP* bmpInput, unsigned int stripWidth)
{
	if(stripWidth> bmpInput->width || stripWidth==0) // The strip width has to be less or equal to the width of the initial image, but more than zero, because a bmp file with a zero width is considered a damaged file by some programs.
	{
		printf("Failed to modify the image.\nInvalid width.\n");
		return NULL;
	}
	unsigned int padding=(4-((stripWidth*(bmpInput->nBitsPerPixel/8))%4))%4, offsetColumn=(bmpInput->width)/2-stripWidth/2; // Size of final padding is calculated for later use. offsetColumn position from which the final image should start is calculated.
	struct BMP* bmpOutput;
	for(int counter=0; counter<=ATTEMPT_LIMIT && (bmpOutput=(struct BMP*)malloc(sizeof(struct BMP)))==NULL; counter++)
	{
		if(counter==ATTEMPT_LIMIT)
		{
			printf("Failed to modify the image.\nMake sure there is some available space in the main memory.\n");
			return NULL;
		}
	}
        bmpOutput->header=bmpInput->header;
        bmpInput->size=(bmpOutput->size)-(bmpInput->width+(4-((stripWidth*(bmpInput->nBitsPerPixel/8))%4)%4)-stripWidth)*(bmpInput->height)*(bmpInput->nBitsPerPixel)+padding*(bmpInput->height); // Final size is calculated and final padding size is also taken into consideration.
	for(int i=0; i<4; i++)
		bmpOutput->unnecessary1[i]=bmpInput->unnecessary1[i];
        bmpOutput->offset=bmpInput->offset;
        bmpOutput->dib=bmpInput->dib;
        bmpOutput->width=stripWidth;
        bmpOutput->height=bmpInput->height;
        bmpOutput->nColorPlanes=bmpInput->nColorPlanes;
	bmpOutput->nBitsPerPixel=bmpInput->nBitsPerPixel;
	for(int counter=0; counter<=ATTEMPT_LIMIT && (bmpOutput->unnecessary2=(char*)malloc(sizeof(char)*(bmpOutput->offset-30)))==NULL; counter++)
	{
		if(counter==ATTEMPT_LIMIT)
		{
			printf("Failed to modify the image.\nMake sure there is some available space in the main memory.\n");
			free(bmpOutput);
			return NULL;
		}
	}
        for(int i=0; i< (bmpOutput->offset-30); i++)
                bmpOutput->unnecessary2[i]=bmpInput->unnecessary2[i];
	for(int counter=0; counter<=ATTEMPT_LIMIT && (bmpOutput->image=(struct Pixel**)malloc(sizeof(struct Pixel*)*(bmpOutput->height)))==NULL; counter++)
        {
		if(counter==ATTEMPT_LIMIT)
		{
			printf("Failed to modify the image.\nMake sure there is some available space in the main memory.\n");
			BMP_clearStruct(bmpOutput);
			return NULL;
		}
	}
        for(unsigned int i=0; i< bmpOutput->height; i++)
        {
		for(int counter=0; counter<=ATTEMPT_LIMIT && (bmpOutput->image[i]=(struct Pixel*)malloc(sizeof(struct Pixel)*stripWidth))==NULL; counter++)
		{
			if(counter==ATTEMPT_LIMIT)
			{
				printf("Failed to modify the image.\nMake sure there is some available space in the main memory.\n");
				BMP_clearMatrixUntilLine(bmpOutput, i);
				BMP_clearStruct(bmpOutput);
				return NULL;
			}
		}
                for(unsigned int j=0; j< stripWidth; j++)
                {
			for(int counter=0; counter<=ATTEMPT_LIMIT && (bmpOutput->image[i][j].bytes=(char*)malloc(sizeof(char)*(bmpOutput->nBitsPerPixel/8)))==NULL; counter++)
			{
				if(counter==ATTEMPT_LIMIT)
				{
					printf("Failed to modify the image.\nMake sure there is some available space in the main memory.\n");
					BMP_clearLineUntilPixel(bmpOutput, i, j);
					BMP_clearMatrixUntilLine(bmpOutput, i);
                        		BMP_clearStruct(bmpOutput);
					return NULL;
				}
			}
                       	for(unsigned int k=0; k< (bmpOutput->nBitsPerPixel)/8; k++)
				bmpOutput->image[i][j].bytes[k]=bmpInput->image[i][offsetColumn+j].bytes[k];
                }
        }
        return bmpOutput;
}
