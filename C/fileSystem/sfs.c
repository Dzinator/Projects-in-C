/*
Yanis Hattab

Simple File System with emulated disk (as a file)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "sfs_api.h"
#include "disk_emu.h"


#define DISK_NAME "./virtualDisk"
#define SECTOR_SIZE 1024 //Sector size in bytes
#define SECTOR_NB 4096 //Number of sectors, giving us a total disk size of 4Mb
#define MAX_FILE_NB 100

//Global variables
aFile *root;	//Root directory (no subdirectory)
fatSlot *fatTable;	//File Allocation Table and data blocks of file
int *sectors;
int fileDescriptors[2][MAX_FILE_NB];	//Holds read and write pointers


/*
	-*-- Helper functions --*-
*/

void initFAT()
{
	/*
		Initializes the FAT table, is called by setSuperBlock
	*/
	int rootSize = MAX_FILE_NB * sizeof(aFile);
	int fatSize = ((8 * SECTOR_NB) / SECTOR_SIZE); //Gives us 32 sectors
	int rootSectors = (rootSize / SECTOR_SIZE);
	sectors = (int *)calloc(SECTOR_NB, sizeof(int));

	int i;
	for(i = 0; i < SECTOR_NB; i++) sectors[i] = 1; //initialize all sectors as free

	if((rootSize % SECTOR_SIZE) > 0) rootSectors++;
	if(((8 * SECTOR_NB) % SECTOR_SIZE) > 0) fatSize++;

	//init used sectors to occupied
	for(i = 0; i < (rootSectors + fatSize); i++)
	{
		sectors[i] = 0;
	}
	//ready other blocks 
	for(i = 0; i < SECTOR_NB; i++)
	{
		int aBlock = fatTable[i].diskBlock;
		if(aBlock != 0) sectors[aBlock] = 0;
	}
}

void setSuperBlock()
{
	/*
		Initializes or updates all necessary structures of file system
	*/
	int rootSize = MAX_FILE_NB * sizeof(aFile);
	int fatSize = SECTOR_NB * sizeof(fatSlot);
	int rootSectors = (rootSize / SECTOR_SIZE);
	int fatSectors = (fatSize / SECTOR_SIZE);

	if((rootSize % SECTOR_SIZE) > 0) rootSectors++;
	if((fatSize % SECTOR_SIZE) > 0) fatSectors++;

	void *buffer;
	//root
	buffer = calloc(rootSectors, SECTOR_SIZE);
	memcpy(buffer, root, rootSize);
	write_blocks(0, rootSectors, buffer); //write to disk

	//fat
	buffer = calloc(fatSectors, SECTOR_SIZE);
	memcpy(buffer, fatTable, fatSize);
	write_blocks(rootSectors, fatSectors, buffer); //write to disk

	//initilialize the fat table
	initFAT();

}


int getEmptySector()
{
	int i;
	for(i = 0; i < SECTOR_NB; i++)
	{
		if(sectors[i] == 1) return i; //Found empty
	}

	return -1; //Did not found empty

}

int getEndFAT(int index)
{
	int i = index;
	while(fatTable[i].nextSlot != 0)
	{
		i = fatTable[i].nextSlot; //check next linked element
	}
	//return the free block
	return fatTable[i].diskBlock;
}

int getFreeFAT()
{
	/*
	Iterate through the FAT to find an empty
	*/
	int i = 0;
	while(fatTable[i].diskBlock != 0) i++;
	return i;

}

int getSmallest(int i, int j)
{
	if(i < j) return i;
	else return j;
}

int getFile(char *pFileName)
{
	/*
	Find a file (if it exists) with a provided name
	*/
	int i;
	for(i = 0; i < MAX_FILE_NB; i++)
	{
		if(root[i].fileName != NULL)
		{
			int result = strcmp(pFileName, root[i].fileName); //check if name corresponds
			if(result == 0) return i; // return name if it corresponds
		}
	}
	//No file found, returns error code
	return -1;

}

/*
	--*--- Main Functions ---*--
*/

