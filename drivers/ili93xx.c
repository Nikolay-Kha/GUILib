#include "ili93xx.h"
#include "ili93xx_private.h"


u16 DeviceCode;

inline void LCD_WriteRegister(u16 reg, u16 data)
{
	LCD_HAL_SetRegister(reg);
	LCD_HAL_WriteData(data);
}

inline void LCD_PrepareWrite()
{
	if(DeviceCode==0x9341) {
		LCD_HAL_SetRegister(0x2C);
	} else {
		LCD_HAL_SetRegister(0x0022);
	}
}

inline void LCD_PrepareRead()
{
	if(DeviceCode==0x9341) {
		LCD_HAL_SetRegister(0x2E);
	} else {
		LCD_HAL_SetRegister(0x0022);
	}
}

inline int LCD_SkipPixelPossible()
{
	return (DeviceCode!=0x9341);
}

inline void LCD_SkipPixel()
{
	if(DeviceCode==0x9341) {
		;
	} else {
		LCD_HAL_ReadData();
		LCD_WriteWord(LCD_BGR2RGB(LCD_HAL_ReadData())); // dummy,for move cursor
	}
}

inline void LCD_WriteWord(u16 data)
{
	if(DeviceCode==0x9341) {
		LCD_HAL_WriteData((data >> 8)  & 0x00FF);
		LCD_HAL_WriteData(data & 0x00FF);
	} else {
		LCD_HAL_WriteData(data);
	}
}

