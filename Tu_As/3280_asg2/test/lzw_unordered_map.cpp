/*
* CSCI3280 Introduction to Multimedia Systems
*
* --- Declaration ---
*
* I declare that the assignment here submitted is original except for source
* material explicitly acknowledged. I also acknowledge that I am aware of
* University policy and regulations on honesty in academic work, and of the
* disciplinary guidelines and procedures applicable to breaches of such policy
* and regulations, as contained in the website
* http://www.cuhk.edu.hk/policy/academichonesty/
*
* Assignment 2
* Name : YE, ZHIZHEN
* Student ID : 1155046993
* Email Addr : yezhizhenjiakang@gmail.com
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <string>
#include <unordered_map>

#define MAX_NUMBER 4096
#define CODE_SIZE  12
#define TRUE 1
#define FALSE 0

using std::string;
using std::unordered_map;
typedef struct
{
    int length;
    char* str;
}entry;

typedef struct{
    entry entries[MAX_NUMBER];
    int size;
}mytable;
/* function prototypes */
unsigned int read_code(FILE*, unsigned int); 
void write_code(FILE*, unsigned int, unsigned int); 
void writefileheader(FILE *,char**,int);
void readfileheader(FILE *,char**,int *);
void compress(FILE*, FILE*, unordered_map<string,unsigned int>*);
void decompress(FILE*, FILE*, mytable*);
void init_table(mytable*);
void free_table(mytable*);
void write_table(mytable* , entry*);
void init_map(unordered_map<string,unsigned int>*);
void write_map(unordered_map<string,unsigned int>* , string, int);

int main(int argc, char **argv)
{
    int printusage = 0;
    int	no_of_file;
    char **input_file_names;    
	char *output_file_names;
    FILE *lzw_file;
    int current_file;
    FILE *myfile;
	char* temp_here;
	if (argc >= 3)
    {
		if ( strcmp(argv[1],"-c") == 0)
		{		
			/* compression */
			lzw_file = fopen(argv[2] ,"wb");
        
			/* write the file header */
			input_file_names = argv + 3;
			no_of_file = argc - 3;
			writefileheader(lzw_file,input_file_names,no_of_file);
			current_file = 1;
			unordered_map<string,unsigned int> mymap;
			init_map(&mymap);
			/* ADD CODES HERE */
        	while(current_file<=no_of_file)
            {
				if((myfile = fopen(argv[current_file+2], "rb"))==NULL)
				{
					perror("fopen()");
					exit(-1);
				}
				//compress
				compress(myfile, lzw_file, &mymap); 
				printf("%s compressed successfully\n", argv[current_file+2]);
				fclose(myfile);
				current_file++;
			}
			fclose(lzw_file);        	
		} else
		if ( strcmp(argv[1],"-d") == 0)
		{	
			/* decompress */
			lzw_file = fopen(argv[2] ,"rb");
			
			/* read the file header */
			no_of_file = 0;			
			readfileheader(lzw_file,&output_file_names,&no_of_file);
			current_file = 1;
			
			temp_here = strtok(output_file_names, "\n");
   
			mytable tb;
			init_table(&tb);
			/* ADD CODES HERE */
			while(current_file<=no_of_file)
            {
				if((myfile = fopen(temp_here, "wb"))==NULL)
				{
					perror("fopen()");
					exit(-1);
				}
				decompress(lzw_file, myfile,&tb);
				printf("%s decompressed successfully\n", temp_here);
				if(temp_here!=NULL)
					temp_here = strtok(NULL, "\n");
				fclose(myfile);
				current_file++;
			}
			fclose(lzw_file);		
			free(output_file_names);
		}else
			printusage = 1;
    }else
		printusage = 1;

	if (printusage)
		printf("Usage: %s -<c/d> <lzw filename> <list of files>\n",argv[0]);
 	
	return 0;
}

/*****************************************************************
 *
 * writefileheader() -  write the lzw file header to support multiple files
 *
 ****************************************************************/
void writefileheader(FILE *lzw_file,char** input_file_names,int no_of_files)
{
	int i;
	/* write the file header */
	for ( i = 0 ; i < no_of_files; i++) 
	{
		fprintf(lzw_file,"%s\n",input_file_names[i]);	
			
	}
	fputc('\n',lzw_file);

}

/*****************************************************************
 *
 * readfileheader() - read the fileheader from the lzw file
 *
 ****************************************************************/
void readfileheader(FILE *lzw_file,char** output_filenames,int * no_of_files)
{
	int noofchar;
	char c,lastc;

	noofchar = 0;
	lastc = 0;
	*no_of_files=0;
	/* find where is the end of double newline */
	while((c = fgetc(lzw_file)) != EOF)
	{
		noofchar++;
		if (c =='\n')
		{
			if (lastc == c )
				/* found double newline */
				break;
			(*no_of_files)++;
		}
		lastc = c;
	}

	if (c == EOF)
	{
		/* problem .... file may have corrupted*/
		*no_of_files = 0;
		return;
	
	}
	/* allocate memeory for the filenames */
	*output_filenames = (char *) malloc(sizeof(char)*noofchar);
	/* roll back to start */
	fseek(lzw_file,0,SEEK_SET);

	fread((*output_filenames),1,(size_t)noofchar,lzw_file);
	
	return;
}

/*****************************************************************
 *
 * read_code() - reads a specific-size code from the code file
 *
 ****************************************************************/
