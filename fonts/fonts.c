#include "fonts.h"
#include <stm32f10x.h>

inline unsigned int FONTS_FontMargin(const FONT_INFO *font) {
	return (font->charHeight+15)/16;
}

// Calculate text width
unsigned short FONTS_GetTextWidth(const FONT_INFO *font, const char *text)
{
	u16 res = 0;
	while ((*text != '\0')) {
		u8 c = *text++;
		if(c>=font->startCharecter && c<=font->endCharacter) {
			res += font->fontCharInfo[c - font->startCharecter].charWidth;
			if (*text > 0x20)
				res += FONTS_FontMargin(font);
		}
	}
	return res;
}

