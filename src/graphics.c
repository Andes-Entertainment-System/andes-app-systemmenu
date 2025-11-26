#include "graphics.h"

#include <math.h>
#include <string.h>

#include "constants.h"
#include "controls.h"
#include "files.h"
#include "stdarg.h"

float bgMoveFactor;
float fgPosition;
float cool;

float fadeMusicVolume = 0;
uint8_t fadeCurrentPalette[256][3];
uint8_t fadeTargetPalette[256][3];
int8_t fadeDirection = 0;
bool gfxFadeDone = false;

Sprite smallThumbnailFrames[APP_ENTRY_AMOUNT];

Sprite chevronLeft = {
    .flags = {.visible = true, .priority = true, .hFlip = false},
    .set = &RES_spriteset_chevron,
    .position = {.x = 0, .y = GFX_SCREEN_HEIGHT / 2 - 24},
};

Sprite chevronRight = {
    .flags = {.visible = true, .priority = true},
    .set = &RES_spriteset_chevron,
    .position = {.x = GFX_SCREEN_WIDTH - 24, .y = GFX_SCREEN_HEIGHT / 2 - 36},
};

void gfxPrintf(uint32_t x, uint32_t y, const char* format, ...) {
  char str[40];

  va_list args;
  va_start(args, format);

  vsnprintf(str, 40, format, args);

  uint16_t tiles[40];
  memset(tiles, 0, sizeof(tiles));

  for (int i = 0; i < 40; i++) {
    if (str[i] == '\0') break;
    tiles[i] = str[i] + TILESET_FONT_OFFSET;
  }

  TIL_setPlaneRow(TILEPLANE_FG, x, y, tiles, 40);
}

void gfxUpdateFade() {
  if (gfxFadeDone) return;

  gfxFadeDone = true;

  for (int i = 0; i < 256; i++) {
    uint8_t* current = fadeCurrentPalette[i];
    uint8_t* target = fadeTargetPalette[i];

    for (int c = 0; c < 3; c++) {
      if (current[c] == target[c]) continue;

      if (abs((int)current[c] - target[c]) < abs(fadeDirection)) {
        current[c] = target[c];
      } else {
        current[c] += fadeDirection;
        gfxFadeDone = false;
      }
    }
  }

  SFX_setMusicVolume(fadeMusicVolume);
  fadeMusicVolume += (float)fadeDirection / 100;
  if (fadeMusicVolume > 1 || fadeMusicVolume < 0) {
    fadeMusicVolume = fmin(fmax(fadeMusicVolume, 0), 1);
  } else {
    gfxFadeDone = false;
  }

  STO_copyPtrToRegister(REG_PALETTE, 0, fadeCurrentPalette, sizeof(fadeCurrentPalette));
}

void gfxFadeIn() {
  gfxFadeDone = false;
  fadeDirection = 2;
  STO_copyRegisterToPtr(fadeTargetPalette, REG_PALETTE, 0, sizeof(fadeTargetPalette));
  memset(fadeCurrentPalette, 0, sizeof(fadeCurrentPalette));

  STO_copyPtrToRegister(REG_PALETTE, 0, fadeCurrentPalette, sizeof(fadeCurrentPalette));
}

void gfxFadeOut() {
  gfxFadeDone = false;
  fadeDirection = -1;
  STO_copyRegisterToPtr(fadeCurrentPalette, REG_PALETTE, 0, sizeof(fadeCurrentPalette));
  memset(fadeTargetPalette, 0, sizeof(fadeTargetPalette));
}

void gfxUpdateBigFrame() {
  TIL_fillPlaneRect(TILEPLANE_FG, 15, 20, 20, 2, 0);
  gfxPrintf(15, 20, "%s", appEntries[appCursor % APP_ENTRY_AMOUNT].title);
  gfxPrintf(15, 21, "by %s", appEntries[appCursor % APP_ENTRY_AMOUNT].author);

  STO_copyPtrToRegister(REG_PALETTE, THUMBNAIL_PALETTE_OFFSET * 3,
                        appEntries[appCursor % APP_ENTRY_AMOUNT].bigThumbnailPalette,
                        sizeof(appEntries[appCursor % APP_ENTRY_AMOUNT].bigThumbnailPalette));
}