int mksfs(int fresh)
{
	/*
	Format virtual disk: 
		-create necessary disk resident data structures
		-initialize them

	@param fresh flag signals that file system should be created, if false, open existing one

	*/

	//creating used variables
	int rootSize = MAX_FILE_NB * sizeof(aFile);
	int fatSize = SECTOR_NB * sizeof(fatSlot);
	int rootSectors = (rootSize / SECTOR_SIZE);
	int fatSectors = (fatSize / SECTOR_SIZE);
	//adjusting to fat and root sizes
	if((rootSize % SECTOR_SIZE) > 0) rootSectors++;
	if((fatSize % SECTOR_SIZE) > 0) fatSectors++;

	if(fresh)
	{
		//Creating a fresh disk
		char *name = (char*) calloc(20, sizeof(char));
		strcpy(name, DISK_NAME);
		init_fresh_disk(name, SECTOR_SIZE, SECTOR_NB);

		root = (aFile *)calloc(MAX_FILE_NB, sizeof(aFile));
		fatTable = (fatSlot *)calloc(SECTOR_NB, sizeof(fatSlot));

		setSuperBlock();

	}
	else
	{
		//loading disk
		char *name = (char*) calloc(20, sizeof(char));
		strcpy(name, DISK_NAME);
		init_disk(name, SECTOR_SIZE, SECTOR_NB);
		//TO VERIFY !!
		root = (aFile *)calloc(rootSectors, SECTOR_SIZE);
		fatTable = (fatSlot *)calloc(fatSectors, SECTOR_NB);

		void *buffer = calloc(rootSectors, SECTOR_SIZE);
		//read the disk structures from disk and copy them
		//root
		read_blocks(0, rootSectors, buffer);
		memcpy(root, buffer, rootSize);
		//FAT
		buffer = calloc(fatSectors, SECTOR_SIZE);
		read_blocks(rootSectors, fatSectors, buffer);
		memcpy(fatTable, buffer, fatSize);
	}

	//initialize FAT table
	initFAT();
	return 0;
}

void sfs_ls(void)
{
	/*
	Lists the contents of the directory in details (including info stored in control blocks)
	*/
	int i;
	printf("\n---------------------------------\n");
	printf("%15s\t%15s\n", "-[File Name]-", "-[Size]-" );
	printf("---------------------------------\n\n");
	for(i = 0; i < MAX_FILE_NB; i++)
	{
		if(strlen(root[i].fileName) != 0)
		{
			printf("%15s\t%15d\n", root[i].fileName, root[i].size);
		}
	}
	printf("---------------------------------\n\n");
	return;


}

int sfs_fopen(char *name)
{
	/*
	Opens file and returns FAT index. 
		-If file does not exist, create new file with size 0. 
		-If file exists, open the file in append mode
	*/
	int fileIndex = getFile(name);

	if(fileIndex == -1)
	{
		//File doesn't already exist, we create it
		aFile *newFile = (aFile *) calloc(1, sizeof(aFile));
		//set its attributes
		strcpy(newFile->fileName, name);
		newFile->fileName[MAX_FILENAME_LENGTH + 1] = '\0';
		newFile->location = 0;
		newFile->size = 0;
		//find slot
		int i;
		for(i = 0; i < MAX_FILE_NB; i++)
		{
			if(strlen(root[i].fileName) == 0) break; //empty no name slot
		}
		//add the file in the slot
		if(i < MAX_FILE_NB)
		{
			fileIndex = i;
			root[i] = *newFile;
		} 
		else return -1; //error return, disk full
	}
	//Set file descriptors
	fileDescriptors[0][fileIndex] = 0;
	fileDescriptors[1][fileIndex] = root[fileIndex].size; //append position

	setSuperBlock(); //update super block on disk

	return fileIndex;

}

int sfs_fclose(int fileID)
{
	/*
	Closes a file, removing entry from the open file descriptor table
	*/
	if(fileID  >= MAX_FILE_NB) return -1; //invalid file ID
	//check if file is closed or inexistant, if so return
	if((fileDescriptors[0][fileID] == 0) && (fileDescriptors[1][fileID] == 0))
	{
		return 0;
	}
	//otherwise set its file descriptors to 0
	fileDescriptors[0][fileID] = 0;
	fileDescriptors[1][fileID] = 0;

	return 0;
}

