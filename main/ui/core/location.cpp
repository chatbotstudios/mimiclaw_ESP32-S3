#include "ui/core/location.h"
#include <string.h>

void location_begin(void) {}
void location_place(char* out, size_t cap) {
    if (cap > 0) {
        strncpy(out, "San Francisco", cap - 1);
        out[cap - 1] = '\0';
    }
}
loc_source_t location_source(void) { return LOC_SRC_NONE; }
void location_set_from_hub(float lat, float lon, const char* tz, const char* place) {}
void location_set_from_ip (float lat, float lon, const char* tz, const char* place) {}
