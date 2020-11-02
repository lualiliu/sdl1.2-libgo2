#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>

#include "display.h"
#include "draw.h"
#include <drm/drm_fourcc.h>

#ifndef SDL_TRIPLEBUF
#define SDL_TRIPLEBUF SDL_DOUBLEBUF
#endif

SDL_Surface* sdl_screen, *img, *img2, *tmp;

int main(void)
{
  Init();
  
  SDL_Init(SDL_INIT_VIDEO);
  SDL_ShowCursor(0);
  sdl_screen = SDL_SetVideoMode(320, 480, 32, SDL_SWSURFACE);
  if (!sdl_screen)
  {
	printf("Can't set video mode\n");
	return 0;
  }
  printf("Video OK ! Width %d, Heigth %d, Bpp %d\n", sdl_screen->w, sdl_screen->h, sdl_screen->format->BytesPerPixel*8);
	
  tmp = SDL_LoadBMP("yes.bmp");
  if (!tmp)
  {
	printf("Could not load yes.bmp image file\n");
	return 0;	
  }
  else
  {
	if (sdl_screen->format->BytesPerPixel == 4) img = SDL_DisplayFormatAlpha(tmp);
	else img = SDL_DisplayFormat(tmp);
	SDL_FreeSurface(tmp);
  }
  while(1)
  {

   if (DrawAvailable() > 0)
    {
      ClearBuffer(0xFE19);
      SDL_BlitSurface(img, NULL, sdl_screen, NULL); //On affiche la rotation de la surface image.
      SDL_copyframebuffer_Go2(sdl_screen->pixels);
      RequestDraw();
    }
  }
}
