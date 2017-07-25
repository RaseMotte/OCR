/*________________________________________*
 |	
 |	* File Name     : image_to_net.c
 |	* Creation Date : 23-11-2016
 |  * Last Modified : Mar  6 déc 19:21:44 2016
 |	* Created by    :	    Claire
 *________________________________________**/

# include <stdio.h>
# include <stdlib.h>
# include <SDL/SDL.h>

# include "backprop.h"
# include "image_to_net.h"
# include "img_process.h"

void 		build_target_list(char c, double *target_list)
{
	int 	val;

	val = (int) c;
	if ( val > 47 && val < 58 )
		target_list[(int) c - 48] = 1.0;
	else if (val > 64 && val < 91)
		target_list[(int) c - 55] = 1.0;
	else if (val > 96 && val < 123)
		target_list[(int) c - 61] = 1.0;
	else
	{
		printf("BUILD_TARGET_LIST : unknown character\n");
	}
}

char 		target_to_char(double *target)
{
	int 	i;

	i = 0;
	for (size_t j = 0; j < 62; j++)
	{
		if (target[j] > target[i])
			i = j;
	}
	/*while (target && (*target < 0.8))
	{
		i++ ;
		target += 1;
	}*/
	if (target)
	{
		if ( i == 0 || i < 10 )
			return (i + 48.0);
		else if ( i > 9 && i < 36 )
			return (i + 55.0);
		else if (i > 35 && i < 62)
			return (i + 61.0);
		else 
		{
			printf("TARGET_TO_CHAR : unexpected value in target vector\n");
			return '\0';
		}
	}
	else 
	{
		printf("TARGET_TO_CHAR : failed to find a non null value in the vector\n");
		return '\0';
	}
}

void 		build_training_list(SDL_Surface *img, double *image_vect) //load inputs vector list with image corresponding vector
{
	int	 	imgsize;
	int	 	k;
	//double 	*image_vect = malloc((1 + 28 * 28) * sizeof(double));

	imgsize = 28 * 28;
	if (imgsize != 28 * 28)
	{
		printf("LOAD_INPUTS_WITH_IMAGE : dimension error between image and net input\n");
		//return NULL;
	}
	
	k = 0;
	for (int i = 0; i < 28; i++)
	{
		for (int j = 0; j < 28; j++)
		{
			image_vect[k] = ((double) getpixel(img, j, i)) / (255.0 * 255.0 * 255.0);
			k++;
		}
	}
	image_vect[k + 1] = 1.0;
//	return image_vect;
}

int 		char_to_int(char *nb)
{
	int 	result;

	result = 0;
	while (*nb != '\0')
	{
		result += *nb - 48;
		result *= 10;
		nb++;
	}
	return (result / 10);
}

