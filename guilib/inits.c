#include "guilib.h"

static void GUI_AddViewToScreen(Screen *screen, View *view)
{
	if(screen->firstView==0) {
		screen->firstView = view;
	} else {
		View *next = screen->firstView;
		while(next->nextView)
			next = next->nextView;
		next->nextView = view;
	}
}

void GUI_initScreen(Screen *screen, u16 color)
{
	screen->bgColor = color;
	screen->firstView = 0;
}

void GUI_initLabel(View *view, Screen *screen, const char *text, const FONT_INFO *font, u16 color, u16 x, u16 y, u16 width, u16 height, ViewTextAlign align)
{
	if(height==GUI_CONTENT)
		height = font->charHeight;
	if(width==GUI_CONTENT)
		width = FONTS_GetTextWidth(font, text);
	view->X = x;
	view->Y = y;
	view->align = align;
	view->height = height;
	view->isCheackable = GUI_FALSE;
	view->isClicked = GUI_FALSE;
	view->nextView = 0;
	view->text = text;
	view->type = ViewLabel;
	view->value = 0;
	view->width = width;
	view->visible = GUI_TRUE;
	view->font = font;
	view->color = color;
	view->contextMenu = 0;
	GUI_AddViewToScreen(screen, view);
}

void GUI_initButton(View *view, Screen *screen, const char *text, const FONT_INFO *font, u16 color, GUI_BOOL cheackable, u16 x, u16 y, u16 width, u16 height, ViewTextAlign align)
{
	if(height==GUI_CONTENT)
			height = font->charHeight+GUI_DEFAULT_PADDING*4+GUI_BUTTON_MOVE_PX;
	if(width==GUI_CONTENT)
			width = FONTS_GetTextWidth(font, text)+GUI_DEFAULT_PADDING*4+GUI_BUTTON_MOVE_PX;
	view->X = x;
	view->Y = y;
	view->align = align;
	view->height = height;
	view->isCheackable = cheackable;
	view->isClicked = GUI_FALSE;
	view->nextView = 0;
	view->text = text;
	view->type = ViewButton;
	view->value = 0;
	view->width = width;
	view->visible = GUI_TRUE;
	view->font = font;
	view->color = color;
	view->contextMenu = 0;
	GUI_AddViewToScreen(screen, view);
}

void GUI_initProgressBar(View *view, Screen *screen, const char *text, const FONT_INFO *font, u16 color, u16 x, u16 y, u16 width, u16 height, ViewTextAlign align)
{
	if(height==GUI_CONTENT)
			height = font->charHeight+GUI_DEFAULT_PADDING*4+GUI_BUTTON_MOVE_PX;
	if(width==GUI_CONTENT)
			width = FONTS_GetTextWidth(font, text)+GUI_DEFAULT_PADDING*8+GUI_BUTTON_MOVE_PX;
	view->X = x;
	view->Y = y;
	view->align = align;
	view->height = height;
	view->isCheackable = GUI_FALSE;
	view->isClicked = GUI_FALSE;
	view->nextView = 0;
	view->text = text;
	view->type = ViewProgressBar;
	view->value = 0;
	view->width = width;
	view->visible = GUI_TRUE;
	view->font = font;
	view->color = color;
	view->contextMenu = 0;
	GUI_AddViewToScreen(screen, view);
}

void GUI_initSlider(View *view, Screen *screen, u16 color, u16 x, u16 y, u16 width, u16 height)
{
	if(height==GUI_CONTENT)
		height = 36;
	if(width==GUI_CONTENT)
		width = 150;
	view->X = x;
	view->Y = y;
	view->align = AlignCenter;
	view->height = height;
	view->isCheackable = GUI_FALSE;
	view->isClicked = GUI_FALSE;
	view->nextView = 0;
	view->text = 0;
	view->type = ViewSlider;
	view->value = 0;
	view->width = width;
	view->visible = GUI_TRUE;
	view->font = 0;
	view->color = color;
	view->contextMenu = 0;
	GUI_AddViewToScreen(screen, view);
}

void GUI_initComboBox(View *view, Screen *screen, const char *text, const FONT_INFO *font, u16 color, u16 x, u16 y, u16 width, u16 height, ViewTextAlign align)
{
	if(height==GUI_CONTENT)
		height = font->charHeight+GUI_DEFAULT_PADDING*2;
	if(width==GUI_CONTENT)
		width = FONTS_GetTextWidth(font, text) + GUI_DEFAULT_PADDING*2 + GUI_COMBOBOX_SWITCH_WIDTH*2;
	view->X = x;
	view->Y = y;
	view->align = align;
	view->height = height;
	view->isCheackable = GUI_FALSE;
	view->isClicked = GUI_FALSE;
	view->nextView = 0;
	view->text = text;
	view->type = ViewComboBox;
	view->value = 0;
	view->width = width;
	view->visible = GUI_TRUE;
	view->font = font;
	view->color = color;
	view->contextMenu = 0;
	GUI_AddViewToScreen(screen, view);
}

void GUI_initContextMenuItem(View *item, View *view, const char *text, const FONT_INFO *font, u16 color)
{
	item->height = font->charHeight+GUI_DEFAULT_PADDING*4+GUI_BUTTON_MOVE_PX;
	item->width = FONTS_GetTextWidth(font, text)+GUI_DEFAULT_PADDING*4+GUI_BUTTON_MOVE_PX;
	int maxWidth = item->width;
	int menuHeight = item->height;
	int xpos = (GUI_SCREEN_WIDTH - maxWidth)/2;
	View *next = view->contextMenu;
	if(next==0) {
		view->contextMenu = item;
	} else {
		while(next) {
			if(next->width>maxWidth) {
				maxWidth = next->width;
				xpos = (GUI_SCREEN_WIDTH - maxWidth)/2;
			}
			menuHeight += next->height;
			if(next->nextView==0) {
				next->nextView = item;
				next = 0;
			} else {
				next = next->nextView;
			}
		}
	}
	if(maxWidth<GUI_SCREEN_WIDTH/3) {
		maxWidth = GUI_SCREEN_WIDTH/3;
		xpos = GUI_SCREEN_WIDTH/3;
	}
	int ypos = (GUI_SCREEN_HEIGHT-menuHeight)/2;
	next = view->contextMenu;
	while(next) {
		next->width = maxWidth;
		next->X = xpos;
		next->Y = ypos;
		ypos += next->height;
		next = next->nextView;
	}

	item->align = AlignCenter;
	item->isCheackable = GUI_FALSE;
	item->isClicked = GUI_FALSE;
	item->nextView = 0;
	item->text = text;
	item->type = ViewContextMenuItem;
	item->value = 0;
	item->visible = GUI_TRUE;
	item->font = font;
	item->color = color;
	item->contextMenu = 0;
}

