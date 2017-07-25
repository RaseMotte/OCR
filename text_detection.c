/* Text's area recognition inside images */

#include <stdio.h>
#include <stdlib.h>
#include "text_detection.h"
#include "pixel_operations.h"


void queue_init(struct queue *queue)
{
	queue->store = NULL;
	queue->size = 0;
}

void queue_push(struct queue *queue, void *elm)
{
	queue->size += 1;
	queue->store = malloc(sizeof(struct list));
	queue->store->data = elm;
	queue->store->next = NULL;
}

SDL_Surface* GetChar(SDL_Surface *surface, int x, int y, int xmax, int ymax)
{
	Uint32 pixel;
	Uint8 r, g, b, r2, g2, b2;
	int xtemp, ytemp;
	ytemp = y;

	SDL_Surface *letter = SDL_CreateRGBSurface(0, 28, 28, 32, 0, 0, 0, 0);

	while(ymax - y > 28)
	{
		pixel = getpixel(surface, x, ytemp);
		SDL_GetRGB(pixel, surface->format, &r, &g, &b);
		if(r != 0 && g != 0 && b != 0)
		{ 
			xtemp = x;
			while(xtemp < xmax && getpixel(surface, xtemp + 1, ytemp) == pixel)
			{
				++xtemp;
			}
			if(xtemp == xmax)
			{
				if(y == ytemp)
				{
					++y;
					++ytemp;
				}
				else
				{
					if(ytemp - y > 13)
					{
						ymax = ytemp;
					}
					else
					{
						++ytemp;
					}
				}
			}
			else
			{
				++ytemp;
			}
		}
		y++;
	} 

	int offset_x = (28 - (xmax - x))/2;
	int offset_y = (28 - (ymax - y))/2;
	for(int i = 0; i < 27; ++i)
	{
		for(int j = 0; j < 27; ++j)
		{
			if(i < 28 - offset_y && i > offset_y && j < 28 - offset_x && j >offset_x)
			{
				pixel = getpixel(surface, j, i);
				SDL_GetRGB(pixel, surface->format, &r, &g, &b);
				pixel = SDL_MapRGB(letter->format, r, g, b);
				putpixel(letter, j, i, pixel);
			}
			else
			{
				pixel = getpixel(letter, j, i);
				SDL_GetRGB(pixel, letter->format, &r, &g, &b);
				r = 255;
				g = 255;
				b = 255;
				pixel = SDL_MapRGB(letter->format, r, g, b);
				putpixel(letter, j, i, pixel);
			}
		}
	}
	//SDL_SaveBMP(letter, "letter.bmp");
	return letter;
}

void Detection(SDL_Surface *surface, SDL_Surface *image, struct queue *queue)
{
	Uint32 pixel;
	Uint8 r, g, b;
	int ypos, xpos;

	for(int i = 0; i < surface->h; ++i)
	{
		for(int j = 0; j < surface->w; ++j)
		{
			pixel = getpixel(surface, j, i);
			SDL_GetRGB(pixel, surface->format, &r, &g, &b);
			if(r == 0 && g == 255 && b == 255)
			{
				xpos = j;
				ypos = i;
				while(i < surface->h && getpixel(surface, xpos, i + 1))
				{
					++i;
				}
				while(j < surface->w - 1)
				{ 
					while(j < surface->w - 1 && r == 0 && g == 255 && b == 255)
					{
						++j;
						pixel = getpixel(surface, j, i);
						SDL_GetRGB(pixel, surface->format, &r, &g, &b);
					}
					queue_push(queue, GetChar(image, xpos, ypos, j, i));
					while(j < surface->w - 1 && r == 255 && g == 255 && b == 255)
					{
						++j;
						pixel = getpixel(surface, j, i);
						SDL_GetRGB(pixel, surface->format, &r, &g, &b);
					} 
					j++;
				}
			}
		}
	}
}

