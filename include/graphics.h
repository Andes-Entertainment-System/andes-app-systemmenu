#pragma once
#include <andes.h>

void gfxPrintf(uint32_t x, uint32_t y, const char *format, ...);
void gfxPrevEntry();
void gfxNextEntry();
void gfxUpdateEntry();
void gfxMovePlanes();
void gfxDrawThumbnails();
void gfxInit();