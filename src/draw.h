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

#include "display.h"

#ifdef __cplusplus
extern "C" {
#endif

bool Init(void);
bool IsInit(void);
void Reset(void);

uint8_t GetBackLight(void);
void    SetBackLight(uint8_t value);

uint32_t ReadPixel(uint16_t x_pos, uint16_t y_pos);
void Clear(uint32_t rgb_code);
void ClearBuffer(uint32_t rgb_code);

bool DrawAvailable(void);
void RequestDraw(void);
void UpdateDraw(void);
void SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

void SetBgColor(int32_t bg_color);


int32_t GetWidth(void);
int32_t GetHeight(void);

uint16_t *GetFrameBuffer(void);
uint16_t *GetCurrentFrameBuffer(void);
void SetDoubleBuffer(bool enable);

void DrawPixel(uint16_t x_pos, uint16_t y_pos, uint32_t rgb_code);
void DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color);
void DrawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void DrawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void DrawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void DrawFillScreen(uint16_t color);
void DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void Printf(int x, int y, uint16_t color,  const char *fmt, ...);
uint32_t GetStrWidth(const char *fmt, ...);

void SDL_copyframebuffer_Go2(void* src);

#ifdef __cplusplus
}
#endif