void FindText(SDL_Surface *surface, SDL_Surface *image)
{
	Uint32 pixel, pixel2;
	Uint8 r, g, b, r2, g2, b2;
	int xpos, ypos, x, ytemp;
	for(int y = 0; y < surface->h; ++y)
	{
		x = 0;
		ypos = y;

		pixel = getpixel(surface, x, y);
		SDL_GetRGB(pixel, surface->format, &r, &g, &b);

		if(r == 0 && g == 0 && b == 0)
		{
			y = y - 1;
			xpos = x;

			//Get height of text line
			while(ypos < surface->h && getpixel(surface, xpos, ypos + 1) == pixel)
			{
				++ypos;
			}
			++ypos;

			while(x < surface->w)
			{
				ytemp = ypos;
				pixel2 = getpixel(image, x, y);
				SDL_GetRGB(pixel2, image->format, &r2, &g2, &b2);

				//Delete blank columns
				while(r2 != 0 && g2 != 0 && b2 != 0 && ytemp == ypos)
				{
					ytemp = y;
					while(ytemp < ypos && getpixel(image, x, ytemp) == pixel2)
					{
						++ytemp;
					}
					if(ytemp == ypos && x < surface->w - 1)
					{
						++x;
						pixel2 = getpixel(image, x, y);
						SDL_GetRGB(pixel2, image->format, &r2, &g2, &b2);
					}
					else
					{
						ytemp = ypos - 1;
					}
				}

				r = 0;
				g = 255;
				b = 255;
				pixel2 = SDL_MapRGB(surface->format, r, g, b);
				if(x < surface->w - 1)
				{
					for(int i = y; i < ypos + 1; ++i)
					{
						putpixel(image, x, i, pixel2);
					}
				}       
				++x;                
			}
		}
		y = ypos;
	}
}

SDL_Surface* Xtraversal(SDL_Surface *surface, int width, int height, int offset)
{
	SDL_LockSurface(surface);
	Uint32 pixel_j;
	Uint32 pixel;
	Uint8 r,g,b;

	for(int y = 0; y < height; ++y)
	{
		for(int x = 0; x < width; ++x)
		{
			pixel = getpixel(surface, x, y);
			SDL_GetRGB(pixel, surface->format, &r, &g, &b);
			if (r > 0 && g > 0 && b > 0)
			{
				int j = x;
				pixel_j = getpixel(surface, j, y);
				while(r > 0 && g > 0 && b > 0 && j < width)
				{
					pixel_j = getpixel(surface, j, y);
					SDL_GetRGB(pixel_j, surface->format, &r, &g, &b);
					++j;
				}
				r = 0;
				g = 0;
				b = 0;
				pixel = SDL_MapRGB(surface->format, r, g, b);
				if(j - x < offset && j < width - 1)
				{
					while(x < j && x < width - 1)
					{
						putpixel(surface, x, y, pixel);
						++x;
					}
					if(j < width - 1)
					{
						putpixel(surface, j, y, pixel);
					}
				}
			}
		}
	}
	SDL_UnlockSurface(surface);
	return surface;
}


SDL_Surface* Ytraversal(SDL_Surface *surface, int width, int height, int offset)
{
	SDL_LockSurface(surface);
	Uint32 pixel_j;
	Uint32 pixel;
	Uint8 r, g, b;

	for(int x = 0; x < width; ++x)
	{
		for(int y = 0; y < height; ++y)
		{
			pixel = getpixel(surface, x, y);
			SDL_GetRGB(pixel, surface->format, &r, &g, &b);
			if (r > 0 && g > 0 && b > 0)
			{
				int j = y;
				pixel_j = getpixel(surface, x, j);
				while(j < height && r > 0 && g > 0 && b > 0 )
				{
					pixel_j = getpixel(surface, x, j);
					SDL_GetRGB(pixel_j, surface->format, &r, &g, &b);
					++j;
				}
				r = 0;
				g = 0;
				b = 0;
				pixel = SDL_MapRGB(surface->format, r, g, b);
				if(j - y < offset /*&& j < height - 1*/)
				{
					while(y < j)
					{
						putpixel(surface, x, y, pixel);
						++y;
					}
					putpixel(surface, x, j - 1, pixel);
				}
			}
		}
	}

	SDL_UnlockSurface(surface);
	return surface;
}