void gfxUpdateSmallFrame(int index) {
  int wrappedIndex = index % APP_ENTRY_AMOUNT;
  STO_copyPtrToRegister(
      REG_PALETTE, (THUMBNAIL_PALETTE_OFFSET + BIG_THUMBNAIL_COLOURS + wrappedIndex * SMALL_THUMBNAIL_COLOURS) * 3,
      appEntries[wrappedIndex].smallThumbnailPalette, sizeof(appEntries[wrappedIndex].smallThumbnailPalette));
}

void gfxBeforeRender() {
  for (int i = 0; i < APP_ENTRY_AMOUNT; i++) {
    float relativeIndex = fmod((-fgPosition + i) / APP_ENTRY_AMOUNT + 0.5 + 1000, 1) - 0.5;
    float angle = relativeIndex * MATH_PI / 1.5;

    int absoluteIndex = round(fgPosition + relativeIndex * APP_ENTRY_AMOUNT);

    Sprite* frame = &smallThumbnailFrames[i];
    frame->flags.visible = absoluteIndex >= 0 && absoluteIndex < appCount;

    frame->position.x = round(cos(angle) * ENTRY_CIRCLE_RADIUS) - ENTRY_CIRCLE_RADIUS + ENTRY_CIRCLE_OFFSET;
    frame->position.y = round(sin(angle) * ENTRY_CIRCLE_RADIUS) + GFX_SCREEN_HEIGHT / 2 - frame->I_source.height / 2;
  }

  TIL_setPlaneScrollXY(TILEPLANE_BG, (int32_t)(sin(bgMoveFactor) * BG_MOVEMENT_RADIUS_X),
                       (int32_t)(cos(bgMoveFactor / 2) * BG_MOVEMENT_RADIUS_Y));

  bgMoveFactor += BG_MOVEMENT_SPEED;
}

void gfxAfterRender() {
  GFX_drawBitmap(appEntries[appCursor % APP_ENTRY_AMOUNT].bigThumbnail, 128, 24, BIG_THUMBNAIL_WIDTH,
                 BIG_THUMBNAIL_HEIGHT);

  for (int i = 0; i < APP_ENTRY_AMOUNT; i++) {
    if (!smallThumbnailFrames[i].flags.visible) continue;

    GFX_drawBitmap(appEntries[i].smallThumbnail, smallThumbnailFrames[i].position.x + 8,
                   smallThumbnailFrames[i].position.y + 8, SMALL_THUMBNAIL_WIDTH, SMALL_THUMBNAIL_HEIGHT);
  }

  fgPosition += ((float)appCursor - fgPosition) * 0.15;
  gfxUpdateFade();
}

void gfxInit() {
  PAL_loadPalette(&RES_palette_main, 0);
  TIL_loadTileSetAt(&RES_tileset_bg, TILESET_BG_OFFSET);
  TIL_loadTileSetAt(&RES_tileset_bigframe, TILESET_BIGFRAME_OFFSET);
  TIL_loadTileSetAt(&RES_tileset_cursor, TILESET_CURSOR_OFFSET);
  TIL_loadTileSetAt(&RES_tileset_font, TILESET_FONT_OFFSET);

  SPR_loadSpriteSet(&RES_spriteset_chevron);
  SPR_loadSpriteSet(&RES_spriteset_smallframe);

  TIL_setPlaneRect(TILEPLANE_FG, 15, 2, BIG_THUMBNAIL_WIDTH / 8 + 2, BIG_THUMBNAIL_HEIGHT / 8 + 2,
                   RES_tileset_bigframe.planeArrangement);

  TIL_setPlaneRect(TILEPLANE_FG, 0, GFX_SCREEN_HEIGHT_TILES / 2 - 4, 13, 8, RES_tileset_cursor.planeArrangement);

  for (int y = 0; y < 64; y += 4) {
    for (int x = 0; x < 64; x += 4) {
      TIL_setPlaneRect(TILEPLANE_BG, x, y, 4, 4, RES_tileset_bg.planeArrangement);
    }
  }

  for (int i = 0; i < APP_ENTRY_AMOUNT; i++) {
    smallThumbnailFrames[i].set = &RES_spriteset_smallframe;
    smallThumbnailFrames[i].flags.visible = true;
    SPR_addSprite(&smallThumbnailFrames[i]);
    SPR_updateSpriteFrame(&smallThumbnailFrames[i]);
  }

  gfxPrintf(15, 26, "           Start App \x01");
  gfxPrintf(15, 27, "   Delete Saved Data \x04");
}