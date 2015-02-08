Yanis Hattab
260535922
------------------

Simple File System
------------------

I wrote my code in sfs.c and modified the header file to declare structures and signatures,
that header file is named sfs_api.h

I included a make file that produces executables ftest and htest.

To test my file system one can also build executables of the tests provided using commands:

For ftest:
	gcc -g -Wall -o ./ftest sfs.c disk_emu.c sfs_ftest.c	

For htest:
	gcc -g -Wall -o ./htest sfs.c disk_emu.c sfs_htest.c


The htest has some errors appearing in it, I was not able to find out why.





------------------

Methods signatures:

--> I have implemented all the required functionalities, although htest seems not to fully work.

int mksfs(int fresh);
void sfs_ls(void);
int sfs_fopen(char *name);
int sfs_fclose(int fileID);
int sfs_fwrite(int fileID, char *buf, int length);
int sfs_fread(int fileID, char *buf, int length);
int sfs_fseek(int fileID, int offset);
int sfs_remove(char *file);

Helper functions (not in header):

void initFAT();
void setSuperBlock();
int getEmptySector();
int getEndFAT(int index);
int getFreeFAT();
int getSmallest(int i, int j);
int getFile(char *pFileName);

