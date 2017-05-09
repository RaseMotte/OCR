#ifndef IMG_PROCESS_H
#define IMG_PROCESS_H

#include <SDL/SDL.h>

typedef struct _Letter _Letter;
struct _Letter
{
	int x0;
	int y0;
	int x1;
	int y1;
	int size;

	char _char;

	int *Matrix;
};

typedef struct _Word _Word;
struct _Word
{
	int x0;
	int y0;
	int x1;
	int y1;
	int size;

	_Letter *Letter;

};

typedef struct TextRegion TextRegion;
struct TextRegion
{
	// ZONE
	int x0;
	int y0;
	int x1;
	int y1;
	int size;

	_Word *Word;

	char *_string;


};


SDL_Surface *image_binarize(char *file_name);
SDL_Surface *load_image(char *file_name);
SDL_Surface *display_image(SDL_Surface *img);
void wait_for_keypressed(void);
Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y);
void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);
void grayscale_image(SDL_Surface *img);
void binarize_image(SDL_Surface *img);
void generateEdgeImage(SDL_Surface *img);
void RGBtoYUV(SDL_Surface *img);
TextRegion* detectTextRegions(SDL_Surface *img);
TextRegion* determineYCoordinate(int *H,TextRegion *TextR,SDL_Surface *img);
void determineXCoordinate(TextRegion *TextR, SDL_Surface *img);
void SegmentTextRegions(TextRegion *TextR ,SDL_Surface *img,int type);
void printList(int *list,int size );
SDL_Surface* copySurface(SDL_Surface *surf);
SDL_Surface *SurfaceToScreenFormat(SDL_Surface *img);

_Letter deleteWhite(_Letter Letter, SDL_Surface *img);
int* histoLetter(_Letter Letter, SDL_Surface *img);
TextRegion* detectWordRegions(TextRegion *TextR,SDL_Surface *img);
TextRegion* detectLetterRegions(TextRegion *TextR,SDL_Surface *img);
int* getMatrixLetter(_Letter Letter, SDL_Surface *img);
int* resizeMatrix(int **Matrix, int lines, int columns);
void printMatrix(int **Matrix, int lines, int columns);
void freeMatrix(int **Matrix, int lines);

#endif
