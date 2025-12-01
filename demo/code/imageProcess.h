#ifndef img_process_h
#define img_process_h

#include "zf_common_headfile.h"
#include "main.h"

void imageProcess(uint8 image[MT9V03X_H][MT9V03X_W]);

// ================== Jump detection configuration ==================
// Override defaults by defining these macros in a project-wide config
#ifndef JP_DEFAULT_THRESHOLD
#define JP_DEFAULT_THRESHOLD 20 // default threshold for a sudden edge change (px)
#endif

#ifndef JP_DEBOUNCE_ROWS
#define JP_DEBOUNCE_ROWS 2 // require this many consecutive rows to confirm
#endif

#ifndef JP_LANE_DIV_FACTOR
#define JP_LANE_DIV_FACTOR 6 // laneWidth / JP_LANE_DIV_FACTOR used for adaptive threshold
#endif

// sanity checks
#if (JP_DEBOUNCE_ROWS < 1)
#undef JP_DEBOUNCE_ROWS
#define JP_DEBOUNCE_ROWS 1
#endif
#if (JP_LANE_DIV_FACTOR < 1)
#undef JP_LANE_DIV_FACTOR
#define JP_LANE_DIV_FACTOR 6
#endif

// optional debug enable
#ifndef JP_DEBUG
#define JP_DEBUG 0
#endif

#endif