#include"resize.h"


struct FILE_INFORMATION  *read_from_file(FILE* fSource,struct FILE_INFORMATION* bmp)
{
	
	for(int counter=0; counter<=ATTEMPT_LIMIT2 && (bmp=(struct FILE_INFORMATION*)malloc(sizeof(struct FILE_INFORMATION)))==NULL; counter++) // At code blocks like this a memory allocation attempt is repeated until the ATTEMPT_LIMIT is reached or some memory is finally allocated.
	{
		if(counter==ATTEMPT_LIMIT2)
		{
			printf("Failed to load the image into main memory.\nMake sure there is some available space in the main memory.\n");
			return NULL; // In case of an allocation failure NULL pointer is returned to signal an error.
		}
	}

	
	fread(&bmp->first_header,sizeof(unsigned short),1,fSource);
	
	fread(&bmp->size,sizeof(unsigned int),1,fSource);
  
	fread(&bmp->unnecessary1, sizeof(char), 4, fSource);
	fread(&bmp->offset,sizeof(unsigned int),1,fSource);
	fread(&(bmp->dib), sizeof(unsigned int), 1, fSource);
	fread(&bmp->width,sizeof(unsigned int),1,fSource);
	fread(&bmp->height,sizeof(unsigned int),1,fSource);
	
	fseek(fSource,28,SEEK_SET);
	fread(&bmp->nBitsPerPixel,sizeof(unsigned short),1,fSource);

  for(int counter=0; counter<=ATTEMPT_LIMIT2 && (bmp->unnecessary=(char*)malloc(sizeof(char)*(bmp->offset-30)))==NULL; counter++)
	{
		if(counter==ATTEMPT_LIMIT2)
		{
			printf("Failed to load the image into main memory.\nMake sure the file is not damaged and there is some available space in the main memory.\n");
			free(bmp); // In case of an allocation failure all the previously dynamically allocated memory for a BMP structure is freed.
                	return NULL;
		}
	}

return bmp;
}

//function to resize strip
void resize_strip(FILE* fTarget,FILE *fSource, struct FILE_INFORMATION* bmp)
{
  

	bmp = read_from_file(fSource,bmp); //call read function for take information from SourceFile

	float max =MaximumPSF(); // we call function for maximum psf value
	
    bmp->width = max *  bmp->width; //set new width 
    bmp->height = (-1)*bmp->height;

   
	
	int NewSize= (bmp->width)*(bmp->height)*(bmp->nBitsPerPixel/8)+bmp->offset;
	
	
    
	fwrite((&bmp->first_header),sizeof(unsigned short),1,fTarget);
	fwrite((&NewSize),sizeof(unsigned int),1,fTarget);	
	fwrite((&bmp->unnecessary1), sizeof(char), 4, fTarget);
	fwrite((&bmp->offset),sizeof(unsigned int),1,fTarget);
	fwrite((&bmp->dib), sizeof(unsigned int), 1, fTarget);
	fwrite((&bmp->width),sizeof(unsigned int),1,fTarget);

	fseek(fSource,22,SEEK_SET);   

    if((bmp->header = (unsigned char*)malloc(bmp->offset*sizeof(unsigned char)))==NULL)//bmp->offset = 54
	{
		printf("problem with allocation\n");
		exit(-1);
	}

	fread(bmp->header,32,1,fSource);
	fwrite(bmp->header,32,1,fTarget);  	

	free(bmp->header);
    
    int NewImageSize = bmp->height*bmp->width*4;
	int NewPixelSize = NewImageSize/(bmp->nBitsPerPixel/8);
	Pixels *NewPixels;
	if((NewPixels = (struct Pixels*)malloc(NewPixelSize*sizeof(struct Pixels)))==NULL)
	{
		printf("error"); 
		exit(-1);
	}

	WritePixels(bmp,fSource,NewPixels); //call function to write pixels
	
	fwrite(NewPixels,NewImageSize,1,fTarget);  //last step two write pixels

 
}

