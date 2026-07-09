#pragma once
// Panel + safe-area geometry. SAFE_INSET / CORNER_R are FROZEN once the
// cyan-border test (Task 1) confirms them on hardware (FR-PLAT-9, DESIGN.md).
#define SCREEN_W   368
#define SCREEN_H   448
#define CORNER_R   40    // Waveshare AMOLED corners
#define SAFE_INSET 20    // smaller inset for rectangular screen

// CO5300 visible-window offset inside its 480x480 GRAM. Centered margin = (480-466)/2 = 7,
// but the CO5300 requires EVEN window coordinates (odd CASET/PASET corrupt partial redraws),
// and the LVGL rounder snaps logical coords to even -> GRAM start = logical + offset must stay even,
// so the offset must be even. 8 is the nearest even to the centered 7 (<=1px asymmetry, immaterial).
// Measured on hardware (P0-A): offset 0 lost top-left, 14 lost bottom-right; 8 centers + stays even.
// FOR 1.75" BOARD: The offset is (6, 0).
#define LCD_X_OFFSET 6
#define LCD_Y_OFFSET 0

// Hit-slop (px) on buddy approve/deny buttons; glance-and-tap reachability across all themes.
#define BUDDY_HIT_SLOP 24
