all: ftest htest

disk_emu.o: disk_emu.c disk_emu.h
	gcc -g -Wall -c disk_emu.c disk_emu.h

sfs.o: sfs.c sfs_api.h disk_emu.h 
	gcc -g -Wall -c sfs.c sfs_api.h disk_emu.h

sfs_ftest.o:  sfs_ftest.c sfs_api.h
	gcc -g -Wall -c sfs_ftest.c sfs_api.h

sfs_htest.o:  sfs_htest.c sfs_api.h
	gcc -g -Wall -c sfs_htest.c sfs_api.h

ftest:  sfs.o disk_emu.o sfs_ftest.o
	gcc -g -Wall -o ftest sfs_ftest.o sfs.o disk_emu.o 

htest: sfs.o disk_emu.o sfs_htest.o
	gcc -g -Wall -o htest sfs_htest.o sfs.o disk_emu.o 
