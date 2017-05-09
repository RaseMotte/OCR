CC=gcc

CPPFLAGS= `pkg-config --cflags sdl` `pkg-config --cflags gtk+-3.0`
CFLAGS= -lm -ldl -Wall -Wextra -std=c99 -O3
LDFLAGS= -rdynamic
LDLIBS= `pkg-config --libs sdl` -lSDL_image `pkg-config --libs gtk+-3.0`
SRC= main.c 
OBJ= ${SRC:.c=.o}

all: main
  
main: ${OBJ}

clean:
	rm -f *~ *.o
	rm -f main
