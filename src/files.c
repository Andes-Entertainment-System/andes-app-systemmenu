#include "files.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>

char appFilenames[4096][255];
struct AppEntry appEntries[5];
uint32_t appCount = 0;

char *getPathExtension(char *path) {
  char *dot = strrchr(path, '.');
  if (!dot || dot == path) return "";
  return dot + 1;
}

void readMetaItem(FILE *file, void *out) {
  uint32_t itemSize;
  fread(&itemSize, sizeof(itemSize), 1, file);
  fread(out, sizeof(char), itemSize, file);
}

bool fsCheckAppValidity(char *path) {
  char tempPath[STRING_BUF_SIZE];
  snprintf(tempPath, sizeof(tempPath), "./apps/%s", path);

  FILE *file = fopen(tempPath, "r");

  // ANDES magic header check
  char magicHeader[6];
  memset(magicHeader, 0, 6);
  fread(magicHeader, sizeof(char), 5, file);
  bool isValidApp = strcmp(magicHeader, "ANDES") == 0;

  fclose(file);
  return isValidApp;
}

bool fsLoadAppEntry(char *path, struct AppEntry *entry) {
  snprintf(entry->path, sizeof(entry->path), "./apps/%s", path);
  FILE *file = fopen(entry->path, "r");

  fseek(file, ANDES_HEADER_SIZE, SEEK_SET);

  memset(entry->id, 0, sizeof(entry->id));
  memset(entry->title, 0, sizeof(entry->title));
  memset(entry->author, 0, sizeof(entry->author));

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

  fclose(file);

  return true;
}

void fsInit() {
  DIR *d;
  struct dirent *dir;
  d = opendir("./apps");
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (dir->d_type != 4) continue;
      if (!fsCheckAppValidity(dir->d_name)) continue;

      strncpy(appFilenames[appCount], dir->d_name, sizeof(appFilenames[appCount]));
      appCount++;
    }
    closedir(d);
  }
}