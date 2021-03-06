/*

	CSCI 3280, Introduction to Multimedia Systems
	Spring 2015
	
	Assignment 01 Skeleton

	ascii.cpp
	
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
	int		w, h;

	//	Read in image data ( command line: argv[1] )
	//
	Bitmap myBmp( argv[1] );
	
	w = myBmp.getWidth();
	h = myBmp.getHeight();

	//	Prepare output TXT file ( command line: argv[2] )
	//
	FILE *fout = fopen( argv[2], "w" );
	fprintf( fout, "%d %d", w, h );
	fprintf( fout, "\n", w, h);
	unsigned char r, g, b;
	unsigned char gray;
    int denominator = 256/MAX_SHADES;
	//
	//	Your code goes here ....
	//	
	//	Advice:
	//	Use Bitmap.getColor(x,y,R,G,B) for getting color of pixel at (x,y)
	//	Use fputc() to read character and "\n" to end your line.
	//
	for(int row = 0; row<h;row++)
	{
		for(int col = 0; col<w;col++)
		{
			myBmp.getColor(col, row, r, g, b);
			gray = 0.299 * r + 0.587 * g + 0.114 * b ;
            //don't forget rounding!
            int up = ceil(gray / (denominator*1.0));
            int down = floor(gray/(denominator*1.0));
            int index = abs(gray-up)> abs(gray-down) ? down : up;
            //now, quantize gray
			fputc(shades[index],fout);
		}
        //newline unless the lass one
		if (row != h-1)
			fputc('\n', fout);
	}

	
	//	close ASCII file
	fclose(fout);

	return 0;

} 
