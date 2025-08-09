HEADERS = cs472-proto.h
CFLAGS = -g -Wall
CC = gcc

SRCPATH = src/
OBJPATH = .obj/

bards: obj main

main: $(OBJPATH)main.o $(OBJPATH)cmd.o
	$(CC) $(CFLAGS) $(OBJPATH)main.o $(OBJPATH)cmd.o -o bards.exe

obj: $(SRCPATH)main.c
	$(CC) $(CFLAGS) -c $(SRCPATH)main.c -o $(OBJPATH)main.o
	$(CC) $(CFLAGS) -c $(SRCPATH)cmd.c -o $(OBJPATH)cmd.o

cmd: $(SRCPATH)cmd.c
	$(CC) $(CFLAGS) -c $(SRCPATH)cmd.c -o $(OBJPATH)cmd.o

clean:
	rm $(OBJPATH)*
	rm ./bards.exe
