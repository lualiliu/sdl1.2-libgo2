#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <drm/drm_fourcc.h>

#include "draw.h"
#include "display.h"

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

#define HW_LCD_WIDTH 480
#define HW_LCD_HEIGHT 320

static bool is_init = false;
static uint8_t frame_index = 0;
static volatile bool is_double_buffer = true;
static int32_t text_bg_color = -1;
static bool is_requested = false;

static uint32_t fps_pre_time;
static uint32_t fps_time;
static uint32_t fps_count = 0;

static uint16_t *p_draw_frame_buf = NULL;
static uint16_t frame_buffer[2][HW_LCD_WIDTH * HW_LCD_HEIGHT];





static go2_display_t  *display;
static go2_presenter_t*presenter;
static go2_surface_t  *fbsurface;



void SwapFrameBuffer(void);


bool Init(void)
{
  is_init = true;



  for (int i=0; i<HW_LCD_WIDTH*HW_LCD_HEIGHT; i++)
  {
    frame_buffer[0][i] = 0x0000;
    frame_buffer[1][i] = 0x0000;
  }

  if (is_double_buffer == true)
  {
    p_draw_frame_buf = frame_buffer[frame_index ^ 1];
  }
  else
  {
    p_draw_frame_buf = frame_buffer[frame_index];
  }

  display = go2_display_create();
  presenter = go2_presenter_create(display, DRM_FORMAT_RGB565, 0xff080808);
  fbsurface = go2_surface_create(display, HW_LCD_WIDTH, HW_LCD_HEIGHT, DRM_FORMAT_RGB565);


  Clear(0x0000);
  return true;
}

bool IsInit(void)
{
  return is_init;
}

void Reset(void)
{

}


uint32_t ReadPixel(uint16_t x_pos, uint16_t y_pos)
{
  return p_draw_frame_buf[y_pos * HW_LCD_WIDTH + x_pos];
}

void DrawPixel(uint16_t x_pos, uint16_t y_pos, uint32_t rgb_code)
{
  p_draw_frame_buf[y_pos * HW_LCD_WIDTH + x_pos] = rgb_code;
}

void Clear(uint32_t rgb_code)
{
  ClearBuffer(rgb_code);

  UpdateDraw();
}

void ClearBuffer(uint32_t rgb_code)
{
  uint16_t *p_dst = GetFrameBuffer();

  for (int i=0; i<GetWidth()*GetHeight(); i++)
  {
    p_dst[i] = rgb_code;
  }
}

void SetDoubleBuffer(bool enable)
{
  is_double_buffer = enable;

  if (enable == true)
  {
    p_draw_frame_buf = frame_buffer[frame_index^1];
  }
  else
  {
    p_draw_frame_buf = frame_buffer[frame_index];
  }
}

bool GetDoubleBuffer(void)
{
  return is_double_buffer;
}

bool DrawAvailable(void)
{
  //return ili9225DrawAvailable();
  return true;
}

void RequestDraw(void)
{
  SwapFrameBuffer();

  is_requested = true;

  UpdateDraw();
}

void UpdateDraw(void)
{
  if (is_requested != true)
  {
    RequestDraw();
  }


  uint16_t* dst = (uint16_t*)go2_surface_map(fbsurface);


  memcpy(dst, frame_buffer[frame_index], HW_LCD_WIDTH*HW_LCD_HEIGHT*2);

  go2_presenter_post( presenter,
                      fbsurface,
                      0, 0, HW_LCD_WIDTH, HW_LCD_HEIGHT,
                      0, 0, HW_LCD_HEIGHT, HW_LCD_WIDTH,
                      GO2_ROTATION_DEGREES_270);

  //while(lcdDrawAvailable() != true);
}

void SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  
}

void SwapFrameBuffer(void)
{
  if (is_double_buffer == true)
  {
    frame_index ^= 1;
    p_draw_frame_buf = frame_buffer[frame_index ^ 1];
  }
  else
  {
    p_draw_frame_buf = frame_buffer[frame_index];
  }
}

uint16_t *GetFrameBuffer(void)
{
  return p_draw_frame_buf;
}

uint16_t *GetCurrentFrameBuffer(void)
{
  return frame_buffer[frame_index];;
}


int32_t GetWidth(void)
{
  return HW_LCD_WIDTH;
}

int32_t GetHeight(void)
{
  return HW_LCD_HEIGHT;
}
void DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color)
{
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep)
  {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1)
  {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++)
  {
    if (steep)
    {
      DrawPixel(y0, x0, color);
    } else
    {
      DrawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

void DrawVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  DrawLine(x, y, x, y+h-1, color);
}

void DrawHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  DrawLine(x, y, x+w-1, y, color);
}

void DrawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  for (int16_t i=x; i<x+w; i++)
  {
    DrawVLine(i, y, h, color);
  }
}

void DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  DrawHLine(x, y, w, color);
  DrawHLine(x, y+h-1, w, color);
  DrawVLine(x, y, h, color);
  DrawVLine(x+w-1, y, h, color);
}

void DrawFillScreen(uint16_t color)
{
  DrawFillRect(0, 0, HW_LCD_WIDTH, HW_LCD_HEIGHT, color);
}

void SetBgColor(int32_t bg_color)
{
  text_bg_color = bg_color;
}

void SDL_copyframebuffer_Go2(void* src){
	memcpy(p_draw_frame_buf, src, HW_LCD_WIDTH*HW_LCD_HEIGHT*2);
}
