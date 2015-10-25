#include "ili93xx.h"
#include "ili93xx_private.h"

void LCD_Line(u16 x0, u16 y0, u16 x1, u16 y1,u16 color)
{
	s32 dx = x1 - x0, dy = y1 - y0;
	s32 sx = 1, sy = 1;
	if(dx<0)
		sx = -1;
	if(dy<0)
		sy = -1;

	LCD_SetPoint(x0, y0, color);

	if(dx*sx>dy*sy) {
		s32 x = x0;
		if(dy==0) {
			while (x!=x1) {
				x += sx;
				LCD_SetPoint(x, y0, color);
			}
		} else {
			while (x!=x1) {
				x += sx;
				LCD_SetPoint(x, y0+dy*(x-(s32)x0)/dx, color);
			}
		}
	} else {
		s32 y = y0;
		if(dx==0) {
			while (y!=y1) {
				y += sy;
				LCD_SetPoint(x0, y, color);
			}
		} else {
			while (y!=y1) {
				y += sy;
				LCD_SetPoint(x0+dx*(y-(s32)y0)/dy, y, color);
			}
		}
	}
}

void LCD_Circle(u16 cx,u16 cy,u16 r,u16 color,u8 fill)
{
	u16 x,y;
	s16 delta,tmp;
	x=0;
	y=r;
	delta=3-(r<<1);

	while(y>x)
	{
		if(fill)
		{
			LCD_Line(cx+x,cy+y,cx-x,cy+y,color);
			LCD_Line(cx+x,cy-y,cx-x,cy-y,color);
			LCD_Line(cx+y,cy+x,cx-y,cy+x,color);
			LCD_Line(cx+y,cy-x,cx-y,cy-x,color);
		}
		else
		{
			LCD_SetPoint(cx+x,cy+y,color);
			LCD_SetPoint(cx-x,cy+y,color);
			LCD_SetPoint(cx+x,cy-y,color);
			LCD_SetPoint(cx-x,cy-y,color);
			LCD_SetPoint(cx+y,cy+x,color);
			LCD_SetPoint(cx-y,cy+x,color);
			LCD_SetPoint(cx+y,cy-x,color);
			LCD_SetPoint(cx-y,cy-x,color);
		}
		x++;
		if(delta>=0)
		{
			y--;
			tmp=(x<<2);
			tmp-=(y<<2);
			delta+=(tmp+10);
		}
		else
		{
			delta+=((x<<2)+6);
		}
	}
}

void LCD_Rectangle(u16 x0, u16 y0, u16 x1, u16 y1,u16 color, u16 fillColor)
{
	if(x0>x1){
		x0 = x0 + x1;
		x1 = x0 - x1;
		x0 = x0 - x1;
	}
	if(y0>y1){
			y0 = y0 + y1;
			y1 = y0 - y1;
			y0 = y0 - y1;
	}
	if(fillColor!=Transparent){
		if(color==Transparent) {
			x0++;x1--;y0++;y1--;
		}
		LCD_SetWindow(x0,y0,x1,y1);
		u32 i;
		LCD_PrepareWrite();
		u32 c = (x1-x0+1)*(y1-y0+1);
		for(i=0; i<c; i++)
			LCD_WriteWord(fillColor);
		LCD_ResetWindow();
		if(color==fillColor || color==Transparent)
			return;
	}

	LCD_Line(x0,y0,x0,y1,color);
	LCD_Line(x0,y1,x1,y1,color);
	LCD_Line(x1,y1,x1,y0,color);
	LCD_Line(x1,y0,x0,y0,color);
}

void  LCD_Square(u16 x0, u16 y0, u16 with, u16 color,u16 fillColor)
{
	LCD_Rectangle(x0, y0, x0+with, y0+with, color,fillColor);
}
