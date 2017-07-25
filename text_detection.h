//text_detection.h

#ifndef TEXT_DETECTION_H_
#define TEXT_DETECTION_H_

#include <stdlib.h>
#include <SDL/SDL.h>

struct list 
{
  struct list *next;
  void *data;
};

struct queue 
{
  struct list *store;
  size_t size;
};

void printImage(SDL_Surface *surface);

void queue_init(struct queue *queue);

void queue_push(struct queue *queue, void *elm);

void FindText(SDL_Surface *surface, SDL_Surface *image);

void Detection(SDL_Surface *surface, SDL_Surface *image, struct queue *queue);

SDL_Surface* GetChar(SDL_Surface *surface, int x, int y, int width, int height);

SDL_Surface* Xtraversal(SDL_Surface *surface, int width, int height, int offset);

SDL_Surface* Ytraversal(SDL_Surface *surface, int width, int height, int offset);

void merge_bitmap(SDL_Surface *bm1, SDL_Surface *bm2, SDL_Surface *bm3);

void text_detection(char *path_to_bmp_img, struct queue *queue);

//Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y);
//void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);

#endif
