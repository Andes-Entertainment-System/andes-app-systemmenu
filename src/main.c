#include <andes.h>
#include <dirent.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "constants.h"
#include "files.h"
#include "graphics.h"

uint32_t appCursor = 0;

bool pressingButton = false;

void setup() {
  fsInit();
  gfxInit();

  fsLoadAppEntry(appFilenames[appCursor], &appEntries[0]);
  gfxUpdateEntry();
}

void process() {
  if (JOY_getButtonJustPressed(0, BUTTON_DPADRIGHT) && appCursor != appCount - 1) {
    appCursor++;
    fsLoadAppEntry(appFilenames[appCursor], &appEntries[0]);
    gfxNextEntry();
  } else if (JOY_getButtonJustPressed(0, BUTTON_DPADLEFT) && appCursor != 0) {
    appCursor--;
    fsLoadAppEntry(appFilenames[appCursor], &appEntries[0]);
    gfxPrevEntry();
  }

  if (JOY_getButtonJustPressed(0, BUTTON_A)) {
    I_SYS_loadApp(appEntries[0].path);
  };

  gfxMovePlanes();
  GFX_render();
  gfxDrawThumbnails();
}