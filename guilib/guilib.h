#ifndef _GUI_LIB_H
#define _GUI_LIB_H

#include "lcdProxy.h"
#include <stm32f10x.h> // for types like u8, u16 etc

typedef enum {
	GUI_FALSE = 0,
	GUI_TRUE = 1
} GUI_BOOL;

typedef enum {
	ViewLabel,
	ViewButton,
	ViewProgressBar,
	ViewSlider,
	ViewComboBox,
	ViewContextMenuItem
} ViewType;

typedef enum {
	AlignCenter,
	AlignRightVCenter,
	AlignLeftVCenter,
	AlignLeftTop
} ViewTextAlign;

typedef struct tView{
	// linked list for elements on screen
	struct tView *nextView;
	// postion
	u16 X;
	u16 Y;
	u16 width;
	u16 height;
	// type of View see ViewType
	ViewType type;
	// text aligment
	ViewTextAlign align;
	// text for button, labels splitted by \0x00 for combobox
	const char * text;
	// button option
	GUI_BOOL isCheackable; // for combobox 'isClicked' of right button
	GUI_BOOL isClicked; // private, don't touch - for interval use
	GUI_BOOL visible;
	// button check state, slider value, combobox item number, progressbar progress
	u32 value;
	// font for label and button
	const FONT_INFO *font;
	u16 color; // color for text and body for slider
	struct tView *contextMenu;
} View;

typedef struct {
	u16 bgColor;
	View *firstView;
} Screen;

#define GUI_CONTENT 65535
#define GUI_DEFAULT_PADDING 3
#define GUI_SLIDER_SLIDE_WIDTH 24
#define GUI_COMBOBOX_SWITCH_WIDTH 24
#define GUI_BUTTON_MOVE_PX 1
#define GUI_CALIBRATION_DRAW_SIZE 9

#define GUI_SLIDER_MAX_VALUE 100
#define GUI_PROGRESSBAR_MAX_VALUE 100
#define GUI_BUTTON_TEXT_COLOR Black

void GUI_InitHardware();

Screen *GUI_GetCurrentScreen();
void GUI_initScreen(Screen *screen, u16 color);
void GUI_initLabel(View *view, Screen *screen, const char *text, const FONT_INFO *font, u16 color, u16 x, u16 y, u16 width, u16 height, ViewTextAlign align);
void GUI_initButton(View *view, Screen *screen, const char *text, const FONT_INFO *font, u16 color, GUI_BOOL cheackable, u16 x, u16 y, u16 width, u16 height, ViewTextAlign align);
void GUI_initProgressBar(View *view, Screen *screen, const char *text, const FONT_INFO *font, u16 color, u16 x, u16 y, u16 width, u16 height, ViewTextAlign align);
void GUI_initSlider(View *view, Screen *screen, u16 color, u16 x, u16 y, u16 width, u16 height);
void GUI_initComboBox(View *view, Screen *screen, const char *text, const FONT_INFO *font, u16 color, u16 x, u16 y, u16 width, u16 height, ViewTextAlign align);
void GUI_initContextMenuItem(View *item, View *view, const char *text, const FONT_INFO *font, u16 color);

void GUI_drawScreen(Screen *screen);
void GUI_repaintScreen();
void GUI_drawView(View *view);
void GUI_drawContextMenu(View *view);
void GUI_drawCloceContextMenu();

extern void guiEvent(View *view, GUITouchEventType type);

#endif //_GUI_LIB_H
