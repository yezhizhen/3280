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
	FILE *html = fopen( "bonus.html", "w" );
    
	fprintf( html, "<!DOCTYPE html>\n <head></head>\n <body>\n<p>");

	unsigned char r, g, b;
	unsigned char gray;
	char temp;
	//indicate if <font..> has not been closed
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
			gray = 0.299 * r + 0.587 * g + 0.114 * b;
            //now, quantize gray
			temp = shades[gray/16];
			//if not black, use the font
			if(!(r < 10 &&  b < 10 && g<10))
			{
				fprintf(html,"<font color=\"rgb(%u,%u,%u)\">%c", r,g,b,temp);
			}
			else
			{
				if(temp!=' ')
					fprintf(html,"%c",temp);
				else
					fprintf(html,"&nbsp;");
			}
		}
		//now, at the end of an line.
		fprintf(html,"<br/>\n");
	}

	fprintf(html,"</div>\n</body>");

	//	close html file
	fclose(html);

	return 0;

} 
