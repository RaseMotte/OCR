<<<<<<< HEAD
## Simple SDL mini code
 
CC=gcc
 
CPPFLAGS= `pkg-config --cflags sdl`
CFLAGS= -Wall -Wextra -Werror -std=c99 -O3 -g
=======
CC=gcc

CPPFLAGS= `pkg-config --cflags sdl`
CFLAGS=  -Wall -Wextra -std=c99 -O3
>>>>>>> e18a5bbc5a74247d6e67bea9237bb602b5e914d1
LDFLAGS=
LDLIBS= `pkg-config --libs sdl` -lSDL_image -lm
SRC=  img_proc.c image_proc.c pixel_operations.c main.c
OBJ= ${SRC:.c=.o}

all: main

main: ${OBJ}

<<<<<<< HEAD
copy:
	cp file.bmp /mnt/c/Users/Solannae/Desktop
	cp file2.bmp /mnt/c/Users/Solannae/Desktop 
=======
>>>>>>> e18a5bbc5a74247d6e67bea9237bb602b5e914d1
clean:
	rm -f *~ *.o
	rm -f main
