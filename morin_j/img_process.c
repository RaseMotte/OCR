/* ----------------------------------------
 * File Name : img_process.c
 * Purpose : IMG SEGMENTATION
 ------------------------------------------*/

#include "img_process.h"
#include <err.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
//#include "neural_net.h"


/*SDL_Surface *SurfaceToScreenFormat(SDL_Surface *img){
  double zoomx,zoomy;
  zoomx = (1920.0/img -> w);
  zoomy = (1080.0/img -> h);
  img = rotozoomSurfaceXY(img, 0.0,  zoomx,  zoomy,1.0);
  return img;
  }	*/

void wait_for_keypressed()
{
	SDL_Event event;
	// Infinite loop, waiting for event
	for (;;)
	{
		// Take an event
		SDL_PollEvent(&event);
		// Switch on event type
		switch (event.type)
		{
			// Someone pressed a key -> leave the function
			case SDL_KEYDOWN:
				return;

			default: break;
		}
		// Loop until we got the expected event
	}
}

SDL_Surface 	*image_binarize(char *image_name)
{
//	init_sdl();
	
	SDL_Surface *img;
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 average;
	Uint32 cur_pixel; 

	img = load_image(image_name);
	//display_image(img);
	//display_image(img);
	for (int y = 0; y < 28 ; y ++) //img->h
	{
		for (int x = 0; x < 28; x++) //img->w
		{
			cur_pixel = getpixel(img, x, y);
			SDL_GetRGB(cur_pixel, img->format, &r, &g, &b);
			average = (float)r * 0.3 + (float)g * 0.59 + (float)b * 0.11; 
			if (average <= 128)
			{
				average=0;
			}
			else
			{
				average = 255;
			}
			r = average;
			g = average;
			b = average;
			cur_pixel = SDL_MapRGB(img->format, r, g, b);
			putpixel(img, x, y, cur_pixel);
		}

	}
	//SDL_SaveBMP(img, "binarized.bmp");
	//display_image(img);
	return img;
	//SDL_FreeSurface(img);

}

SDL_Surface *load_image(char *file_name)
{
	SDL_Surface          *img;
	// Load an image using SDL_image with format detection
	img = IMG_Load(file_name);
	if (!img)
		// If it fails, die with an error message
		errx(3, "can't load %s: %s", file_name, IMG_GetError());
	return img;
}

SDL_Surface *display_image(SDL_Surface *img)
{
	SDL_Surface          *screen;
	// Set the window to the same size as the image
	screen = SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE|SDL_ANYFORMAT);
	if (screen == NULL)
	{
		// error management
		errx(1, "Couldn't set %dx%d video mode: %s\n",
				img->w, img->h, SDL_GetError());
	}

	/* Blit onto the screen surface */
	if(SDL_BlitSurface(img, NULL, screen, NULL) < 0)
		warnx("BlitSurface error: %s\n", SDL_GetError());

	// Update the screen
	SDL_UpdateRect(screen, 0, 0, img->w, img->h);

	// wait for a key
	wait_for_keypressed();

	// return the screen for further uses
	return screen;
}

	static inline
Uint8* pixelref(SDL_Surface *surf, unsigned x, unsigned y)
{
	int bpp = surf->format->BytesPerPixel;
	return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}

Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y)
{
	Uint8 *p = pixelref(surface, x, y);
	switch(surface->format->BytesPerPixel)
	{
		case 1:
			return *p;
		case 2:
			return *(Uint16 *)p;
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;
		case 4:
			return *(Uint32 *)p;
	}
	return 0;
}

void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel)
{
	Uint8 *p = pixelref(surface, x, y);
	switch(surface->format->BytesPerPixel)
	{
		case 1:
			*p = pixel;
			break;
		case 2:
			*(Uint16 *)p = pixel;
			break;
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			}
			else
			{
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
			break;
		case 4:
			*(Uint32 *)p = pixel;
			break;
	}
}

void grayscale_image(SDL_Surface *img)
{
	Uint8 r, g, b;

	for(int y = 0; y < img->h; ++y)
		for(int x = 0; x < img->w; ++x)
		{
			Uint32 pixel = getpixel(img, (unsigned int)x, (unsigned int)y);
			SDL_GetRGB(pixel, img->format, &r, &g, &b);
			Uint8 rgb = 0.3 * (float)r + 0.59 * (float)g + 0.11 * (float)b;
			putpixel(img, x, y, SDL_MapRGB(img->format, rgb, rgb, rgb));
		}
}

