#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include"resize.h"

int main(int argc, char const *argv[])
{


    FILE_INFORMATION *bmp;
    
	int NumberOfImages= 0;

    for(int i = 1; i < argc ; i++){
        
        NumberOfImages++;
    }



    FILE *file = fopen(argv[1],"r");



    bmp = read_from_file(file,bmp);

    FILE *new = fopen("glue.bmp","wb+"); 

	bmp->height = (-1)*bmp->height;

	bmp->header = (unsigned char*)malloc(bmp->offset*sizeof(unsigned char));
	int width = NumberOfImages*bmp->width;
    bmp->size = 4*(width*bmp->height)+bmp->offset; 
	bmp->size = bmp->size - bmp->offset; 


	fseek(file,0,SEEK_SET);
	fwrite(&bmp->first_header,2,1,new);
	fwrite(&bmp->size,4,1,new);		
	fseek(file,6,SEEK_SET);
	fread(bmp->header,12,1,file);
	fwrite(bmp->header,12,1,new);		
	fseek(file,18,SEEK_SET);
	
	fwrite(&width,4,1,new);
	fseek(file,22,SEEK_SET);

	fread(bmp->header,bmp->offset - 22,1,file);
	fwrite(bmp->header,bmp->offset - 22,1,new);	
	free(bmp->header);


    fclose(file);


    unsigned char *arr[NumberOfImages];
	for (int i=0; i<NumberOfImages; i++) 
         arr[i] = (unsigned char *)malloc(bmp->size * sizeof(unsigned char)); 

	
	bmp->pixels = (unsigned char *)malloc(4*(width*bmp->height)*sizeof(unsigned char));
	


    for(int i = 1 ; i <= NumberOfImages; i++){
        file = fopen(argv[i],"rb");
        fseek(file,bmp->offset,SEEK_SET);
        fread(arr[i-1],bmp->size,1,file);
        fclose(file);
    }


	

	// printf("%d--- %d----- %d\n",bmp->height,bmp->width,width);

	for(int i = 0 ; i < bmp->height ; i++){
		for(int j = 0;j < NumberOfImages; j++){
			fwrite(&arr[j][i*bmp->width*4],bmp->width*4,1,new);
		}
	}

    return 0;
}
