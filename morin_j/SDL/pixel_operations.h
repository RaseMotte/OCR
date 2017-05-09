// pixel_operations.h
 
# ifndef PIXEL_OPERATIONS_H_
# define PIXEL_OPERATIONS_H_
 
# include <stdlib.h>
# include <stdint.h>
# include <SDL/SDL.h>
 
Uint32 			getpixel(SDL_Surface *surface, unsigned x, unsigned y);
void 			putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);
SDL_Surface* 	load_image(char *path);
SDL_Surface*	display_image(SDL_Surface *img);
SDL_Surface*	image_binarize(char *umage_name);
 
# endif
