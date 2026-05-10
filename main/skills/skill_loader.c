#include "skills/skill_loader.h"
#include "mimi_config.h"

#include "esp_log.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>

static const char *TAG = "skills";

esp_err_t skill_loader_init(void) {
  ESP_LOGI(TAG, "Initializing skills system");

  DIR *dir = opendir(MIMI_SKILLS_PREFIX);
  if (!dir) {
    dir = opendir(MIMI_SPIFFS_BASE);
    if (!dir) {
      ESP_LOGW(TAG, "Cannot open storage root or skills folder");
      return ESP_OK;
    }
  }

  int count = 0;
  struct dirent *ent;
  while ((ent = readdir(dir)) != NULL) {
    const char *name = ent->d_name;
    ESP_LOGI(TAG, "Found file: %s", name);
    size_t len = strlen(name);
    if (len > 3 && strcmp(name + len - 3, ".md") == 0) {
      count++;
    }
  }
  closedir(dir);

  ESP_LOGI(TAG, "Skills system ready (%d skills found)", count);
  return ESP_OK;
}

static void extract_title(const char *line, size_t len, char *out,
                          size_t out_size) {
  const char *start = line;
  if (len >= 2 && line[0] == '#' && line[1] == ' ') {
    start = line + 2;
    len -= 2;
  }
  while (len > 0 && (start[len - 1] == '\n' || start[len - 1] == '\r' ||
                     start[len - 1] == ' ')) {
    len--;
  }
  size_t copy = len < out_size - 1 ? len : out_size - 1;
  memcpy(out, start, copy);
  out[copy] = '\0';
}

static void extract_description(FILE *f, char *out, size_t out_size) {
  size_t off = 0;
  char line[256];
  while (fgets(line, sizeof(line), f) && off < out_size - 1) {
    size_t len = strlen(line);
    if (len == 0 || (len == 1 && line[0] == '\n') ||
        (len >= 2 && line[0] == '#' && line[1] == '#')) {
      break;
    }
    if (off == 0 && line[0] == '\n')
      continue;
    if (line[len - 1] == '\n')
      line[len - 1] = ' ';
    size_t copy = len < out_size - off - 1 ? len : out_size - off - 1;
    memcpy(out + off, line, copy);
    off += copy;
  }
  while (off > 0 && out[off - 1] == ' ')
    off--;
  out[off] = '\0';
}

size_t skill_loader_build_summary(char *buf, size_t size) {
  DIR *dir = opendir(MIMI_SKILLS_PREFIX);
  bool flat_mode = false;
  if (!dir) {
    dir = opendir(MIMI_SPIFFS_BASE);
    flat_mode = true;
    if (!dir) {
      ESP_LOGW(TAG, "Cannot open storage root for skill enumeration");
      buf[0] = '\0';
      return 0;
    }
  }

  size_t off = 0;
  struct dirent *ent;
  const char *skills_subdir = "skills/";
  const size_t subdir_len = strlen(skills_subdir);

  while ((ent = readdir(dir)) != NULL && off < size - 1) {
    const char *name = ent->d_name;
    if (flat_mode) {
      if (strncmp(name, skills_subdir, subdir_len) != 0)
        continue;
    }
    size_t name_len = strlen(name);
    if (name_len < 3 || strcmp(name + name_len - 3, ".md") != 0)
      continue;

    char full_path[296];
    if (flat_mode) {
      snprintf(full_path, sizeof(full_path), "%s/%s", MIMI_SPIFFS_BASE, name);
    } else {
      snprintf(full_path, sizeof(full_path), "%s%s", MIMI_SKILLS_PREFIX, name);
    }

    FILE *f = fopen(full_path, "r");
    if (!f)
      continue;

    char first_line[128];
    if (!fgets(first_line, sizeof(first_line), f)) {
      fclose(f);
      continue;
    }
    char title[64];
    extract_title(first_line, strlen(first_line), title, sizeof(title));

    char desc[256];
    extract_description(f, desc, sizeof(desc));
    fclose(f);

    off += snprintf(buf + off, size - off,
                    "- **%s**: %s (read with: read_file %s)\n", title, desc,
                    full_path);
  }
  closedir(dir);
  buf[off] = '\0';
  ESP_LOGI(TAG, "Skills summary: %d bytes", (int)off);
  return off;
}
