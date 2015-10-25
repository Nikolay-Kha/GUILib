#ifndef _ILI93XX_H_
#define _ILI93XX_H_

#include "halProxy.h"
#include "fonts.h"

/********************************************************************************/
// http://www.henningkarlsen.com/electronics/calc_rgb565.php
#define Transparent    0xFFFE
#define White          0xFFFF
#define Grey           0x7BEF
#define LightGrey 	   0xF79E
#define Black          0x0000
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0
#define Violet		   0xE01F
/********************************************************************************/

// main
void LCD_Initializtion(void);
void LCD_Clear(u16 color);
void LCD_Test(void);
void LCD_SetCursor(u16 x,u16 y);
void LCD_SetWindow(u16 StartX,u16 StartY,u16 EndX,u16 EndY);
void LCD_ResetWindow();
u16 LCD_BGR2RGB(u16 c);
u16 LCD_Color565(u32 RGB);
void LCD_SetPoint(u16 x,u16 y,u16 color);
u16 LCD_GetPoint(u16 x,u16 y);
void LCD_DrawPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic);

// text
void LCD_PutChar(u16 x,u16 y,u8 c, const FONT_INFO *font, u16 charColor,u16 bkColor);
void LCD_WriteString(u16 x, u16 y, u8 *str,const FONT_INFO *font, u16 Color, u16 bkColor);

// simple geometry
void LCD_Line(u16 x0, u16 y0, u16 x1, u16 y1,u16 color);
void LCD_Circle(u16 cx,u16 cy,u16 r,u16 color,u8 fill);
void LCD_Rectangle(u16 x0, u16 y0, u16 x1, u16 y1,u16 color,u16 fillColor);
void LCD_Square(u16 x0, u16 y0, u16 with, u16 color,u16 fillColor);

#endif // _ILI93XX_H_
