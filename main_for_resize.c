#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "resize.h"


void displayHelp();
int isBmp(char* string);
int isTxt(char* string);
char* generateNewName(char* oldName);



int main(int argc, char** argv)  
{
		if(argc==2)
      	{
		if((argv[1][0]=='-' && argv[1][1]=='h' && argv[1][2]=='\0') || (argv[1][0]=='-' && argv[1][1]=='-' && argv[1][2]=='h' && argv[1][3]=='e' && argv[1][4]=='l' && argv[1][5]=='p' && argv[1][6]=='\0')) // If there is only one argument "-h" or "--help", the usage syntax is displayed.
		{
			displayHelp();
			return 0; // The program was succesfully executed.
		}
		  }
	
	if(argc==3)
	{ 
	 FILE *inputStream;
     FILE* outputStream;

		if(isBmp(argv[1])==-1 || isTxt(argv[2])) // Correctness of both arguments is checked.
		{
			printf("Wrong syntax.\n");
			displayHelp();
			return -1;
		}
	
        printf("Opening the file: %s\n", argv[1]);
	    inputStream=fopen(argv[1], "rb+");
		if(inputStream==NULL)
		{
			printf("Failed to open the input file.\n");
			return -1;
		}
		printf("Loading the image.\n");
	

         char* newName=generateNewName(argv[1]);  //create new name
		printf("Saving the image at %s.\n", newName);
    
	
         
               struct stat st = {0}; 
		 if (stat("./ResizedImages", &st) == -1)    //create directory for storing resizing images
		 {
             mkdir("./ResizedImages", 0700);
         }
		
		                            
		 outputStream=fopen(newName, "wb");
   
 
        	free(newName);
		if(outputStream==NULL)
		{
			
			printf("Failed to open the output file.\n");
			return -1;
		}
		
		struct FILE_INFORMATION  bmp;  //structure
		
        if(readPsfArray(argv[2])==-1) // reading psf values from input file
 		          return -1;
		
         resize_strip(outputStream,inputStream,&bmp); //function which resize
		 printf("%s has been resized.\n", argv[1]);
	     fclose(inputStream);
         fclose(outputStream);
		 
	}   

}





// This function displays the usage syntax.
void displayHelp()
{
	printf("Syntax:\n");
	printf("./resize fileName fileOfPsfValues \n");
    printf("File: Relative path to the input file that has to be an undamaged bmp file ending with .bmp or .BMP.\n");
     printf("File: Relative path to the input of PsvValues must be end with .txt or .psv\n");
	printf("./resize -h or ./resize --help to display this message again.\n");
}


// This function determines if a string corresponds to name of a file with .bmp or .BMP extension.
int isBmp(char* string)
{
	int length=strlen(string);
	if(string[length-4]=='.' && ((string[length-3]=='b' && string[length-2]=='m' && string[length-1]=='p')||(string[length-3]=='B' && string[length-2]=='M' && string[length-1]=='P')))
		return 0; // 0 for true.
	else
		return -1; // -1 for false.
}

int isTxt(char* string)
{
    int length=strlen(string);
	if(string[length-4]=='.' && ((string[length-3]=='t' && string[length-2]=='x' && string[length-1]=='t')||(string[length-3]=='p' && string[length-2]=='s' && string[length-1]=='v')))
		return 0; // 0 for true.
	else
		return -1; // -1 for false.
}

// This function dynamically allocates memory for a string containing the name of the modified file, writes the file name into it, and returns its pointer.
char* generateNewName(char* oldName)
{
	unsigned int oldLength=strlen(oldName)-4; // -4, because ".bmp" or ".BMP" is omitted.
	char* newName=(char*)calloc(oldLength+12, sizeof(char)); 
	for(int i=0; i<oldLength; i++)
		newName[i]=oldName[i];
	
    strcat(newName,"Resized.bmp");

	return newName;
}