// function two write pixels to new image
 void WritePixels(struct FILE_INFORMATION* bmp,FILE* fSource,struct Pixels* NewPixels )
  {
    int OldSize = bmp->size; 
    int OldImageSize = OldSize-bmp->offset; 

	

	if((bmp->pixels = (unsigned char*)malloc((OldImageSize)*sizeof(unsigned char)))==NULL)
	{
		printf("problem with allocation\n");
		exit(-1);
	}
	
	fseek(fSource,54,SEEK_SET);

    int BytesForPixels = bmp->nBitsPerPixel/8;
	int OldPixelSize = OldImageSize/BytesForPixels;	
	Pixels* OldPixels;

		if((OldPixels = (struct Pixels*)malloc(OldPixelSize*sizeof(struct Pixels)))==NULL)
     	{
		printf("error"); 
		exit(-1);
    	}

	fread(OldPixels,OldImageSize,1,fSource); //reading pixels from old image

	unsigned char * newPixels;

	if((newPixels=(unsigned char*)malloc(bmp->height*bmp->width*4*sizeof(unsigned char)))==NULL)
	{
		printf("problem with malloc");
		exit(-1);
	}

	int NewImageSize = bmp->height*bmp->width*4;
	int NewPixelSize = NewImageSize/BytesForPixels;
	

	//changing pixels
	int index = 0; 	 int NewIndex=0;	int OldIndex=0;
	
	float psfValue = array[index++];	 float CurrentPSF = psfValue; int CurrentWidth = psfValue * 480;  
	int offset,LastPixel=0;
	
	int PixelNumber = 0; // for elongate pixels 200 pixels each time
	int StartPoint = PixelNumber*bmp->width + (bmp->width-CurrentWidth)/2;
	PixelNumber=PixelNumber+200;
	int NumberOfRepetitions = bmp->height/PixelNumber ; //how many NumberOfRepetitionss we will go 

	for(int OnePixel = StartPoint; OnePixel < NewPixelSize;OnePixel++){
		
		if(OnePixel == StartPoint + CurrentWidth){
			OnePixel = StartPoint + bmp->width;
			StartPoint = OnePixel;
		}
		if(LastPixel == PixelNumber*480 ){
			NewIndex = LastPixel; NumberOfRepetitions--;
			if(NewIndex != OldIndex){
				if(NumberOfRepetitions == 0){
					index--; PixelNumber--;
				}
				psfValue=array[index++]; CurrentPSF=psfValue;  CurrentWidth = psfValue*480;
				
				OldIndex = NewIndex; offset = bmp->width - CurrentWidth;
				
				OnePixel = PixelNumber*bmp->width + offset/2;
				
				StartPoint = OnePixel; PixelNumber += 200;
			}
		}	

		 if(CurrentPSF < 1){
			NewPixels[OnePixel].red = (1-CurrentPSF)*OldPixels[LastPixel+1].red  +  CurrentPSF*OldPixels[LastPixel].red ;
			NewPixels[OnePixel].green =(1-CurrentPSF)*OldPixels[LastPixel+1].green  +  CurrentPSF*OldPixels[LastPixel].green ;
			NewPixels[OnePixel].blue = (1-CurrentPSF)*OldPixels[LastPixel+1].blue  +  CurrentPSF*OldPixels[LastPixel].blue ;
			NewPixels[OnePixel].alfa = (1-CurrentPSF)*OldPixels[LastPixel+1].alfa  +  CurrentPSF*OldPixels[LastPixel].alfa;
			
			CurrentPSF = CurrentPSF -1;  CurrentPSF=CurrentPSF + psfValue;  LastPixel=LastPixel+1;
		}
		else{
			NewPixels[OnePixel].red = OldPixels[LastPixel].red;   NewPixels[OnePixel].green = OldPixels[LastPixel].green;
			NewPixels[OnePixel].blue = OldPixels[LastPixel].blue;  NewPixels[OnePixel].alfa = OldPixels[LastPixel].alfa;
			
			CurrentPSF--;
		}
		
		
	}
  }

// Function to find maximum psf

float MaximumPSF()
{
  float max=array[0];
  for(int i=1;i<11;i++)
  {
    if(max<array[i]) max=array[i];
  }

  return (float)max;
}


//read from file and write psf values to an array	
int  readPsfArray(char *fileName){
  int i=0;
  FILE *pif;
  pif=fopen(fileName,"r");
  char str[100], *pIn;
  do{
    fscanf(pif,"%s",str);
    pIn=strtok(str,",");
    while(pIn != NULL){
      array[i]=atof(pIn);
	  i++;
      pIn =strtok(NULL,","); //to a new data
    }
  } while (fgetc(pif)!=EOF);
 if(i==0)
 {
	  printf("empty file\n");
	  return -1; //if given file is empty
 }
 return 1;

}