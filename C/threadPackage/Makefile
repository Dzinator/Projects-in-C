all : diner

#the diner executable is an implementation of the dining philosophers problem using myThreadPackage 
diner : test.o myThreadPackage.o 
	gcc -o diner test.o myThreadPackage.o -Werror -Wall -g -DHAVE_PTHREAD_RWLOCK=1 -lslack

test.o: test.c myThreadPackage.o myThreadPackage.h
	gcc -c test.c -Werror -Wall -g -DHAVE_PTHREAD_RWLOCK=1 -lslack

myThreadPackage.o: myThreadPackage.c myThreadPackage.h
	gcc -c myThreadPackage.h myThreadPackage.c -Werror -Wall -g -DHAVE_PTHREAD_RWLOCK=1 -lslack