#include "guilib.h"
#include "guiprivate.h"

Screen *GUI_GetCurrentScreen()
{
	return GUI_currentScreen;
}

void GUI_InitHardware()
{
	GUI_TS_LCD_InitHardware();
}
