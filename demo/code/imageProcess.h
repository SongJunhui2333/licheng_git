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

// 四步过环，遇环、入环、出环、过环
extern uint8 meetRingFlag;    // 遇环标志位，第一个A字标志点——第一个V字标志点
extern uint8 enterRingFlag_1; // 入环标志位，第一个V字标志点——第二个A字标志点
extern uint8 enterRingFlag_2; // 上一帧跳变标志位
extern uint8 leaveRingFlag;   // 出环标志位，第二个A字标志点——第二个V字标志点
extern uint8 passRingFlag;    // 过环标志位，第二个V字标志点——下一个A字标志点
extern uint8 ringSide;        // 环岛类型，1表示左，2表示右
extern uint8 carType;       // 指示车辆状态 

#endif