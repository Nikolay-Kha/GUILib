#include "ili93xx.h"
#include "ili93xx_private.h"

// Put char
void LCD_PutChar(u16 x,u16 y,u8 c, const FONT_INFO *font, u16 charColor, u16 bkColor)
{
	if(c < font->startCharecter || c > font->endCharacter)
		return;
	u16 i=0, j=0;

	u8 tmp_char=0;
	u16 offset=font->fontCharInfo[c-font->startCharecter].charOffset;
	u8 width = font->fontCharInfo[c-font->startCharecter].charWidth;

	int spm = (bkColor==Transparent) && !LCD_SkipPixelPossible();

	if(!spm) {
		LCD_SetWindow(x, y, x + width-1, y + font->charHeight-1);
		LCD_PrepareWrite();
	}
	for (i = 0; i < font->charHeight; i++) {
		for (j = 0; j < width; j++) {
			u16 k = j % 8;
			if (k == 0) {
				tmp_char = font->bitmap[offset + i*((width+7)/8) + j/8];
			}
			if (((tmp_char >> (k)) & 0x01) == 0x01) {
				if(spm)
					LCD_SetPoint(x+j, y+i, charColor);
				else
					LCD_WriteWord(charColor);
			} else 	if (bkColor != Transparent) {
				LCD_WriteWord(bkColor);
			} else if(!spm) {
				LCD_SkipPixel();
			}
		}
	}
	LCD_ResetWindow();


}


// Put line
void LCD_WriteString(u16 x, u16 y, u8 *str, const FONT_INFO *font, u16 Color, u16 bkColor)
{
	u16 xt = x;
	u8 nextWidth;
	while ((*str != '\0')) {
		if(*str >= font->startCharecter && *str<= font->endCharacter) {
			LCD_PutChar(xt, y, *str++, font, Color, bkColor);
			if (*str > 0x20)
				nextWidth = font->fontCharInfo[*str - font->startCharecter].charWidth;
			else
				nextWidth = 0;
			if (*str!='\n' && xt < (LCD_XSIZE - nextWidth)) {
				xt += font->fontCharInfo[*(str-1) - font->startCharecter].charWidth;
				if(nextWidth>0 && bkColor!=Transparent) {
					LCD_Rectangle(xt,y,xt+FONTS_FontMargin(font)-1, y+font->charHeight-1,bkColor,bkColor);
				}
				xt += FONTS_FontMargin(font);
			} else {
				xt = x;
				y += font->charHeight;
				if(*str=='\n')
					str++;
			}
		} else {
			str++;
		}
	}
}
