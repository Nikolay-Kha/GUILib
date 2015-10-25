#ifndef _LCD_PROXY_H
#define _LCD_PROXY_H
/******************************************************
REIMPLEMENTS FUNCTION IN THIS FILE FOR YOUR LCD DRIVER
******************************************************/
#include "ili93xx.h"

#define GUI_SCREEN_WIDTH LCD_XSIZE
#define GUI_SCREEN_HEIGHT LCD_YSIZE
void GUI_LCD_Clear(u16 color);
void GUI_LCD_Rectangle(u16 x, u16 y, u16 width, u16 height, u16 color, u16 fillColor);
void GUI_LCD_Line(u16 x0, u16 y0, u16 x1, u16 y1,u16 color);
void GUI_LCD_Print(u16 x, u16 y, char *text, const FONT_INFO *font, u16 color);
void GUI_TS_LCD_InitHardware();
/******************************************************
CHOOSE FONT THAT YOU NEED
******************************************************/
#include "lucida6.h"
#include "lucida12.h"
#include "lucida18.h"
/******************************************************
REIMPLEMENTS FUNCTION IN THIS FILE FOR YOUR TOUCH DRIVER
******************************************************/
// dummy
typedef int GUITouchEventType;
#define GUITS_Down 1
#define GUITS_Up 2
#define GUITS_Move 3
#define GUITS_ContextMenu 4
#define GUITS_DrawCalibration 5
#define GUITS_FinishCalibration 6
typedef int GUITouchEventReturn;
#define GUITS_NotInteresting 7
#define GUITS_InterestingUpOnly 8
#define GUITS_InterestingMovementAndUp 9

#endif // _LCD_PROXY_H
