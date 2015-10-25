#include "guilib.h"
#include "tsc2046.h"

Screen whiteScreen;
Screen greyScreen;
View labelBigFontView;
View labelMediumFontView;
View labelSmallFontView;
View comboBoxView;
View checkBoxView;
View sliderView;
View menuButton;
View calibrationButton;
View progressBarView;
View whiteScreenButton;
View greyScreenButton;
View contextMenuItem1View;
View contextMenuItem2View;

const char checkBoxUnchecked[] = "CheckBox - unchecked";
const char checkBoxChecked[] = "CheckBox - checked";

void guiEvent(View *view, GUITouchEventType type)
{
	if(view == &menuButton && type == GUITS_ContextMenu) {
		GUI_drawContextMenu(&menuButton);
	} else if(view == &whiteScreenButton && type == GUITS_Up) {
		GUI_drawScreen(&whiteScreen);
	} else if(view == &greyScreenButton && type == GUITS_Up ) {
		GUI_drawScreen(&greyScreen);
	} else if(view == &calibrationButton && type == GUITS_Up) {
		TSCalibrate();
	} else if(view == &checkBoxView && type == GUITS_Up) {
		checkBoxView.text = checkBoxView.value ? checkBoxChecked : checkBoxUnchecked;
		GUI_drawView(&checkBoxView);
	} else if(view->type == ViewContextMenuItem && type == GUITS_Up) {
		GUI_drawCloceContextMenu();
	}
}

void logoScreen()
{
	LCD_Test();
	LCD_WriteString(20,166,(u8*)"GUI library is designed", &lucidaConsole_18pt, Black, Transparent);
	LCD_WriteString(40,190,(u8*)"by Nikolay Khabarov", &lucidaConsole_18pt,Black, Transparent);
	LCD_WriteString(100,214,(u8*)"***2014***", &lucidaConsole_18pt,Black, Transparent);
}


int main(void)
{
	GUI_InitHardware();
	logoScreen();
	// delay, like something booting
	int i, prevy;
	int x = 0;
	float a = 0.0f;
	for(i = 4000000; i > 0; i--);

	GUI_initScreen(&whiteScreen, White);
	GUI_initLabel(&labelBigFontView, &whiteScreen, "Hello, red world!", &lucidaConsole_18pt, Red, 4,4, GUI_CONTENT, GUI_CONTENT, AlignCenter);
	GUI_initLabel(&labelMediumFontView, &whiteScreen, "Hello, green world!", &lucidaConsole_12pt, Green, 4, 30, GUI_CONTENT, GUI_CONTENT, AlignCenter);
	GUI_initLabel(&labelSmallFontView, &whiteScreen, "Hello, blue world!", &lucidaConsole_6pt, Blue, 4, 52, GUI_CONTENT, GUI_CONTENT, AlignCenter);
	GUI_initComboBox(&comboBoxView, &whiteScreen, " Item1\0 Item2\0 Item3\0 Item4\0 Item5\0 Item6\0 Item7\0 Item8\0 Item9\0\0", &lucidaConsole_18pt, Black, 4, 74, GUI_CONTENT, GUI_CONTENT, AlignCenter);
	GUI_initButton(&checkBoxView, &whiteScreen, checkBoxUnchecked, &lucidaConsole_12pt, Magenta, GUI_TRUE, 4, 120, GUI_CONTENT, GUI_CONTENT, AlignCenter);
	GUI_initSlider(&sliderView, &whiteScreen, Cyan, 4, 170, LCD_XSIZE - 8, GUI_CONTENT);
	GUI_initButton(&greyScreenButton, &whiteScreen, "Grey screen", &lucidaConsole_12pt, Grey, GUI_FALSE, 210, 210, GUI_CONTENT, GUI_CONTENT, AlignCenter);

	GUI_initScreen(&greyScreen, Grey);
	GUI_initButton(&menuButton, &greyScreen, "Long press", &lucidaConsole_12pt, Blue, GUI_FALSE, 30, 20, GUI_CONTENT, GUI_CONTENT, AlignCenter);
	GUI_initButton(&calibrationButton, &greyScreen, "Calibrate", &lucidaConsole_12pt, Blue2, GUI_FALSE, 190, 20, GUI_CONTENT, GUI_CONTENT, AlignCenter);
	GUI_initContextMenuItem(&contextMenuItem1View, &menuButton, "Menu Item 1", &lucidaConsole_12pt, LightGrey);
	GUI_initContextMenuItem(&contextMenuItem2View, &menuButton, "Menu Item 2", &lucidaConsole_12pt, LightGrey);
	GUI_initProgressBar(&progressBarView, &greyScreen, "ProgressBar", &lucidaConsole_12pt, Green, 78, 80, 160, GUI_CONTENT, AlignCenter);
	GUI_initButton(&whiteScreenButton, &greyScreen, "White screen", &lucidaConsole_12pt, White, GUI_FALSE, 4, 210, GUI_CONTENT, GUI_CONTENT, AlignCenter);

	GUI_drawScreen(&whiteScreen);
    while(1)
    {
    	i++;
    	if(i % 200000 == 0) {
    		if(progressBarView.value <= GUI_PROGRESSBAR_MAX_VALUE - 5)
    			progressBarView.value += 5;
    		else
    			progressBarView.value = 0;
    		GUI_drawView(&progressBarView);
    	}
    	if(i % 10000 == 0) {
    		// free draw between y=120...200
			const int ymin = 120, ymax = 200;

			if(GUI_GetCurrentScreen() == &greyScreen) {
				GUI_LCD_Rectangle(x, ymin, 3, ymax - ymin, GUI_GetCurrentScreen()->bgColor, GUI_GetCurrentScreen()->bgColor);

				// a in range -PI...PI
				a += 0.05f;
				if(a > 3.14159265358f)
					a = -3.14159265358f;
				float sina;
				// calc sin, actually can be replaced with sinf() from libmath
				if (a < 0)	{
					sina = 1.27323954f * a + 0.405284735f * a * a;
					if (sina < 0)
						sina = 0.225f * (sina *-sina - sina) + sina;
					else
						sina = 0.225f * (sina * sina - sina) + sina;
				} else {
					sina = 1.27323954f * a - 0.405284735f * a * a;
					if (sina < 0)
						sina = 0.225f * (sina *-sina - sina) + sina;
					else
						sina = 0.225f * (sina * sina - sina) + sina;
				}

				int y = ymin + (1 + sina) * (ymax - ymin) / 2;
				if(x > 0)
					GUI_LCD_Line(x, y, x - 1, prevy, Red);
				prevy = y;

				if(x < LCD_XSIZE - 1)
					x++;
				else
					x = 0;
			}
    	}
    }
}
