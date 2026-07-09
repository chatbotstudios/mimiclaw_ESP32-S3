#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "config/tickers.h"
#include "core/records.h"   // FIN_ID_LEN

// Runtime, owned-storage ticker table (design §3.1). Seeded from DEFAULT_TICKERS at boot;
// later chunks load it from NVS and hot-swap it on a hub push. Guarded by a dedicated lock
// owned by this module (NOT DataStore's private s_lock): consumers copy the row they need
// under the lock, then release before any blocking call (fetch/render).

// String bounds. Worst case in DEFAULT_TICKERS today: symbol "EURUSD=X" (8) and name
// "OIL WTI"/"S&P 500" (7); Yahoo encoded symbols like "%5EGSPC" stay <= 7. 24/24 leaves
// headroom for longer hub-pushed symbols/names; A4 rejects anything over these bounds.
#define TKR_SYM_LEN  24
#define TKR_NAME_LEN 24

typedef struct {
  char            id[FIN_ID_LEN];
  ticker_source_t source;
  char            symbol[TKR_SYM_LEN];
  char            name[TKR_NAME_LEN];
  ticker_kind_t   kind;
  uint16_t        cadence_s;
  uint32_t        stale_s;
  change_basis_t  change_basis;
} ticker_runtime_t;

static inline void     ticker_table_init(void) {}
static inline int      ticker_table_count(void) { return 0; }
static inline bool     ticker_table_get(int i, ticker_runtime_t* out) { return false; }
static inline uint32_t ticker_table_gen(void) { return 0; }
static inline void     ticker_table_set(const ticker_runtime_t* rows, int count) {}
static inline bool     ticker_table_apply(const ticker_runtime_t* rows, int count) { return false; }