void LCD_Initializtion()
{
	LCD_HAL_Init();
	LCD_HAL_Reset();

  	u16 i;
  	LCD_HAL_Light(100);

	LCD_WriteRegister(0x0000,0x0001); //start oscillation
	for(i=50000;i>0;i--);
	for(i=50000;i>0;i--);
	DeviceCode = LCD_HAL_ReadRegister(0x0000);
	if(DeviceCode==0x9325)
	{
		LCD_WriteRegister(0x00e7,0x0010);
		LCD_WriteRegister(0x0000,0x0001); //start internal osc
		LCD_WriteRegister(0x0001,0x0100);
		LCD_WriteRegister(0x0002,0x0700); //power on sequence
		LCD_WriteRegister(0x0003,0x1018); //paint direction
		LCD_WriteRegister(0x0004,0x0000);
		LCD_WriteRegister(0x0008,0x0207);
		LCD_WriteRegister(0x0009,0x0000);
		LCD_WriteRegister(0x000a,0x0000); //display setting
		LCD_WriteRegister(0x000c,0x0001); //display setting
		LCD_WriteRegister(0x000d,0x0000); //0f3c
		LCD_WriteRegister(0x000f,0x0000);
		LCD_WriteRegister(0x0010,0x0000);
		LCD_WriteRegister(0x0011,0x0007);
		LCD_WriteRegister(0x0012,0x0000);
		LCD_WriteRegister(0x0013,0x0000);
		for(i=50000;i>0;i--);
		LCD_WriteRegister(0x0010,0x1590);
		LCD_WriteRegister(0x0011,0x0227);
		for(i=50000;i>0;i--);
		LCD_WriteRegister(0x0012,0x009c);
		for(i=50000;i>0;i--);
		LCD_WriteRegister(0x0013,0x1900);
		LCD_WriteRegister(0x0029,0x0023);
		LCD_WriteRegister(0x002b,0x000e);
		for(i=50000;i>0;i--);
		LCD_WriteRegister(0x0020,0x0000);
		LCD_WriteRegister(0x0021,0x0000);
		for(i=50000;i>0;i--);
		 // Set gamma
		LCD_WriteRegister(0x0030,0x0007);
		LCD_WriteRegister(0x0031,0x0707);
		LCD_WriteRegister(0x0032,0x0006);
		LCD_WriteRegister(0x0035,0x0704);
		LCD_WriteRegister(0x0036,0x1f04);
		LCD_WriteRegister(0x0037,0x0004);
		LCD_WriteRegister(0x0038,0x0000);
		LCD_WriteRegister(0x0039,0x0706);
		LCD_WriteRegister(0x003c,0x0701);
		LCD_WriteRegister(0x003d,0x000f);
		for(i=50000;i>0;i--);
		LCD_WriteRegister(0x0050,0x0000);
		LCD_WriteRegister(0x0051,LCD_YSIZE-1);
		LCD_WriteRegister(0x0052,0x0000);
		LCD_WriteRegister(0x0053,LCD_XSIZE-1);
		LCD_WriteRegister(0x0060,0xa700);
		LCD_WriteRegister(0x0061,0x0001);
		LCD_WriteRegister(0x006a,0x0000);
		LCD_WriteRegister(0x0080,0x0000);
		LCD_WriteRegister(0x0081,0x0000);
		LCD_WriteRegister(0x0082,0x0000);
		LCD_WriteRegister(0x0083,0x0000);
		LCD_WriteRegister(0x0084,0x0000);
		LCD_WriteRegister(0x0085,0x0000);
		LCD_WriteRegister(0x0090,0x0010);
		LCD_WriteRegister(0x0092,0x0000);
		LCD_WriteRegister(0x0093,0x0003);
		LCD_WriteRegister(0x0095,0x0110);
		LCD_WriteRegister(0x0097,0x0000);
		LCD_WriteRegister(0x0098,0x0000);
		 //display on sequence
		LCD_WriteRegister(0x0007,0x0133);
		LCD_WriteRegister(0x0020,0x0000);
		LCD_WriteRegister(0x0021,0x0000);
	}
	else if(DeviceCode==0x9320)
	{
		LCD_WriteRegister(0x00,0x0000);
		LCD_WriteRegister(0x01,0x0100);	//Driver Output Contral.
		LCD_WriteRegister(0x02,0x0700);	//LCD Driver Waveform Contral.
		LCD_WriteRegister(0x03,0x1018);	//Entry Mode Set.

		LCD_WriteRegister(0x04,0x0000);	//Scalling Contral.
		LCD_WriteRegister(0x08,0x0202);	//Display Contral 2.(0x0207)
		LCD_WriteRegister(0x09,0x0000);	//Display Contral 3.(0x0000)
		LCD_WriteRegister(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
		LCD_WriteRegister(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
		LCD_WriteRegister(0x0d,0x0000);	//Frame Maker Position.
		LCD_WriteRegister(0x0f,0x0000);	//Extern Display Interface Contral 2.

		for(i=50000;i>0;i--);
		LCD_WriteRegister(0x07,0x0101);	//Display Contral.
		for(i=50000;i>0;i--);

		LCD_WriteRegister(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4)); //Power Control 1.(0x16b0)
		LCD_WriteRegister(0x11,0x0007);	//Power Control 2.(0x0001)
		LCD_WriteRegister(0x12,(1<<8)|(1<<4)|(0<<0)); //Power Control 3.(0x0138)
		LCD_WriteRegister(0x13,0x0b00);	//Power Control 4.
		LCD_WriteRegister(0x29,0x0000);	//Power Control 7.

		LCD_WriteRegister(0x2b,(1<<14)|(1<<4));
		LCD_WriteRegister(0x50,0); //Set X Start.
		LCD_WriteRegister(0x51,LCD_YSIZE-1); //Set X End.
		LCD_WriteRegister(0x52,0); //Set Y Start.
		LCD_WriteRegister(0x53,LCD_XSIZE-1); //Set Y End.

		LCD_WriteRegister(0x60,0x2700);	//Driver Output Control.
		LCD_WriteRegister(0x61,0x0001);	//Driver Output Control.
		LCD_WriteRegister(0x6a,0x0000);	//Vertical Srcoll Control.

		LCD_WriteRegister(0x80,0x0000);	//Display Position? Partial Display 1.
		LCD_WriteRegister(0x81,0x0000);	//RAM Address Start? Partial Display 1.
		LCD_WriteRegister(0x82,0x0000);	//RAM Address End-Partial Display 1.
		LCD_WriteRegister(0x83,0x0000);	//Displsy Position? Partial Display 2.
		LCD_WriteRegister(0x84,0x0000);	//RAM Address Start? Partial Display 2.
		LCD_WriteRegister(0x85,0x0000);	//RAM Address End? Partial Display 2.

		LCD_WriteRegister(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
		LCD_WriteRegister(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
		LCD_WriteRegister(0x93,0x0001);	//Panel Interface Contral 3.
		LCD_WriteRegister(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
		LCD_WriteRegister(0x97,(0<<8));	//
		LCD_WriteRegister(0x98,0x0000);	//Frame Cycle Contral.

		LCD_WriteRegister(0x07,0x0173);	//(0x0173)
	} else
	{
		LCD_HAL_SetRegister(0xD9);
		LCD_HAL_WriteData(0x10);
		LCD_HAL_SetRegister(0xD3);
		LCD_HAL_ReadData();// dummy
		LCD_HAL_SetRegister(0xD9);
		LCD_HAL_WriteData(0x11);
		LCD_HAL_SetRegister(0xD3);
		DeviceCode = LCD_HAL_ReadData();
		LCD_HAL_SetRegister(0xD9);
		LCD_HAL_WriteData(0x12);
		LCD_HAL_SetRegister(0xD3);
		LCD_HAL_ReadData(); // dummy
		LCD_HAL_SetRegister(0xD9);
		LCD_HAL_WriteData(0x13);
		LCD_HAL_SetRegister(0xD3);
		DeviceCode |= LCD_HAL_ReadData() << 8;

		if(DeviceCode==0x9341) {
			LCD_HAL_SetRegister(0xCB);
			LCD_HAL_WriteData(0x39);
			LCD_HAL_WriteData(0x2C);
			LCD_HAL_WriteData(0x00);
			LCD_HAL_WriteData(0x34);
			LCD_HAL_WriteData(0x02);

			LCD_HAL_SetRegister(0xCF);
			LCD_HAL_WriteData(0x00);
			LCD_HAL_WriteData(0xC1);
			LCD_HAL_WriteData(0x30);

			LCD_HAL_SetRegister(0xE8);
			LCD_HAL_WriteData(0x85);
			LCD_HAL_WriteData(0x00);
			LCD_HAL_WriteData(0x78);

			LCD_HAL_SetRegister(0xEA);
			LCD_HAL_WriteData(0x00);
			LCD_HAL_WriteData(0x00);

			LCD_HAL_SetRegister(0xED);
			LCD_HAL_WriteData(0x64);
			LCD_HAL_WriteData(0x03);
			LCD_HAL_WriteData(0x12);
			LCD_HAL_WriteData(0x81);

			LCD_HAL_SetRegister(0xF7);
			LCD_HAL_WriteData(0x20);

			LCD_HAL_SetRegister(0xC0); //Power control
			LCD_HAL_WriteData(0x23); //VRH[5:0]

			LCD_HAL_SetRegister(0xC1); //Power control
			LCD_HAL_WriteData(0x10); //SAP[2:0];BT[3:0]

			LCD_HAL_SetRegister(0xC5); //VCM control
			LCD_HAL_WriteData(0x3e); //Contrast
			LCD_HAL_WriteData(0x28);

			LCD_HAL_SetRegister(0xC7); //VCM control2
			LCD_HAL_WriteData(0x86); //--

			LCD_HAL_SetRegister(0x36); // Memory Access Control - paint direction
			LCD_HAL_WriteData(0xE8);

			LCD_HAL_SetRegister(0x3A);
			LCD_HAL_WriteData(0x55);

			LCD_HAL_SetRegister(0xB1);
			LCD_HAL_WriteData(0x00);
			LCD_HAL_WriteData(0x18);

			LCD_HAL_SetRegister(0xB6); // Display Function Control
			LCD_HAL_WriteData(0x08);
			LCD_HAL_WriteData(0x82);
			LCD_HAL_WriteData(0x27);

			LCD_HAL_SetRegister(0xF2); // 3Gamma Function Disable
			LCD_HAL_WriteData(0x00);

			LCD_HAL_SetRegister(0x26); //Gamma curve selected
			LCD_HAL_WriteData(0x01);

			LCD_HAL_SetRegister(0xE0); //Set Gamma
			LCD_HAL_WriteData(0x0F);
			LCD_HAL_WriteData(0x31);
			LCD_HAL_WriteData(0x2B);
			LCD_HAL_WriteData(0x0C);
			LCD_HAL_WriteData(0x0E);
			LCD_HAL_WriteData(0x08);
			LCD_HAL_WriteData(0x4E);
			LCD_HAL_WriteData(0xF1);
			LCD_HAL_WriteData(0x37);
			LCD_HAL_WriteData(0x07);
			LCD_HAL_WriteData(0x10);
			LCD_HAL_WriteData(0x03);
			LCD_HAL_WriteData(0x0E);
			LCD_HAL_WriteData(0x09);
			LCD_HAL_WriteData(0x00);

			LCD_HAL_SetRegister(0xE1); //Set Gamma
			LCD_HAL_WriteData(0x00);
			LCD_HAL_WriteData(0x0E);
			LCD_HAL_WriteData(0x14);
			LCD_HAL_WriteData(0x03);
			LCD_HAL_WriteData(0x11);
			LCD_HAL_WriteData(0x07);
			LCD_HAL_WriteData(0x31);
			LCD_HAL_WriteData(0xC1);
			LCD_HAL_WriteData(0x48);
			LCD_HAL_WriteData(0x08);
			LCD_HAL_WriteData(0x0F);
			LCD_HAL_WriteData(0x0C);
			LCD_HAL_WriteData(0x31);
			LCD_HAL_WriteData(0x36);
			LCD_HAL_WriteData(0x0F);


			LCD_HAL_SetRegister(0x11); //Exit Sleep
			for(i=0; i<0x00FFF; i++);

			LCD_HAL_SetRegister(0x29); //Display on
			LCD_HAL_SetRegister(0x2c);
		}
	}
  for(i=50000;i>0;i--);
}

// Draw test screen
void LCD_Test()
{
	u8  R_data,G_data,B_data,i,j;
	R_data=0;G_data=0;B_data=0;
	LCD_SetWindow(0,0,59,LCD_YSIZE-1);
	LCD_PrepareWrite();
	for(i=0;i<LCD_YSIZE;i++)
	{
		for(j=0;j<60;j++)
		{
			R_data=i/8;LCD_WriteWord(R_data<<11|G_data<<5|B_data);
		}
	}
	R_data=0x1f;G_data=0x3f;B_data=0x1f;
	LCD_SetWindow(60,0,109,LCD_YSIZE-1);
	LCD_PrepareWrite();
	for(i=0;i<LCD_YSIZE;i++)
	{
		for(j=0;j<50;j++)
		{
			G_data=0x3f-(i/4);
			B_data=0x1f-(i/8);
			LCD_WriteWord(R_data<<11|G_data<<5|B_data);
		}
	}
	//----------------------------------
	R_data=0;G_data=0;B_data=0;
	LCD_SetWindow(110,0,159,LCD_YSIZE-1);
	LCD_PrepareWrite();
	for(i=0;i<LCD_YSIZE;i++)
	{
		for(j=0;j<50;j++)
		{G_data=i/4;
		LCD_WriteWord(R_data<<11|G_data<<5|B_data);
		}
	}

	R_data=0x1f;G_data=0x3f;B_data=0x1f;
	LCD_SetWindow(160,0,209,LCD_YSIZE-1);
	LCD_PrepareWrite();
	for(i=0;i<LCD_YSIZE;i++)
	{
		for(j=0;j<50;j++)
		{
			R_data=0x1f-(i/8);
			B_data=0x1f-(i/8);
			LCD_WriteWord(R_data<<11|G_data<<5|B_data);
		}
	}
	//----------------------------------

	R_data=0;G_data=0;B_data=0;
	LCD_SetWindow(210,0,259,LCD_YSIZE-1);
	LCD_PrepareWrite();
	for(i=0;i<LCD_YSIZE;i++)
	{
		for(j=0;j<50;j++)
		{B_data=i/8;LCD_WriteWord(R_data<<11|G_data<<5|B_data);
		}
	}

	B_data=0;
	R_data=0x1f;G_data=0x3f;B_data=0x1f;
	LCD_SetWindow(260,0,LCD_XSIZE-1,LCD_YSIZE-1);
	LCD_PrepareWrite();
	for(i=0;i<LCD_YSIZE;i++)
	{
		for(j=0;j<60;j++)
		{
			G_data=0x3f-(i/4);
			R_data=0x1f-(i/8);
			LCD_WriteWord(R_data<<11|G_data<<5|B_data);
		}
	}
	LCD_ResetWindow();
}

// Convert color scheme
u16 LCD_BGR2RGB(u16 c)
{
  u16  r, b;

  b = c & 0x1f;
  r = (c>>11) & 0x1f;

  return( r + (c & 0x7E0) + (b<<11) );
}

u16 LCD_Color565(u32 RGB)
{
  u8  r, g, b;

  b = ( RGB >> (0+3) ) & 0x1f;
  g = ( RGB >> (8+2) ) & 0x3f;
  r = ( RGB >> (16+3)) & 0x1f;

  return( (r<<11) + (g<<5) + (b<<0) );
}

//Set cursor position to point in ili93xx device
void LCD_SetCursor(u16 x,u16 y)
{
	if(DeviceCode==0x9341)
	{
		LCD_HAL_SetRegister(0x2A);
		LCD_WriteWord(x);
		LCD_WriteWord(x);
		LCD_HAL_SetRegister(0x2B);
		LCD_WriteWord(y);
		LCD_WriteWord(y);
	}
	else
	{
  		LCD_WriteRegister(0x0020,y);
  		LCD_WriteRegister(0x0021,LCD_XSIZE-1-x);
	}
}

// Set area on screen where driver have to paint
void LCD_SetWindow(u16 StartX,u16 StartY,u16 EndX,u16 EndY)
{
	if(DeviceCode==0x9341)
	{
		LCD_HAL_SetRegister(0x2A);
		LCD_WriteWord(StartX);
		LCD_WriteWord(EndX);
		LCD_HAL_SetRegister(0x2B);
		LCD_WriteWord(StartY);
		LCD_WriteWord(EndY);
	}
	else
	{
	  LCD_WriteRegister(0x0050, StartY);
	  LCD_WriteRegister(0x0052, LCD_XSIZE-1-EndX);
	  LCD_WriteRegister(0x0051, EndY);
	  LCD_WriteRegister(0x0053, LCD_XSIZE-1-StartX);
	  LCD_SetCursor(StartX,StartY);
	}
}

// Set area to all screen
void LCD_ResetWindow()
{
	LCD_SetWindow(0, 0, LCD_XSIZE-1, LCD_YSIZE-1);
}

// Fill screen with one color
void LCD_Clear(u16 color)
{
  u32  i;

  LCD_ResetWindow();
  LCD_PrepareWrite();
  for(i=0;i<LCD_XSIZE*LCD_YSIZE;i++)
  {
	  LCD_WriteWord(color);
  }
}


// Get point color
u16 LCD_GetPoint(u16 x,u16 y)
{
	u16 temp;
	LCD_SetCursor(x,y);
	LCD_PrepareRead();
	if(DeviceCode!=0x9341) {
		temp = LCD_HAL_ReadData(); //dummy
		temp = LCD_HAL_ReadData();
	} else {
		temp = LCD_HAL_ReadData(); //dummy
		temp = LCD_HAL_ReadData(); //dummy
		temp = LCD_HAL_ReadData()<<8;
		temp |= LCD_HAL_ReadData();
	}
	if(DeviceCode!=0x7783)
		temp=LCD_BGR2RGB(temp);
	return (temp);
}

//Set point to color
void LCD_SetPoint(u16 x,u16 y,u16 color)
{
  if ( (x>LCD_XSIZE)||(y>LCD_YSIZE)||(x<0)||(y<0) ) return;
  LCD_SetCursor(x,y);

  LCD_PrepareWrite();
  LCD_WriteWord(color);
}

// Draw picture
void LCD_DrawPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic)
{
  u16  i;
  LCD_SetWindow(StartX,StartY,EndX,EndY);
  LCD_PrepareWrite();
  u32 c = (EndX-StartX+1)*(EndY-StartY+1);
  for (i=0;i<c;i++)
  {
	  LCD_WriteWord(*pic++);
  }
  LCD_ResetWindow();
}
