//Author Peter Adamson
//This program expects the following syntax when being run: ./a.out n file.txt < sample
//where n is the number of frames, file.txt is the memory file to be read, and sample is the list of logical addresses

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//global variable declarations
int pageTable[256];
int indicatorTable[256];
int modified[256];
int pageFaults;
int freeFrame;
int full;
int numOfFrames;

//function declarations
int findFreeFrame();

int main(int argc, char **argv)
{
	//variable set up
	pageFaults = 0;
	freeFrame = 0;
	full = 0;
	numOfFrames = atoi(argv[1]);
	int frames[numOfFrames * 256];
	long int logAddr;
	int i;
	char *character;
	character = (char*)malloc(sizeof(char*));

	//open the file
	FILE *file = fopen(argv[2], "rb");

	//populate page table and indicator table
	for(i = 0; i < 256; i++)
	{
		pageTable[i] = 0;
		indicatorTable[i] = 0;
		modified[i] = 0;
	}

	//loop as long as there are still logical addresses to be read and the format is correct
	while(!feof(stdin) && (1 == scanf("%ld",&logAddr)))
	{
		fseek(file, logAddr, SEEK_SET);	//set the file stream to the appropriate location
		int val = fgetc(file);		//get the character at the file stream location
		val = (char) val;		//cast the character to char

		//get page number and offset of logical address
		int pageNum = logAddr / 256;	
		int offset = logAddr % 256;
		if(indicatorTable[pageNum] == 1)	//we have a valid page table entry
		{
			int physAddr = pageTable[pageNum] * 256 + offset;
			printf("%d->%d->%c\n",logAddr,physAddr,frames[physAddr]);
		}
		else					//the page is not loaded
		{	
			pageFaults = pageFaults + 1;
			int frameNum = findFreeFrame();
			indicatorTable[pageNum] = 1;
			pageTable[pageNum] = frameNum;
			int physAddr = frameNum * 256 + offset;
			int ch[255];

			//load the page
			for(i = 0; i < 256; i++)
			{
				fseek(file,pageNum*256 + i, SEEK_SET);
				int v = fgetc(file);
				v = (char) v;
				ch[i] = v;
			}

			//put the page into physical memory
			int count = 0;
			for(i = frameNum * 256; i < frameNum * 256 + 256; i++)
			{
				frames[i] = ch[count];
				count = count + 1;
			}
			printf("%d->%d->%c\n",logAddr,physAddr,frames[frameNum * 256 + offset]);
		}	
	}

	//close the file
	fclose(file);

	printf("total page faults: %d\n",pageFaults);
}

int findFreeFrame()
{
	int found;
	if(full == 0)	//still free frames
	{
		found = freeFrame;
		freeFrame = freeFrame + 1;
		if(freeFrame >= numOfFrames)	//loop freeframes
		{
			full = 1;
			freeFrame = 0;
		}
		return found;
	}
	else		//no free frames
	{
		int j;
		int index;
		for(j = 0; j < 256; j++)
		{
			if(pageTable[j] == freeFrame)	//we have found the entry to remove
			{
				pageTable[j] = 0;
				indicatorTable[j] = 0;
				break;
			}
		}
		found = freeFrame;
		freeFrame = freeFrame + 1;
		if(freeFrame >= numOfFrames)	//loop freeframes
		{
			freeFrame = 0;
		}
		return found;
	}
}
