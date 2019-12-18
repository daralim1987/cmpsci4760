all: master palin

CC = gcc
MOBJS = master.o
POBJS = palin.o

master: $(MOBJS)
	$(CC) -g -o master master.o

palin: $(POBJS)
	$(CC) -g -o palin palin.o

palin.o: shm.h
	$(CC) -g -c palin.c

master.o: shm.h
	$(CC) -g -c master.c
//.PHONY: clean

clean:
	// /bin/rm -f core *.o  $(MOBJS) $(POBJS) palin master
	 /bin/rm -f *.o $(MOBJS) $(POBJS)
