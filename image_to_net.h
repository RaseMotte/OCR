# ifndef _IMAGE_TO_NET_H_
# define _IMAGE_TO_NET_H_

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include <SDL/SDL.h>

# include "img_process.h"

 void      	build_target_list(char c, double *target_list);

 char        	target_to_char(double *target);

 void      	build_training_list(SDL_Surface *img, double *image_vect);

 int 		char_to_int(char *nb);

 void 		addSpace();
# endif
