#This is a MakeFile for assignment1
CC = gcc
CFLAGS = -g
logParse: osAssign1.o
	$(CC) $(CFLAGS) -o logParse osAssign1.o
osAssign1.o: osAssign1.c 
	$(CC) $(CFLAGS) -c osAssign1.c 
clean:
	rm logParse osAssign1.o

