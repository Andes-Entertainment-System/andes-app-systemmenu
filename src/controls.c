#include "controls.h"

#include "files.h"
#include "graphics.h"

int appCursor = 0;
char* pathToLoadApp = NULL;

void controlsProcess() {
  if (pathToLoadApp != NULL) {
    if (gfxFadeDone) I_SYS_loadApp(appEntries[appCursor % APP_ENTRY_AMOUNT].path);
    return;
  }

  if (JOY_getButtonJustPressed(0, BUTTON_DPADDOWN) && appCursor != appCount - 1) {
    appCursor++;
    int toLoad = appCursor + APP_ENTRY_AMOUNT / 2;
    if (toLoad < appCount) {
      fsLoadAppEntry(toLoad);
      gfxUpdateSmallFrame(toLoad);
    }
    SFX_playSound(0, &RES_sfx_select);
    gfxUpdateBigFrame();
  } else if (JOY_getButtonJustPressed(0, BUTTON_DPADUP) && appCursor != 0) {
    appCursor--;
    int toLoad = appCursor - APP_ENTRY_AMOUNT / 2;
    if (toLoad >= 0) {
      fsLoadAppEntry(toLoad);
      gfxUpdateSmallFrame(toLoad);
    }
    SFX_playSound(0, &RES_sfx_select);
    gfxUpdateBigFrame();
  }

  if (JOY_getButtonJustPressed(0, BUTTON_A)) {
    pathToLoadApp = appEntries[appCursor % APP_ENTRY_AMOUNT].path;
    SFX_playSound(0, &RES_sfx_enter);
    gfxFadeOut();
  };
}