void binarize_image(SDL_Surface *img)
{
	grayscale_image(img);
	Uint8 minVal = 127, maxVal = 128;

	Uint8 pix;

	for(int y = 0; y < img->h; ++y)
	  for(int x = 0; x < img->w; ++x)
	  {
	  Uint32 pixel = getpixel(img, (unsigned int)x, (unsigned int)y);
	  SDL_GetRGB(pixel, img->format, &pix, &pix, &pix);

	  if(pix < minVal)
	  minVal = pix;

	  if(pix > maxVal)
	  maxVal = pix;
	  }

	Uint8 mid = (minVal + maxVal) / 2;
	for(int y = 0; y < img->h; ++y)
		for(int x = 0; x < img->w; ++x)
		{
			Uint32 pixel = getpixel(img, (unsigned int)x, (unsigned int)y);
			SDL_GetRGB(pixel, img->format, &pix, &pix, &pix);
			Uint8 nPix = (pix > mid) * 255;
			putpixel(img, x, y, SDL_MapRGB(img->format, nPix, nPix, nPix));
		}
}

void RGBtoYUV(SDL_Surface *img)
{
	Uint8 r, g, b, Y, U, V;

	for(int y = 0; y < img->h; ++y)
		for(int x = 0; x < img->w; ++x)
		{
			Uint32 pixel = getpixel(img, (unsigned int)x, (unsigned int)y);
			SDL_GetRGB(pixel, img->format, &r, &g, &b);
			Y = 0.3 * (float)r + 0.59 * (float)g + 0.11 * (float)b;
			U = 0.492 * (float)(b-Y);
			V = 0.877 * (float)(r-Y);

			putpixel(img, x, y, SDL_MapRGB(img->format, Y, U, V));
			//Uint8 rgb = 0.3 * (float)Y + 0.59 * (float)U + 0.11 * (float)V;
			//putpixel(img, x, y, SDL_MapRGB(img->format, rgb, rgb, rgb));
		}
}

Uint8 maxUnit8(Uint8 x,Uint8 y,Uint8 z)
{
	Uint8 max= x;
	if(y>max)
		max = y;
	if(z>max)
		max = z;
	return max;
}

SDL_Surface* copySurface(SDL_Surface *surf)
{
	SDL_Surface *res;
	res = SDL_CreateRGBSurface(surf->flags, surf->w, surf->h,
			surf->format->BitsPerPixel, surf->format->Rmask,
			surf->format->Gmask, surf->format->Bmask,
			surf->format->Amask);
	if(res != NULL) {
		SDL_BlitSurface(surf, NULL, res, NULL);
	}
	return res;
}

void generateEdgeImage(SDL_Surface *img)
{
	SDL_Surface *edgeImg = SDL_CreateRGBSurface(SDL_SWSURFACE, img->w, img->h, (img->format->BytesPerPixel*8), 0, 0, 0, 0);
	Uint8 pix1, pix2, pix3, pix4, left, upper, rightUpper ,maxV,empty;
	empty=0;

	for(int y = 0; y < img->h; ++y)
		for(int x = 0; x < img->w; ++x)
		{
			if(0 < x &&  x < img->w && 0 < y && y < img->h)
			{
				Uint32 pixel1 = getpixel(img, (unsigned int)x, (unsigned int)y);
				SDL_GetRGB(pixel1, img->format, &pix1, &pix1, &pix1);

				Uint32 pixel2 = getpixel(img, (unsigned int)x-1, (unsigned int)y);
				SDL_GetRGB(pixel2, img->format, &pix2, &pix2, &pix2);

				Uint32 pixel3 = getpixel(img, (unsigned int)x, (unsigned int)y-1);
				SDL_GetRGB(pixel3, img->format, &pix3, &pix3, &pix3);

				Uint32 pixel4 = getpixel(img, (unsigned int)x+1, (unsigned int)y-1);
				SDL_GetRGB(pixel4, img->format, &pix4, &pix4, &pix4);

				left = abs(pix1 - pix2);
				upper = abs(pix1 - pix3);
				rightUpper = abs(pix1 - pix4);
				maxV = maxUnit8(left,upper,rightUpper);

				putpixel(edgeImg, x, y, SDL_MapRGB(img->format, maxV, maxV, maxV));

			}
			else
			{
				putpixel(edgeImg, x, y, SDL_MapRGB(img->format, empty, empty, empty));
			}

		}
	*img = *edgeImg;
}

