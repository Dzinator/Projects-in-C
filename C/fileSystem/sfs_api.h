/*
Yanis Hattab
260535922

PROGRAMMING ASSIGNMENT 2

	HEADER FILE

*/


#define MAX_FILENAME_LENGTH 12

int mksfs(int fresh);
void sfs_ls(void);
int sfs_fopen(char *name);
int sfs_fclose(int fileID);
int sfs_fwrite(int fileID, char *buf, int length);
int sfs_fread(int fileID, char *buf, int length);
int sfs_fseek(int fileID, int offset);
int sfs_remove(char *file);

typedef struct
{
	int diskBlock;
	int nextSlot;
}fatSlot;

typedef struct
{
	char fileName [MAX_FILENAME_LENGTH + 1];
	int location;
	int size;	
}aFile;





