#pragma once
#include <andes.h>

#include "constants.h"
#include "stdio.h"

struct AppEntry {
  char path[STRING_BUF_SIZE];
  char id[STRING_BUF_SIZE];
  char title[STRING_BUF_SIZE];
  char author[STRING_BUF_SIZE];
  uint8_t smallThumbnailPalette[3 * 32];
  uint8_t smallThumbnail[SMALL_THUMBNAIL_WIDTH * SMALL_THUMBNAIL_HEIGHT];
  uint8_t bigThumbnailPalette[3 * 128];
  uint8_t bigThumbnail[BIG_THUMBNAIL_WIDTH * BIG_THUMBNAIL_HEIGHT];
};

extern char appFilenames[4096][255];
extern struct AppEntry appEntries[5];
extern uint32_t appCount;

bool fsCheckAppValidity(char *path);
bool fsLoadAppEntry(char *path, struct AppEntry *entry);
void fsInit();