TextRegion* detectTextRegions(SDL_Surface *img)
{
	SDL_Surface* img2 = copySurface(img);
	binarize_image(img2);
	//display_image(img2);
	generateEdgeImage(img);
	Uint8 r, g, b;
	TextRegion *TextR = malloc( 1 * sizeof(TextRegion));

	int *H = malloc(img->h*sizeof(int32_t));
	for(int i = 0; i < img->h; ++i)
		H[i] = 0;

	for(int y = 0; y < img->h; ++y)
		for(int x = 0; x < img->w; ++x)
		{
			Uint32 pixel = getpixel(img, (unsigned int)x, (unsigned int)y);
			SDL_GetRGB(pixel, img->format, &r, &g, &b);
			if(r > 100)
				H[y]++;
		}

	TextR = determineYCoordinate(H,TextR,img);
	determineXCoordinate(TextR,img);
	TextR = detectWordRegions(TextR,img);
	TextR = detectLetterRegions(TextR,img2);
	//free(H);
	return TextR;
}

void printList(int *list, int size )
{
	int i=0;
	printf("Liste : \n ");
	while (i<size) {
		printf("%d ", list[i]);
		i++;
	}
	printf("\n");
}

TextRegion* determineYCoordinate(int *H,TextRegion *TextR, SDL_Surface *img)
{

	// ---------- DETECTION Y AXIS ----------
	int MinEdges = 2;
	int MinLineDiff = 2;
	int MinLines = 12;
	int insideTextArea = 0;
	// ---------------------------------------------
	int len = img->h;

	int nb=0;
	int y0=0,y1=0;

	for(int i = 1; i < len ; i++)
	{
		if(H[i] > MinEdges || (H[i]-H[i-1]) > MinLineDiff)
		{
			if ( insideTextArea == 0)
			{
				y0= i;
				insideTextArea = 1;
			}
		}
		else if(insideTextArea == 1)
		{
			y1 = i-1;
			if((y1 - y0) > MinLines)
			{
				TextR[nb].y0 = y0;
				TextR[nb].y1 = y1;
				nb++;
				TextR = realloc(TextR,(nb+1)*sizeof(TextRegion));
			}
			insideTextArea = 0;
		}
	}
	TextR[0].size = nb;
	return TextR;
}


// ---------- DETECTION X AXIS ----------

Uint8 minVal=70;

// ----------------------------------------------


void determineXCoordinate(TextRegion *TextR, SDL_Surface *img)
{
	Uint8  r,g, b;
	int size_of_TR = TextR[0].size;
	for(int i=0;i<size_of_TR;i++)
	{
		int left = img->w + 1;
		int right = -1;

		if(TextR[i].y0 == TextR[i].y1)
			return;

		for(int x=0; x < img->w; x++)
			for(int y = TextR[i].y0; y < TextR[i].y1; y++)
			{
				Uint32 pixel = getpixel(img, (unsigned int)x, (unsigned int)y);
				SDL_GetRGB(pixel, img->format, &r, &g, &b);

				if(r > minVal)
				{
					if(left>x)
						left = x;

					if(right < x)
						right = x;
				}
			}
		TextR[i].x0 = left;
		TextR[i].x1 = right;
	}
}

