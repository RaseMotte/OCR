// img_ptoc.h

# ifndef IMG_PROC_H
# define IMG_PROC_H

# include <stdlib.h>
# include <SDL/SDL.h>

int maxArrayPos(float *a, int nb);

void vHistogram(SDL_Surface *surface, int *array);

void hHistogram(SDL_Surface *surface, int *array);

void rHistogram(SDL_Surface *surface, int *array, int angle);

float getVariance(int *array);

int findAngle(SDL_Surface *surface);


void rotation(char *image_name);


# endif
