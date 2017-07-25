// img_proc.c
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>

#include "pixel_operations.h"
#include "img_proc.h"

#define ANGLE 30

int maxArrayPos(float *a, int nb)
{
    float max = 0;
    int pos = 0;
    for(int i = 0; i < nb; i++)
    {
        if (a[i] > max)
        {
            max = a[i];
            pos = i;
        }
    }
    return pos;
}

void vHistogram(SDL_Surface *surface, int *array) // vertical histogram
{
    Uint32 pixel;
    Uint8 gray;
    int sum;
    for(int y=0; y < surface->h; y++)
    {
        sum = 0;
        for(int x=0; x < surface->w; x++)
        {
            pixel = getpixel(surface, x, y);
            SDL_GetRGB(pixel, surface->format, &gray, &gray, &gray);

            if(!gray)
                sum++;
        }
        array[y] = sum;
    }
}

void hHistogram(SDL_Surface *surface, int *array) // horizontal histogram
{
    Uint32 pixel;
    Uint8 gray;
    int sum;
    for(int x=0; x < surface->w; x++)
    {
        sum = 0;
        for(int y=0; y < surface->h; y++)
        {
            pixel = getpixel(surface, x, y);
            SDL_GetRGB(pixel, surface->format, &gray, &gray, &gray);

            if(!gray)
                sum++;
        }
        array[x] = sum;
    }
}

void rHistogram(SDL_Surface *surface, int *array, int angle) // with an angle
{
    Uint32 pixel;
    Uint8 gray;
    float tany = 0;
    int nx = 0;
    int ny = 0; // nouvelles valeurs pour x et y
    int sum;

    int processed = 0;

    for(int y=0; y < surface->h; y++)
    {
        sum = 0;
        nx = 0;
        ny = y;
        while(nx >= 0 && nx < surface->w && ny >= 0 && ny < surface->h)
        {
            tany = tan(3.14159*angle/180);
            nx = nx + 1;
            ny = y + nx * tany;

            //printf("nx: %d, ny: %d, tany: %f\n", nx, ny, tany);

            if(nx >= 0 && nx < surface->w && ny >= 0 && ny < surface->h)
            {
                pixel = getpixel(surface, nx, ny);
                SDL_GetRGB(pixel, surface->format, &gray, &gray, &gray);

                if(!gray)
                    sum++;
            }
        }

        if(nx >= surface->w)
        {
            array[processed+1] = sum;
            processed++;
        }
    }
    printf("Lines processed for angle %d: %d\n", angle, processed);
    array[0] = processed;
}

float getVariance(int *array)
{
    float variance = 0;
    float average = 0;
    int nb = array[0];
    for(int i=1; i <= nb; i++)
    {
        average += array[i];
    }
    average /= nb;

   for(int i=1; i <= nb; i++)
    {
        variance += pow((array[i] - average), 2);
    }
    variance /= nb;

    return variance;
}

int findAngle(SDL_Surface *surface)
{
    int histo[surface->h + 1];
    float variance[ANGLE*2 + 1];

    int i = 0;
    for(int angle=-ANGLE; angle <= ANGLE; angle++)
    {
        rHistogram(surface, histo, angle);

        variance[i] = getVariance(histo);

        printf("angle: %d, variance: %f\n\n", -ANGLE+i, variance[i]);


        i++;
    }


    return -(-ANGLE + maxArrayPos(variance, ANGLE*2 + 1));
}

void rotation(char *image_name)
{
	init_sdl();
	SDL_Surface *img;
	Uint32 cur_pixel;
	img = load_image(image_name);
	display_image(img);
	int a=findAngle(img);
		for (int y=0; y< img->h;y++)
			{
				int oy=y/2;
				for (int x = 0; x < img->w; x++)
					{
						int ox=x/2;
		 				cur_pixel = getpixel(img, x, y);
		 				int _x = (x-x/2)*cos(a) + ox;
         					int _y = (y-y/2)*sin(a) + oy;
		 				putpixel(img, _x, _y, cur_pixel);
		 				display_image(img);

					}
			}
}