void SegmentTextRegions(TextRegion *TextR ,SDL_Surface *img,int type)
{
	int size_of_TR = TextR[0].size;

	int x0,y0,x1,y1, i=0;
	int j, l;
	int size_of_WRD;
	int size_of_LTR;
	if(type <0)
		return;
	while(i<size_of_TR)
	{
		if(type == 0){
			x0=TextR[i].x0;
			y0=TextR[i].y0;
			x1=TextR[i].x1;
			y1=TextR[i].y1;

			while(x0<=x1)
			{
				putpixel(img, x0, y0, SDL_MapRGB(img->format, 250, 0, 0));
				putpixel(img, x0, y1, SDL_MapRGB(img->format, 250, 0, 0));
				x0++;
			}
			x0=TextR[i].x0;
			while(y0<=y1)
			{
				putpixel(img, x0, y0, SDL_MapRGB(img->format, 250, 0, 0));
				putpixel(img, x1, y0, SDL_MapRGB(img->format, 250, 0, 0));
				y0++;
			}
		}
		if(type == 1){

			size_of_WRD= TextR[i].Word[0].size;
			j=0;

			while(j<size_of_WRD)
			{
				x0=TextR[i].Word[j].x0;
				y0=TextR[i].Word[j].y0;
				x1=TextR[i].Word[j].x1;
				y1=TextR[i].Word[j].y1;

				while(x0<=x1)
				{
					putpixel(img, x0, y0, SDL_MapRGB(img->format, 0, 0, 0));
					putpixel(img, x0, y1, SDL_MapRGB(img->format, 0, 0, 0));
					x0++;
				}

				x0=TextR[i].Word[j].x0;
				while(y0<=y1)
				{
					putpixel(img, x0, y0, SDL_MapRGB(img->format, 0, 0, 0));
					putpixel(img, x1, y0, SDL_MapRGB(img->format, 0, 0, 0));
					y0++;
				}
				j++;
			}
		}
		if (type == 2)
		{
			size_of_WRD= TextR[i].Word[0].size;
			j=0;

			while(j<size_of_WRD)
			{
				x0=TextR[i].Word[j].x0;
				y0=TextR[i].Word[j].y0;
				x1=TextR[i].Word[j].x1;
				y1=TextR[i].Word[j].y1;

				size_of_LTR = TextR[i].Word[j].Letter[0].size;
				l = 0;
				while (l < size_of_LTR)
				{
					x0=TextR[i].Word[j].Letter[l].x0;
					y0=TextR[i].Word[j].Letter[l].y0;
					x1=TextR[i].Word[j].Letter[l].x1;
					y1=TextR[i].Word[j].Letter[l].y1;

					while(x0<=x1)
					{
						putpixel(img, x0, y0, SDL_MapRGB(img->format, 0, 0, 0));
						putpixel(img, x0, y1, SDL_MapRGB(img->format, 0, 0, 0));
						x0++;
					}

					x0=TextR[i].Word[j].Letter[l].x0;
					while(y0<=y1)
					{
						putpixel(img, x0, y0, SDL_MapRGB(img->format, 0, 0, 0));
						putpixel(img, x1, y0, SDL_MapRGB(img->format, 0, 0, 0));
						y0++;
					}
					l++;
				}
				j++;
			}
		}
		i++;
	}
}

TextRegion* detectWordRegions(TextRegion *TextR,SDL_Surface *img)
{

	// ---------- DETECTION X AXIS ----------
	int MinEdges = 0;
	int BlackCountMax = 6;
	int insideTextArea = 0;
	int BlackCount;
	// ---------------------------------------------

	Uint8 r,g,b;
	int size_of_TR = TextR[0].size;
	int x0,x1,y0,y1;

	int nb;
	int a0=0,a1=0;

	for(int i=0; i<size_of_TR;i++)
	{
		x0=TextR[i].x0;
		y0=TextR[i].y0;
		x1=TextR[i].x1;
		y1=TextR[i].y1;

		int *H = malloc((x1-x0+1)*sizeof(int32_t));
		for(int i = 0; i < (x1-x0); ++i)
			H[i] = 0;

		for(int x = x0; x <= x1 ; ++x)
			for(int y = y0; y<= y1; ++y)
			{
				Uint32 pixel = getpixel(img, (unsigned int)x, (unsigned int)y);
				SDL_GetRGB(pixel, img->format, &r, &g, &b);
				if(r > 10)
					H[x-x0]++;
			}

		_Word *Word=malloc(1*sizeof(_Word));

		nb=0;
		BlackCount=0;
		insideTextArea = 0;
		for(int j = x0; j <= x1 ; j++)
		{
			if( j == x1)
			{
				Word[nb].x0 = a0;
				Word[nb].y0 = y0;
				Word[nb].x1 = j;
				Word[nb].y1 = y1;
				nb++;
				Word = realloc(Word,(nb+1)*sizeof(_Word));
				BlackCount = 0;
				insideTextArea = 0;
			}

			else if(H[j-x0] > MinEdges)
			{
				BlackCount = 0;
				if ( insideTextArea == 0)
				{
					a0= j;
					insideTextArea = 1;
				}
			}
			else if(insideTextArea == 1)
			{
				a1 = j;
				BlackCount++;
				if(BlackCount == BlackCountMax)
				{
					Word[nb].x0 = a0;
					Word[nb].y0 = y0;
					Word[nb].x1 = a1;
					Word[nb].y1 = y1;
					nb++;
					Word = realloc(Word,(nb+1)*sizeof(_Word));
					BlackCount = 0;
					insideTextArea = 0;
					a0=a1;
				}
			}
		}
		Word[0].size = nb;
		TextR[i].Word = Word;
		//free(H);
	}
	return TextR;
}

