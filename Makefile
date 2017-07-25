CC=gcc

CPPFLAGS= `pkg-config --cflags sdl` `pkg-config --cflags gtk+-3.0`
CFLAGS= -Wall -Wextra -std=c99 -O3 
LDFLAGS = -L/usr/local/opt/libffi/lib
PKG_CONFIG_PATH = /usr/local/opt/libffi/lib/pkgconfig
LDLIBS= `pkg-config --libs sdl` -lSDL_image `pkg-config --libs gtk+-3.0`
SRC=  backprop.c image_to_net.c img_process.c
OBJ= ${SRC:.c=.o}

all: main
  
main: ${OBJ}

clean:
	rm -f *~ *.o
	rm -f main
