#ifndef __TSC2046_H
#define __TSC2046_H

#include "stm32f10x.h"

typedef enum {
	TS_Down = 0,
	TS_Up,
	TS_Move,
	TS_LongPress,
	TS_WaitTouchPointForCalibration,
	TS_CalibrationDone
} TouchEventType;

typedef enum {
	TS_NotInteresting = 0,
	TS_InterestingUpOnly,
	TS_InterestingMovementAndUp
} TouchEventReturn;

typedef enum {
	TS_NotPressed = 0,
	TS_Pressed = 1
} TouchState;

typedef struct {
	s32 ADC_X_MIN;
	s32 ADC_Y_MIN;
	s32 ADC_X_WIDTH;
	s32 ADC_Y_WIDTH;
} TS_CALIBRATION_DATA;


void TS_Init();

u16 TSMeasureX(void);
u16 TSMeasureY(void);
TouchState TSState(void);
void TS_TouchIrq(); // call it when interruption by touching happen

int TSCalibrate();
void TSInteruptCallibration();
TS_CALIBRATION_DATA * TSGetCalibrationDataPtr();

extern TouchEventReturn touchEvent(u16 x, u16 y, TouchEventType type); // callback, implement it somewhere,
#endif // __TSC2046_H
