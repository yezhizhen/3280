/*

	CSCI 3280, Introduction to Multimedia Systems
	Spring 2016
	
	Assignment 01 Skeleton

	inverse.cpp
	
*/

#include "stdio.h"
#include "malloc.h"
#include "memory.h"
#include "math.h"
#include "bmp.h"		//	Simple .bmp library

#define MAX_SHADES 8


//
//	***** PUT ALL YOUR CODE INSIDE main() *****
//
int main( int argc, char** argv)
{

	char shades[MAX_SHADES] = {' ', '.', '+', '*', 'X', '%', '#', '@'};
	int 	w, h;
	
	//	Open ASCII Art file ( command line: argv[1] )
	//
	FILE *fin = fopen( argv[1], "r" );
	fscanf( fin, "%d %d", &w, &h );
		
	//	Create a Bitmap object accordingly
	Bitmap myBmp( w, h);

	//
	//	Your code goes here ....
	//
	//	Advice:
	//	Use Bitmap.setColor(x,y,R,G,B) for setting color of pixel at (x,y)
	//	Use fgetc()
	//
	
    unsigned char gray;
	unsigned char temp;
	//skipping the first line
	temp = fgetc(fin);
	int max = w * h;
	int counter = 0;
	for (;counter<max;counter++)
	{
		//if new line character, go to the next
		if( (temp = fgetc(fin))=='\n')
		{
			temp = fgetc(fin);
		}

		for(int i = 0;i<MAX_SHADES;i++)
		{
			if (temp == shades[i])
			{
				gray =  i * 32;
				myBmp.setColor(counter%w,counter/w,gray,gray,gray);
				break;
			}
		}
		//myBmp._data
	}
	
	//	Save Bitmap file ( command line: argv[2] )	
	myBmp.save( argv[2] );
	fclose(fin);
	

	return 0;

} 