int sfs_fwrite(int fileID, char *buf, int length)
{
	/*
	Writes "length" bytes of buf data in the open file, starting from current file pointer. 
	This increases the size of the file by “length” bytes.
	*/
	int wp = fileDescriptors[1][fileID]; //write pointer

	if(wp > (10 * SECTOR_NB)) return -1; //Memory overflow ERROR

	if(root[fileID].location == 0)
	{
		//New File
		int aBlock = getFreeFAT();
		int aSector = getEmptySector();
		root[fileID].location = aBlock;
		fatTable[aBlock].diskBlock = aSector;

		int unit = getSmallest(length, SECTOR_SIZE);
		//Writing data to disk
		void *buffer = calloc(1, SECTOR_SIZE);
		memcpy(buffer, buf, unit); //buffering
		write_blocks(aSector, 1, buffer);

		//update file properties and file descriptors
		root[fileID].size += unit;
		fileDescriptors[1][fileID] += unit;
		//If there is still data to write
		if(length > unit)
		{
			initFAT();
			sfs_fwrite(fileID, (buf + unit), (length - unit));
		}
		else setSuperBlock(); //done writing

		return length;
	}

	else if((wp % SECTOR_SIZE) != 0)
	{
		//Need more disk sectors
		int surplus = SECTOR_SIZE - (wp % SECTOR_SIZE);
		int found = 0;
		int loc = root[fileID].location;

		void *buffer = calloc(1, SECTOR_SIZE); //MAYBE char *

		while((fatTable[loc].nextSlot != 0) && (wp < (found + SECTOR_SIZE)))
		{
			loc = fatTable[loc].nextSlot;
			found += SECTOR_SIZE;
		}

		read_blocks(fatTable[loc].diskBlock, 1, buffer);
		//reading and buffering disk
		void *buffer2 = calloc(1, SECTOR_SIZE);
		memcpy(buffer2, buffer, (wp % SECTOR_SIZE));

		int unit = getSmallest(length, surplus);
		memcpy((buffer2 + (wp % SECTOR_SIZE)), buf, unit);

		//rest = total memory  - found memory
		int rest = SECTOR_SIZE - ((wp % SECTOR_SIZE) + unit);
		int bufferRest = root[fileID].size - found - unit - (wp % SECTOR_SIZE);
		if(bufferRest > rest) bufferRest = rest;
		if((rest > 0) && (bufferRest > 0))
		{
			memcpy((buffer2 + unit + (wp % SECTOR_SIZE)), buffer, bufferRest);
		}

		//update disk
		int x = getEndFAT(root[fileID].location);
		write_blocks(x, 1, buffer2);
		//update file properties and descriptor table
		root[fileID].size += unit;
		fileDescriptors[1][fileID] += unit;

		if(length > surplus)
		{
			//Still stuff to write
			initFAT();
			sfs_fwrite(fileID, (buf + unit), (length - unit));
		}
		else{
			setSuperBlock(); //update superblock
		}

		return length;		
	}
	
	else if(root[fileID].size == wp)
	{
		//need to find free space and write data
		int aBlock = getFreeFAT();
		int aSector = getEmptySector();
		int loc = root[fileID].location;
		//find free block
		while(fatTable[loc].nextSlot != 0)
		{
			loc = fatTable[loc].nextSlot;
		}
		fatTable[loc].nextSlot = aBlock;
		fatTable[aBlock].diskBlock = aSector;

		int unit = getSmallest(length, SECTOR_SIZE);
		void *buffer = calloc(1, SECTOR_SIZE);
		memcpy(buffer, buf, unit); //copy unit length of buf data
		//write to disk
		write_blocks(aSector, 1, buffer);
		//update file properties and descriptor table
		root[fileID].size += unit;
		fileDescriptors[1][fileID] += unit;

		if(length > SECTOR_SIZE)
		{
			//still work to do
			initFAT();
			sfs_fwrite(fileID, (buf + unit), (length - unit));
		}
		else setSuperBlock(); //done

		return length;
	}

	else
	{
		int found = 0;
		int loc = root[fileID].location;

		while((fatTable[loc].nextSlot != 0) && (found != wp))
		{
			loc = fatTable[loc].nextSlot;
			found += SECTOR_SIZE;
		}
		//copy to a buffer the precise amnt of data we'll write
		int unit = getSmallest(length, SECTOR_SIZE);
		void *buffer = calloc(1, SECTOR_SIZE);
		//memset(buffer, 0, SECTOR_SIZE);
		memcpy(buffer, buf, unit);
		//write to disk
		write_blocks((fatTable[loc].diskBlock), 1, buffer);
		//update file descriptor table
		fileDescriptors[1][fileID] += unit;

		if(length > SECTOR_SIZE)
		{
			sfs_fwrite(fileID, (buf + unit), (length - unit)); //still writing to do
		}

		return length;
	}

	return 0;
}

