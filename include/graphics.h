#pragma once
#include <andes.h>

void gfxPrintf(uint32_t x, uint32_t y, const char* format, ...);
void gfxUpdateBigFrame();
void gfxUpdateSmallFrame(int index);
void gfxBeforeRender();
void gfxAfterRender();
void gfxInit();

extern bool gfxFadeDone;
void gfxFadeIn();
void gfxFadeOut();