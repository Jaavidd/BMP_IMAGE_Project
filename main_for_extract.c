#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "extractStrip.h"

// This function displays the usage syntax.
void displayHelp()
{
	printf("Syntax:\n");
	printf("./extractStrip -[sWidth] [File]\n");
	printf("sWidth: A positive integer smaller than the width of the input file.\n");
	printf("File: Relative path to the input file that has to be an undamaged bmp file ending with .bmp or .BMP.\n");
	printf("./extractString -h or ./extractString --help to display this message again.\n");
}

// This function determines if a string is a positive integer.
int isNumber(char* string)
{
	int i=(string[0]=='+'?1:0);
	for(; string[i]; i++)
		if(string[i]<'0' || string[i]>'9')
			return -1; // -1 is returned if the string is not a positive integer.
	return 0; // 0 is returned if the string is a positive integer.
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

// This function converts and writes an integer into a string and then returns the length of that integer.
int writeInteger(char* string, unsigned int integer)
{
	int length;
	for(length=0; integer>0; length++, integer/=10)
		string[length]=((char)(integer%10))+48;
	for(int i=0; i<length/2; i++)
	{
		char tmp=string[i];
		string[i]=string[length-1-i];
		string[length-1-i]=tmp;
	}
	return length;
}

// This function dynamically allocates memory for a string containing the name of the modified file, writes the file name into it, and returns its pointer.
char* generateNewName(char* oldName, int sWidth)
{
	unsigned int oldLength=strlen(oldName)-4; // -4, because ".bmp" or ".BMP" is omitted.
	char* newName=(char*)calloc(oldLength+17, sizeof(char)); // +17, because maximum integer represented via 4 bytes is 10 characters long, 2 characters for "_S", 4 more characters for ".bmp", and 1 byte for '\0'.
	for(int i=0; i<oldLength; i++)
		newName[i]=oldName[i];
	newName[oldLength]='_';
	newName[oldLength+1]='S';
	int length=writeInteger(newName+oldLength+2, sWidth);
	newName[oldLength+2+length]='.';
	newName[oldLength+2+length+1]='b';
	newName[oldLength+2+length+2]='m';
	newName[oldLength+2+length+3]='p';
	return newName;
}

int main(int argc, char** argv)
{
	if(argc==2)
	{
		if((argv[1][0]=='-' && argv[1][1]=='h' && argv[1][2]=='\0') || (argv[1][0]=='-' && argv[1][1]=='-' && argv[1][2]=='h' && argv[1][3]=='e' && argv[1][4]=='l' && argv[1][5]=='p' && argv[1][6]=='\0')) // If there is only one argument "-h" or "--help", the usage syntax is displayed.
		{
			displayHelp();
			return 0; // The program was succesfully executed.
		}
		else // Otherwise an error message is displayed before the usage syntax.
		{
			printf("Wrong syntax.\n");
			displayHelp();
			return -1; // Error.
		}
	}
	if(argc==3)
	{
              struct stat st = {0};
		 if (stat("./ExtractedImages", &st) == -1) //creade folder to store extracting images
		 {
                 mkdir("./ExtractedImages", 0700);
		}
             if(argv[1][0]!='-' || isNumber(argv[1]+1)==-1 || isBmp(argv[2])==-1) // Correctness of both arguments is checked.
		{
			printf("Wrong syntax.\n");
			displayHelp();
			return -1;
		}
		unsigned int sWidth=atoi(argv[1]+1); // First argument is converted into an unsigned integer.
		printf("Opening the file: %s\n", argv[2]);
		FILE *inputStream=fopen(argv[2], "rb");
		if(inputStream==NULL)
		{
			printf("Failed to open the input file.\n");
			return -1;
		}
		printf("Loading the image.\n");
		BMP bmpInitial=BMP_read(inputStream);
		fclose(inputStream);
		if(bmpInitial==NULL)
			return -1; // Error messages are diplayed by the BMP_read function.
		printf("Modifying the image.\n");
		BMP bmpModified=BMP_modify(bmpInitial, sWidth);
		BMP_delete(bmpInitial);
		if(bmpModified==NULL)
			return -1; // Error messages are diplayed by the BMP_modify function.
		char* newName=generateNewName(argv[2], sWidth);
		printf("Saving the image at %s.\n", newName);
		FILE* outputStream=fopen(newName, "wb");
		free(newName);
		if(outputStream==NULL)
		{
			BMP_delete(bmpModified);
			printf("Failed to open the output file.\n");
			return -1;
		}
		BMP_write(bmpModified, outputStream);
		BMP_delete(bmpModified);
		fclose(outputStream);
		printf("%s has been modified.\n", argv[2]);
		return 0;
	}
	else // Error, if there are no arguments or the number of arguments exceeds 2.
	{
		printf("Wrong syntax.\n");
		displayHelp();
                return -1;
	}
}
