#include <andes.h>
#include <dirent.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "constants.h"
#include "controls.h"
#include "files.h"
#include "graphics.h"

void setup() {
  fsInit();
  gfxInit();

  SFX_loadSound(&RES_sfx_select);
  SFX_loadSound(&RES_sfx_enter);
  SFX_playMusic(&RES_music_main);

  for (int i = 0; i < APP_ENTRY_AMOUNT; i++) {
    fsLoadAppEntry(i);
    gfxUpdateSmallFrame(i);
  }

  gfxUpdateBigFrame();
  gfxFadeIn();
}

void process() {
  controlsProcess();
  gfxBeforeRender();
  GFX_render();
  gfxAfterRender();
}