#include <andes.h>
#include <dirent.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define BIG_THUMBNAIL_WIDTH 256
#define BIG_THUMBNAIL_HEIGHT 192
#define SMALL_THUMBNAIL_WIDTH 40
#define SMALL_THUMBNAIL_HEIGHT 32

#define ANDES_HEADER_SIZE 13
#define STRING_BUF_SIZE 1024

#define TILESET_BG_OFFSET 1
#define TILESET_BIGFRAME_OFFSET 32
#define TILESET_FONT_OFFSET 8063

struct AppEntry {
  char id[STRING_BUF_SIZE];
  char title[STRING_BUF_SIZE];
  char author[STRING_BUF_SIZE];
  uint8_t smallThumbnailPalette[3 * 32];
  uint8_t smallThumbnail[SMALL_THUMBNAIL_WIDTH * SMALL_THUMBNAIL_HEIGHT];
  uint8_t bigThumbnailPalette[3 * 128];
  uint8_t bigThumbnail[BIG_THUMBNAIL_WIDTH * BIG_THUMBNAIL_HEIGHT];
};

void readMetaItem(FILE *file, void *out) {
  uint32_t itemSize;
  fread(&itemSize, sizeof(itemSize), 1, file);
  fread(out, sizeof(char), itemSize, file);
}

void loadAppEntry(char *path, struct AppEntry *entry) {
  FILE *file = fopen(path, "r");

  fseek(file, ANDES_HEADER_SIZE, SEEK_SET);

  readMetaItem(file, entry->id);
  readMetaItem(file, entry->title);
  readMetaItem(file, entry->author);
  readMetaItem(file, entry->smallThumbnailPalette);
  readMetaItem(file, entry->smallThumbnail);
  readMetaItem(file, entry->bigThumbnailPalette);
  readMetaItem(file, entry->bigThumbnail);

  for (int i = 0; i < sizeof(entry->bigThumbnail); i++) {
    entry->bigThumbnail[i] += 128;
  }
  for (int i = 0; i < sizeof(entry->smallThumbnail); i++) {
    entry->smallThumbnail[i] += 128;
  }
}

float shit;
float ass;

struct AppEntry appEntries[5];

Sprite chevronLeft = {
    .flags = {.visible = true, .priority = true, .hFlip = true},
    .set = &RES_spriteset_chevron,
    .position = {.x = 0, .y = GFX_SCREEN_HEIGHT / 2 - 36},
};

Sprite chevronRight = {
    .flags = {.visible = true, .priority = true},
    .set = &RES_spriteset_chevron,
    .position = {.x = GFX_SCREEN_WIDTH - 24, .y = GFX_SCREEN_HEIGHT / 2 - 36},
};

void gfxPrintf(uint32_t x, uint32_t y, const char *format, ...) {
  char str[40];

  va_list args;
  va_start(args, format);

  vsnprintf(str, 40, format, args);

  uint16_t tiles[40];
  memset(tiles, 0, sizeof(tiles));

  for (int i = 0; i < 40; i++) {
    if (str[i] == 0) break;
    tiles[i] = str[i] + TILESET_FONT_OFFSET;
  }

  TIL_setPlaneRow(TILEPLANE_FG, x, y, tiles, 40);
}

void setup() {
  PAL_loadPalette(&RES_palette_main, 0);
  TIL_loadTileSetAt(&RES_tileset_bg, TILESET_BG_OFFSET);
  TIL_loadTileSetAt(&RES_tileset_bigframe, TILESET_BIGFRAME_OFFSET);
  TIL_loadTileSetAt(&RES_tileset_font, TILESET_FONT_OFFSET);

  SPR_loadSpriteSet(&RES_spriteset_chevron);

  SPR_addSprite(&chevronLeft);
  SPR_updateSpriteFrame(&chevronLeft);

  SPR_addSprite(&chevronRight);
  SPR_updateSpriteFrame(&chevronRight);

  TIL_setPlaneRect(TILEPLANE_FG, 3, 0, BIG_THUMBNAIL_WIDTH / 8 + 2, BIG_THUMBNAIL_HEIGHT / 8 + 3,
                   RES_tileset_bigframe.planeArrangement);

  for (int y = 0; y < 64; y += 4) {
    for (int x = 0; x < 64; x += 4) {
      TIL_setPlaneRect(TILEPLANE_BG, x, y, 4, 4, RES_tileset_bg.planeArrangement);
    }
  }

  DIR *d;
  struct dirent *dir;
  d = opendir(".");
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (dir->d_type != 4) continue;

      loadAppEntry(dir->d_name, &appEntries[0]);

      gfxPrintf(1, 27, "%s", appEntries[0].title);
      gfxPrintf(1, 28, "by %s", appEntries[0].author);

      STO_copyPtrToRegister(REG_PALETTE, 128 * 3, appEntries[0].bigThumbnailPalette,
                            sizeof(appEntries[0].bigThumbnailPalette));
    }
    closedir(d);
  }
}

void process() {
  // put your main loop code here, to run on each frame:
  TIL_setPlaneScrollXY(TILEPLANE_BG, sin(shit) * 40 + 1000, cos(shit / 2) * 40 + 1000);
  uint32_t fuck[64];
  for (int i = 0; i < 64; i++) {
    fuck[i] = round(ass);
  }
  TIL_setPlaneHScrollTable(TILEPLANE_FG, 0, fuck, 27);

  shit += 0.01;
  GFX_render();
  GFX_drawBitmap(appEntries[0].bigThumbnail, -round(ass) + 32 + 512, 12, 256, 192);

  ass = ass + (512.0 - ass) * 0.1;

  if (JOY_getButtonPressed(0, BUTTON_A)) ass = 0;
}