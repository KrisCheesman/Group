<<<<<<< HEAD
CFLAGS = -I../include -pthread -O1
LIB    = ../lib/x11/libglfw.a
LFLAGS = $(LIB) -lrt -lX11 -lGLU -lGL -pthread -lm -lXrandr

all: ms

ms: ms.o ../lib/x11/libglfw.a
	gcc ms.o $(LFLAGS) -o ms

ms.o: ms.c
	gcc -c $(CFLAGS) ms.c

clean:
	rm -f ms
	rm -f *.o

=======
CFLAGS = -I../include -pthread -O1
LIB    = ../lib/x11/libglfw.a
LFLAGS = $(LIB) -lrt -lX11 -lGLU -lGL -pthread -lm -lXrandr

all: ms

ms: ms.o ../lib/x11/libglfw.a
	gcc ms.o $(LFLAGS) -o ms

ms.o: ms.c
	gcc -c $(CFLAGS) ms.c

clean:
	rm -f ms
	rm -f *.o
>>>>>>> 970709c35b7fe5dc650c3a0dc928001b89ac8c6a