TextRegion* detectLetterRegions(TextRegion *TextR,SDL_Surface *img)
{

	// ---------- DETECTION X AXIS ----------
	int MinEdges = 0;
	int BlackCountMax = 1;
	int insideTextArea = 0;
	int BlackCount = 0;
	// ---------------------------------------------

	Uint8 r,g,b;
	int size_of_TR = TextR[0].size;
	int size_of_WRD;
	int x0,x1,y0,y1;

	int nb = 0;
	int a0=0,a1=0;

	for(int i=0; i<size_of_TR;i++)
	{
		size_of_WRD = TextR[i].Word[0].size;
		for(int w=0; w < size_of_WRD; w++)
		{

			x0=TextR[i].Word[w].x0;
			y0=TextR[i].Word[w].y0;
			x1=TextR[i].Word[w].x1;
			y1=TextR[i].Word[w].y1;

			int *H = malloc((x1-x0+1)*sizeof(int32_t));
			for(int l = 0; l <= (x1-x0); ++l)
				H[l] = 0;

			for(int x = x0; x <= x1 ; ++x)
				for(int y = y0; y<= y1; ++y)
				{
					Uint32 pixel = getpixel(img, (unsigned int)x, (unsigned int)y);
					SDL_GetRGB(pixel, img->format, &r, &g, &b);
					if(r < 100)
						H[x-x0]++;
				}

			_Letter *Letter=malloc(1*sizeof(_Letter));

			nb=0;
			BlackCount=0;
			insideTextArea = 0;
			for(int j = x0; j <= x1 ; j++)
			{
				if( j == x1)
				{
					Letter[nb].x0 = a0;
					Letter[nb].y0 = y0;
					Letter[nb].x1 = j - 1;
					Letter[nb].y1 = y1;
					Letter[nb] = deleteWhite(Letter[nb], img);
					if (!(Letter[nb].x0 == -1))
					{
						Letter[nb].Matrix = getMatrixLetter(Letter[nb], img);
						nb++;
						Letter = realloc(Letter, (nb + 1) * sizeof (_Letter));
					}
					BlackCount = 0;
					insideTextArea = 0;
				}

				else if(H[j-x0] > MinEdges)
				{
					BlackCount = 0;
					if ( insideTextArea == 0)
					{
						a0= j;
						insideTextArea = 1;
					}
				}
				else if(insideTextArea == 1)
				{
					a1 = j;
					BlackCount++;
					if(BlackCount == BlackCountMax)
					{
						Letter[nb].x0 = a0;
						Letter[nb].y0 = y0;
						Letter[nb].x1 = a1 - 1;
						Letter[nb].y1 = y1;
						Letter[nb] = deleteWhite(Letter[nb], img);
						if (!(Letter[nb].x0 == -1))
						{
							Letter[nb].Matrix = getMatrixLetter(Letter[nb], img);
							nb++;
							Letter = realloc(Letter, (nb + 1) * sizeof (_Letter));
						}
						BlackCount = 0;
						insideTextArea = 0;
						a0=a1;
					}
				}
			}
			Letter[0].size = nb;
			TextR[i].Word[w].Letter = Letter;
			//free(H);
		}
	}
	return TextR;
}

