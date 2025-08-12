HEADERS = cs472-proto.h
CFLAGS = -g -Wall
CC = gcc

SRCPATH = src/
OBJPATH = .obj/

bards: obj main

main: $(OBJPATH)main.o $(OBJPATH)cmd.o
	$(CC) $(CFLAGS) $(OBJPATH)main.o $(OBJPATH)cmd.o $(OBJPATH)io.o \
    $(OBJPATH)redistrict.o $(OBJPATH)state.o  $(OBJPATH)stack.o \
	-o bards

obj: $(SRCPATH)main.c
	$(CC) $(CFLAGS) -c $(SRCPATH)main.c -o $(OBJPATH)main.o
	$(CC) $(CFLAGS) -c $(SRCPATH)cmd.c -o $(OBJPATH)cmd.o
	$(CC) $(CFLAGS) -c $(SRCPATH)io.c -o $(OBJPATH)io.o
	$(CC) $(CFLAGS) -c $(SRCPATH)redistrict.c -o $(OBJPATH)redistrict.o
	$(CC) $(CFLAGS) -c $(SRCPATH)struct/state.c -o $(OBJPATH)state.o
	$(CC) $(CFLAGS) -c $(SRCPATH)struct/stack.c -o $(OBJPATH)stack.o

cmd: $(SRCPATH)cmd.c
	$(CC) $(CFLAGS) -c $(SRCPATH)cmd.c -o $(OBJPATH)cmd.o


clean:
	rm $(OBJPATH)*
	rm ./bards.exe