int sfs_fread(int fileID, char *buf, int length)
{
	/*
	Reads "length" from file (at current read pointer) and writes it to buf
	*/

	//check if file is open, if not abort and retrun 0 (nothing read)
	if((fileDescriptors[0][fileID] == 0) && (fileDescriptors[1][fileID] == 0)) return 0;

	int rc = 0;	//read count
	int rp = fileDescriptors[0][fileID]; //read pointer
	int readSize = 0;
	int loc = root[fileID].location;
	int fSize = root[fileID].size;
	int l = length;

	void *buffer = calloc(1, SECTOR_SIZE);

	while((fSize > 0) && (length > 0))
	{
		//we keep going until we read everything
		readSize = getSmallest(length, fSize);
		if(readSize > SECTOR_SIZE) readSize = SECTOR_SIZE; // we read by block
		read_blocks(fatTable[loc].diskBlock, 1, buffer);
		if(((rc - rp) < 0) && ((rc - rp + SECTOR_SIZE) < 0))
		{
			rc += SECTOR_SIZE; //one read
			fSize -= SECTOR_SIZE; //update remaining to read 
		}
		else if((rc - rp) < 0)
		{
			int diff = (rp - rc);
			readSize = getSmallest(length, (SECTOR_SIZE - diff)); 
			//buffer data
			memcpy(buf, (buffer + diff), readSize);
			//update variables
			rc += diff;
			length -= readSize;
			fSize -= readSize;
			rc += readSize;
			//update read pointer
			fileDescriptors[0][fileID] += readSize;
		}
		else
		{
				//copy data
			memcpy((buf + rc -rp), buffer, readSize);
			//update variables
			length -= readSize;
			fSize -= readSize;
			rc += readSize;
			//update file descriptor table's read pointer
			fileDescriptors[0][fileID] += readSize;
		}

		loc = fatTable[loc].nextSlot;
	}

	//returning appropriate read amount
	if(readSize == strlen(buf))
	{
		return readSize;
	}
	else return l;
}

int sfs_fseek(int fileID, int offset)
{
	/*
	Move reading head of fileID by "offset" amount
	*/
	if(strlen(root[fileID].fileName) == 0) return -1;// file doesn't exist
	if(offset > root[fileID].size) return -1; //offest too larger
	//We update the filedescriptors by the offset
	fileDescriptors[0][fileID] = offset;
	fileDescriptors[1][fileID] = offset;

	return 0;
}

int sfs_remove(char *file)
{
	/*
	Removes file from directory entry
		-Releases the FAT entries and data blocks used by the file (free for later use)
	*/

	int fileIndex = getFile(file);

	if(fileIndex == -1) return -1; //file doesn't exist

	int fatIndex = root[fileIndex].location;
	int i;
	for(i = 0; i < MAX_FILENAME_LENGTH; i++)
	{
		//blank the name
		root[fileIndex].fileName[i] = '\0';
	}

	//clear FAT entry
	int next;
	while(fatIndex != 0)
	{
		next = fatTable[fatIndex].nextSlot;
		fatTable[fatIndex].nextSlot = 0;
		fatTable[fatIndex].diskBlock = 0;
		fatIndex = next;
	}
	
	return 0;
}

/*
	That's all folks!
*/