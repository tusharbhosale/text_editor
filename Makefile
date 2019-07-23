project: editor.o buffer.o
	cc editor.o buffer.o -o project -lncurses
editor.o: editor.c buffer.h
	cc -c editor.c -Wall
buffer.o: buffer.c buffer.h
	cc -c buffer.c -Wall
