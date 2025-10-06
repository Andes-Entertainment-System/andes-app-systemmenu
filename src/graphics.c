#include "graphics.h"

#include <string.h>

#include "constants.h"
#include "files.h"
#include "math.h"
#include "stdarg.h"

float bgMoveFactor;
float fgPosition;

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
  printf("%s\n", str);

  uint16_t tiles[40];
  memset(tiles, 0, sizeof(tiles));

  for (int i = 0; i < 40; i++) {
    if (str[i] == '\0') break;
    tiles[i] = str[i] + TILESET_FONT_OFFSET;
  }

  TIL_setPlaneRow(TILEPLANE_FG, x, y, tiles, 40);
}

void gfxUpdateEntry() {
  TIL_fillPlaneRect(TILEPLANE_FG, 0, 27, 40, 2, 0);
  gfxPrintf(1, 27, "%s", appEntries[0].title);
  gfxPrintf(1, 28, "by %s", appEntries[0].author);
  STO_copyPtrToRegister(REG_PALETTE, 128 * 3, appEntries[0].bigThumbnailPalette,
                        sizeof(appEntries[0].bigThumbnailPalette));
}

void gfxPrevEntry() {
  gfxUpdateEntry();
  fgPosition = 512;
}

void gfxNextEntry() {
  gfxUpdateEntry();
  fgPosition = -512;
}

void gfxMovePlanes() {
  TIL_fillPlaneHScrollTable(TILEPLANE_FG, 0, round(fgPosition) + 512, 32);
  TIL_setPlaneScrollXY(TILEPLANE_BG, sin(bgMoveFactor) * 40 + 1000, cos(bgMoveFactor / 2) * 40 + 1000);

  bgMoveFactor += 0.01;
}

void gfxDrawThumbnails() {
  GFX_drawBitmap(appEntries[0].bigThumbnail, -round(fgPosition) + 32, 12, BIG_THUMBNAIL_WIDTH, BIG_THUMBNAIL_HEIGHT);

  fgPosition -= fgPosition * 0.12;
}

void gfxInit() {
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
}