int i = 0;
_Letter deleteWhite(_Letter Letter, SDL_Surface *img)
{
	int *H = histoLetter(Letter, img);
	if (H[0] == -1)
	{
		Letter.x0 = -1;
	}
	int y0 = Letter.y0, y1 = Letter.y1;
	int y = y0;
	while (y <= y1 && H[y - y0] == 0)
	{
		y++;
	}
	Letter.y0 = y;
	y = y1;
	while (y >= y0 && H[y - y0] == 0)
	{
		y--;
	}
	Letter.y1 = y;
	//free(H);
	//printf("Let ");
	int *matLet = getMatrixLetter(Letter, img);
	if (!matLet)
	{
		addSpace();
	}
	else
	{
		double *matLetd = (double *) malloc(28 * 28 * sizeof(double));
		for (int i = 0; i < 28 * 28; i++)
		{
			matLetd[i] = (double) matLet[i];
			/*if (i % 28 == 0)
			  printf("\n");

			if (matLet[i])
				printf("%d ", matLet[i]);
			else
				printf("  ");*/
		}

		//dump(matLet);
		char let = bpnn_recognition(matLetd);
		//Letter._char = let;
		//printf("%c", let);
	}

	return Letter;
}

int* histoLetter(_Letter Letter, SDL_Surface *img)
{
	int x0 = Letter.x0;
	int y0 = Letter.y0;
	int x1 = Letter.x1;
	int y1 = Letter.y1;
	int *H = calloc((y1 - y0 + 1), sizeof(int));
	int notEmpty = 0;
	Uint8 r, g, b;
	Uint32 pixel;

	for (int i = 0; i < y1 - y0; ++i)
	{
		*(H + i) = 0;
	}

	for (int y = y0; y <= y1; ++y)
	{
		for (int x = x0; x <= x1; ++x)
		{
			pixel = getpixel(img, (unsigned int)x, (unsigned int)y);
			SDL_GetRGB(pixel, img -> format, &r, &g, &b);
			if (r + g + b < 20)
			{
				H[y - y0] += 1;
				notEmpty++;
			}
		}
	}

	if (!notEmpty)
	{
		H[0] = -1;
	}
	return H;
}

int* getMatrixLetter(_Letter Letter, SDL_Surface *img)
{
	int x0 = Letter.x0, x1 = Letter.x1, y0 = Letter.y0, y1 = Letter.y1;
	int columns = x1 - x0 + 1, lines = y1 - y0 + 1;
	//printf("col %d, ln %d\n", columns, lines);
	if (lines <= 5)
		return NULL;
	int **Matrix = (int**)malloc(lines * sizeof (int*));
	Uint32 pixel;
	Uint8 r, g, b;
	int value;
	for (int i = 0; i < lines; i++)
	{
		Matrix[i] = (int*)malloc(columns * sizeof (int));
		for (int j = 0; j < columns; j++)
		{
			pixel = getpixel(img, j + x0, i + y0);
			SDL_GetRGB(pixel, img -> format, &r, &g, &b);
			value = r < 64;
			//printf(" %d", value);
			Matrix[i][j] = value;
		}
		//printf("\n");
	}
	//printf("\n");
	//printMatrix(Matrix, lines, columns);
	return resizeMatrix(Matrix, lines, columns);
}

void printMatrix(int **Matrix, int lines, int columns)
{
	float scaleY = (float)lines / 28, scaleX = (float)columns / 28;
	int in, jn;
	for (int i = 0; i < 28; i++)
	{
		for (int j = 0; j < 28; j++)
		{
			in = i * scaleY;
			jn = j * scaleX;
			printf(" %d", Matrix[in][jn]);
		}
		printf("\n");
	}
	printf("\n");
}

// Resizes into a 16x16 matrix then into a 256 int array;
int* resizeMatrix(int **Matrix, int lines, int columns)
{
	float scaleY = (float)lines / 28, scaleX = (float)columns / 28;
	int* array = (int*)malloc(784 * sizeof (int));
	int in, jn;
	int count = 0;
	for (int i = 0; i < 28; i++)
	{
		for (int j = 0; j < 28; j++)
		{
			in = i * scaleY;
			jn = j * scaleX;
			array[i * 28 + j] = Matrix[in][jn];
			count++;
			//printf(" %d", array[i * 28 + j]);
			if (count == 28)
			{
				//printf("\n");
				count = 0;
			}
		}
	}
	//printf("\n");
	//freeMatrix(Matrix, lines);
	return array;
}

void freeMatrix(int **Matrix, int lines)
{
	for (int i = 0; i < lines; i++)
	{
		//free(Matrix[i]);
	}
	//free(Matrix);
}
