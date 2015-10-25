#ifndef __FONTS_H_
#define __FONTS_H_

typedef struct {
	const unsigned char charWidth;
	const unsigned short charOffset;
} FONT_CHAR_INFO;


typedef struct {
	const unsigned char charHeight;
	const unsigned char startCharecter;
	const unsigned char endCharacter;
	const FONT_CHAR_INFO *fontCharInfo;
	const unsigned char *bitmap;
}FONT_INFO;

unsigned short FONTS_GetTextWidth(const FONT_INFO *font, const char *text);
unsigned int FONTS_FontMargin(const FONT_INFO *font);

#endif //__FONTS_H_