unsigned int read_code(FILE *input, unsigned int code_size)
{
    unsigned int return_value;
    static int input_bit_count = 0;
    static unsigned long input_bit_buffer = 0L;

    /* The code file is treated as an input bit-stream. Each     */
    /*   character read is stored in input_bit_buffer, which     */
    /*   is 32-bit wide.                                         */

    /* input_bit_count stores the no. of bits left in the buffer */

    while (input_bit_count <= 24) {
        input_bit_buffer |= (unsigned long) getc(input) << (24-input_bit_count);
        input_bit_count += 8;
    }
    
    return_value = input_bit_buffer >> (32 - code_size);
    input_bit_buffer <<= code_size;
    input_bit_count -= code_size;
    
    return(return_value);
}


/*****************************************************************
 *
 * write_code() - write a code (of specific length) to the file 
 *
 ****************************************************************/
void write_code(FILE *output, unsigned int code, unsigned int code_size)
{
    static int output_bit_count = 0;
    static unsigned long output_bit_buffer = 0L;

    /* Each output code is first stored in output_bit_buffer,    */
    /*   which is 32-bit wide. Content in output_bit_buffer is   */
    /*   written to the output file in bytes.                    */

    /* output_bit_count stores the no. of bits left              */    

    output_bit_buffer |= (unsigned long) code << (32-code_size-output_bit_count);
    output_bit_count += code_size;

    while (output_bit_count >= 8) {
        putc(output_bit_buffer >> 24, output);
        output_bit_buffer <<= 8;
        output_bit_count -= 8;
    }


    /* only < 8 bits left in the buffer                          */    

}

/*****************************************************************
 *
 * compress() - compress the source file and output the coded text
 *
 ****************************************************************/
void compress(FILE *input, FILE *output, unordered_map<string,unsigned int>*mymap)
{

	/* ADD CODES HERE */
	string p = "";
	char c;
	int i=0;
	while(TRUE)
	{
		c = getc(input);
		if(feof(input))
			break;
		i++;
		//search pc
		unordered_map<string,unsigned int>::const_iterator find_result = mymap->find(p+c);
		//found
		if(find_result != mymap->end())
		{
			p+= c;
		}
		else
		{
			write_code(output, (*mymap)[p],CODE_SIZE);
			write_map(mymap, p+c,i);
			p = string(1,c);
		}
	}
	if(p!="")
		//write p
		write_code(output, (*mymap)[p],CODE_SIZE);
	write_code(output, MAX_NUMBER - 1, CODE_SIZE);

}


/*****************************************************************
 *
 * decompress() - decompress a compressed file to the orig. file
 *
 ****************************************************************/
void decompress(FILE *input, FILE *output, mytable* tb)
{	

	/* ADD CODES HERE */
	unsigned int cw = read_code(input, CODE_SIZE);
	unsigned int pw;
	char c;
	char* p;
	//detect empty
	if(cw == MAX_NUMBER - 1	)	return;
	entry current_entry = tb->entries[cw];
	entry newentry;
	fwrite(current_entry.str, 1, current_entry.length,output);
	while(TRUE)
	{
		pw = cw;
		cw = read_code(input, CODE_SIZE);
		if(cw == MAX_NUMBER - 1	)
		{
			return;
		}
		//in the dict
		if(cw<tb->size)
		{
			current_entry = tb->entries[cw];
			fwrite(current_entry.str, 1, current_entry.length,output);
			c = current_entry.str[0];
			//write newentry
			p = tb->entries[pw].str;
			newentry.length = tb->entries[pw].length+1;
			newentry.str = (char*)malloc(newentry.length);
			newentry.str[newentry.length-1] = c;
			memcpy(newentry.str, p,newentry.length-1);
			write_table(tb, &newentry);
		}
		else
		{
			p = tb->entries[pw].str;
			newentry.length = tb->entries[pw].length+1;
			newentry.str = (char*)malloc(newentry.length);
			newentry.str[newentry.length-1] = c;
			memcpy(newentry.str, p,newentry.length-1);
			fwrite(newentry.str, 1, newentry.length,output);
			write_table(tb, &newentry);
		}

	}

}

void write_table(mytable* tb, entry* newentry)
{
	if(tb->size<MAX_NUMBER-1)
	{
		memcpy(tb->entries + tb->size,newentry,sizeof(entry)); 
		tb->size++;
	}
	else
	{
		free_table(tb);
		init_table(tb);
	}
}

void init_table(mytable* tb)
{
    int i;
    tb->size = 256;
    for(i=0;i<=255;i++)
    {
        tb->entries[i].length = 1;
        tb->entries[i].str=(char*)malloc(1);
        tb->entries[i].str[0] = i;
    }
}

void free_table(mytable* tb)
{
    int i;
    for(i=0;i<tb->size;i++)
		free(tb->entries[i].str);
}

void init_map(unordered_map<string,unsigned int>* mymap)
{
	int i;
	for(i=0;i<=255;i++)
	{
		(*mymap)[string(1,i)] = i;
	}
}
void write_map(unordered_map<string,unsigned int>* mymap, string entry, int i)
{
	//if not full
	if(mymap->size()<MAX_NUMBER-1)
	{ 
		(*mymap)[entry] = mymap->size();
	}
	else
	{
        printf("unordered_map full when read %d bytes... Clear the unordered_map.\n", i);
		mymap->clear();
		init_map(mymap);
	}
}