void merge_bitmap(SDL_Surface *bm1, SDL_Surface *bm2, SDL_Surface *bm3)
{
	Uint8 r, g, b, r2, g2, b2;
	int width = bm1->w;
	int height = bm1->h;
	SDL_LockSurface(bm1);
	SDL_LockSurface(bm2);
	//SDL_LockSurface(bm3);

	for(int x = 0; x < width; ++x)
	{
		for(int y = 0; y < height; ++y)
		{
			Uint32 pix1 = getpixel(bm1, x, y);
			Uint32 pix2 = getpixel(bm2, x, y);
			SDL_GetRGB(pix1, bm1->format, &r, &g, &b);
			SDL_GetRGB(pix2, bm2->format, &r2, &g2, &b2);
			if ((r == 0 && g == 0 && b == 0) && (r2 == 0 && g2 == 0 && b2 == 0))
			{
				Uint32 pix3 = getpixel(bm3, x, y);
				SDL_GetRGB(pix3, bm3->format, &r, &g, &b);
				r = 0;
				g = 0;
				b = 0;
				pix3 = SDL_MapRGB(bm3->format, r, g, b);
				putpixel(bm3, x, y, pix3);
			}
			else
			{
				Uint32 pix3 = getpixel(bm3, x, y);
				SDL_GetRGB(pix3, bm3->format, &r, &g, &b);
				r = 255;
				g = 255;
				b = 255;
				pix3 = SDL_MapRGB(bm3->format, r, g, b);
				putpixel(bm3, x, y, pix3);
			}
		}
	}
	//SDL_UnlockSurface(bm3);
	SDL_UnlockSurface(bm2);
	SDL_UnlockSurface(bm1);
}


void text_detection(char *path, struct queue *queue)
{

	SDL_Init(SDL_INIT_EVERYTHING);
	/*SDL_Surface *screen = SDL_SetVideoMode(640, 490, 32, SDL_SWSURFACE);
	*/

	SDL_Surface *image;
	SDL_Surface *image2;
	image = SDL_LoadBMP(path);
	image2 = SDL_LoadBMP(path);

	/*SDL_BlitSurface(image, NULL, screen, NULL);
	  SDL_Flip(screen);*/

	SDL_Surface *bitmap1 = SDL_LoadBMP(path);
	SDL_Surface *bitmap2 = SDL_LoadBMP(path);
	SDL_Surface *bitmap3 = bitmap2;

	int offset_x = bitmap1->w - 1;
	int offset_y = bitmap1->h - 1;

	bitmap1 = Xtraversal(bitmap1, bitmap1->w, bitmap1->h, offset_x);
	bitmap2 = Ytraversal(bitmap2, bitmap2->w, bitmap2->h, offset_y);
	//SDL_SaveBMP(bitmap1, "file.bmp");
	//SDL_SaveBMP(bitmap2, "file.bmp");

	merge_bitmap(bitmap1, bitmap2, bitmap3);
	SDL_SaveBMP(bitmap3, "file.bmp");

	/*image2 = SDL_LoadBMP("file.bmp");
	  SDL_BlitSurface(image2, NULL, screen, NULL);
	  SDL_Flip(screen);
	  SDL_Delay(5000);*/
	
	FindText(bitmap3, image);
	SDL_SaveBMP(image, "file2.bmp");
	Detection(image, image2, queue);
	puts("Rigi");
	SDL_SaveBMP(image2, "file.bmp");
	//SDL_SaveBMP(bitmap3, "file2.bmp");
	printf("Saved\n");

	//SDL_FreeSurface(image);
	//SDL_FreeSurface(bitmap1);
	//SDL_FreeSurface(bitmap2);
	//SDL_FreeSurface(bitmap3);

	//image = SDL_LoadBMP("file.bmp");
	/*SDL_BlitSurface(image, NULL, screen, NULL);
	  SDL_Flip(screen);
	  SDL_Delay(5000);*/

	SDL_Quit();
	//return charQueue;
}

