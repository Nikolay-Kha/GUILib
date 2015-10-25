#ifndef _ILI93XX_PRIVATE_H_
#define _ILI93XX_PRIVATE_H_

//private
void LCD_PrepareWrite();
void LCD_PrepareRead();
void LCD_WriteWord(u16 data);
void LCD_SkipPixel();
int LCD_SkipPixelPossible();

#endif // _ILI93XX_PRIVATE_H_
