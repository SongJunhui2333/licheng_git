#ifndef TRACK_H
#define TRACK_H

#include "main.h"
#include "zf_common_headfile.h"

extern uint8_t x1;
extern uint8_t x2;
extern uint8_t x3;
extern uint8_t x4;
extern uint8_t x5;
extern uint8_t x6;
extern uint8_t x7;
extern uint8_t x8;

// Function declarations
void Track_Init(void);

void Track_GPIO_test(void);
void CONTRAL1(void);
// Turn 90 degrees: dir = 0 -> left, dir = 1 -> right
void Turn90(uint8_t dir, uint32_t turn_ms);
#endif // TRACK_H