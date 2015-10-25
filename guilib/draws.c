#include "guilib.h"
#include "lcdProxy.h"
#include "guiprivate.h"

Screen * screenDrawCurrent = 0;
#define SCREENBACKGROUND (View*)-1
#define CONTEXTMENUACKGROUND (View*)-2

void GUI_repaintScreen()
{
	if(screenDrawCurrent || GUI_currentScreen==0)
		return;
	do {
		screenDrawCurrent = GUI_currentScreen;
		GUI_drawView(SCREENBACKGROUND);
		View *next = screenDrawCurrent->firstView;
		while(next && GUI_currentScreen==screenDrawCurrent) {
			GUI_drawView(next);
			next = next->nextView;
		}
		if(GUI_viewContextMenu) {
			GUI_drawView(CONTEXTMENUACKGROUND);
			View *next = GUI_viewContextMenu->contextMenu;
			while(next && GUI_currentScreen==screenDrawCurrent) {
				GUI_drawView(next);
				next = next->nextView;
			}
		}
	} while(GUI_currentScreen!=screenDrawCurrent);
	screenDrawCurrent = 0;
}

void GUI_drawScreen(Screen *screen) // thread safe
{
	if(GUI_currentScreen==screen)
		return;
	if(screen!=&GUI_contextMenuScreen)
		GUI_viewContextMenu = 0;
	GUI_currentScreen = screen;
	GUI_repaintScreen();
}

static void drawText(u16 x, u16 y, u16 width, u16 height, const FONT_INFO *font, u16 color, ViewTextAlign align, const char *text)
{
	switch(align) {
	case AlignCenter:
		x += (width - FONTS_GetTextWidth(font, text))/2;
		y += (height - font->charHeight)/2;
		break;
	case AlignRightVCenter:
		x += width - FONTS_GetTextWidth(font, text);
		y += (height - font->charHeight)/2;
		break;
	case AlignLeftTop:
		y += GUI_DEFAULT_PADDING;
		break;
	case AlignLeftVCenter:
	default:
		y += (height - font->charHeight)/2;
	}
	GUI_LCD_Print(x, y, text, font, color);
}

static GUI_BOOL isViewFromContextMenu(View *view)
{
	if(GUI_viewContextMenu) {
		View *v = GUI_viewContextMenu->contextMenu;
		while(v) {
			if(v==view)
				return GUI_TRUE;
			v = v->nextView;
		}
	}
	return GUI_FALSE;
}

static GUI_BOOL isViewFromCurrentScreen(View *view)
{
	if(GUI_currentScreen) {
		View *v = GUI_currentScreen->firstView;
		while(v) {
			if(v==view)
				return GUI_TRUE;
			v = v->nextView;
		}
	}
	return isViewFromContextMenu(view);
}

u16 GUI_getDarkColor(u16 color)
{
	u16  r, g, b;
	r = (color>>0)  & 0x1f;
	g = (color>>5)  & 0x3f;
	b = (color>>11) & 0x1f;
	r = r/2; g = g/2; b = b/2;
	return  (b<<11) + (g<<5) + (r<<0);
}

static void GUI_drawViewUnSafe(View *view)
{
	if(GUI_currentScreen==0)
		return;
	if(view == SCREENBACKGROUND) {
		if(GUI_viewContextMenu)
			GUI_LCD_Clear(GUI_getDarkColor(GUI_currentScreen->bgColor));
		else
			GUI_LCD_Clear(GUI_currentScreen->bgColor);
		return;
	}
	if(view == CONTEXTMENUACKGROUND) {
		if(GUI_viewContextMenu) {
			if(GUI_viewContextMenu->contextMenu) {
				u16 x = GUI_viewContextMenu->contextMenu->X - GUI_DEFAULT_PADDING*4;
				u16 y = GUI_viewContextMenu->contextMenu->Y - GUI_DEFAULT_PADDING*4;
				GUI_LCD_Rectangle(x, y, GUI_SCREEN_WIDTH-x*2, GUI_SCREEN_HEIGHT-y*2, GUI_currentScreen->bgColor, GUI_currentScreen->bgColor);
			}
		}
		return;
	}
	u16 bgcolor = GUI_currentScreen->bgColor;
	u16 vcolor = view->color;
	u16 dcolor =  GUI_getDarkColor(vcolor);
	if(GUI_viewContextMenu) {
		if(isViewFromContextMenu(view)==GUI_FALSE) {
			bgcolor = GUI_getDarkColor(bgcolor);
			vcolor = GUI_getDarkColor(vcolor);
			dcolor = GUI_getDarkColor(dcolor);
		}
	}

	GUI_LCD_Rectangle(view->X, view->Y, view->width, view->height, bgcolor, bgcolor);

	if(view->visible==GUI_FALSE){
		return;
	}

	switch(view->type){
	case ViewLabel:
		drawText(view->X, view->Y, view->width, view->height, view->font, vcolor, view->align, view->text);
		break;
	case ViewContextMenuItem:
	case ViewButton:
	case ViewProgressBar:
	{
		u16 x = view->X, y=view->Y, w=view->width-GUI_BUTTON_MOVE_PX, h=view->height-GUI_BUTTON_MOVE_PX;
		u16 color = vcolor;
		if(view->isClicked==GUI_TRUE){
			color = dcolor;
			x+=GUI_BUTTON_MOVE_PX;
			y+=GUI_BUTTON_MOVE_PX;
		}
		GUI_LCD_Rectangle(x, y, w, h,color,Transparent);
		if(view->isCheackable==GUI_FALSE || (view->isCheackable==GUI_TRUE && (view->value==GUI_TRUE || view->isClicked==GUI_TRUE)) ) {
			u16 dw=w-4;
			if(view->type==ViewProgressBar)
				dw=(view->value>GUI_PROGRESSBAR_MAX_VALUE)?dw:(dw*view->value/GUI_PROGRESSBAR_MAX_VALUE);
			GUI_LCD_Rectangle(x+2, y+2, dw, h-4,color,color);
		}
		drawText(x+GUI_DEFAULT_PADDING*2, y+GUI_DEFAULT_PADDING*2, \
				w-GUI_DEFAULT_PADDING*4, h-GUI_DEFAULT_PADDING*4, \
				view->font, GUI_BUTTON_TEXT_COLOR, view->align, view->text);
		break;
	}
	case ViewSlider:
	{
		GUI_LCD_Rectangle(view->X, view->Y+view->height/4, view->width,view->height/2,vcolor, dcolor);
		u32 sliderPos = view->X+(view->width-GUI_SLIDER_SLIDE_WIDTH)*((view->value>GUI_SLIDER_MAX_VALUE)?GUI_SLIDER_MAX_VALUE:view->value)/GUI_SLIDER_MAX_VALUE;
		if(view->isClicked==GUI_FALSE)
			dcolor = vcolor;
		GUI_LCD_Rectangle(sliderPos,view->Y,GUI_SLIDER_SLIDE_WIDTH,view->height,vcolor, dcolor);
		break;
	}
	case ViewComboBox:
	{
		u16 x0,x1,y0,y1;
		//GUI_LCD_Rectangle(view->X,view->Y, GUI_COMBOBOX_SWITCH_WIDTH,view->height, vcolor, Transparent);
		if(view->isClicked)
			GUI_LCD_Rectangle(view->X,view->Y, GUI_COMBOBOX_SWITCH_WIDTH,view->height, dcolor, dcolor);
		x0=view->X+GUI_DEFAULT_PADDING; y0=view->Y+view->height/2; x1 = x0+GUI_COMBOBOX_SWITCH_WIDTH-2*GUI_DEFAULT_PADDING; y1 = view->Y+GUI_DEFAULT_PADDING;
		GUI_LCD_Line(x0,y0,x1,y1,vcolor);
		GUI_LCD_Line(x0+1,y0+1,x1,y1+1,vcolor);
		y1=view->Y+view->height-1-GUI_DEFAULT_PADDING;
		GUI_LCD_Line(x0,y0,x1,y1,vcolor);
		GUI_LCD_Line(x0+1,y0-1,x1,y1-1,vcolor);

		const char *text = view->text;
		u32 i;
		for(i=view->value; i>0; i--) {
			while ((*text++)!='\0');
		}
		drawText(view->X+GUI_DEFAULT_PADDING+GUI_COMBOBOX_SWITCH_WIDTH, view->Y+GUI_DEFAULT_PADDING, \
				view->width-GUI_DEFAULT_PADDING*2-GUI_COMBOBOX_SWITCH_WIDTH*2, view->height-GUI_DEFAULT_PADDING*2, \
				view->font, vcolor, view->align, text);

		x0 = view->X+view->width-1-GUI_COMBOBOX_SWITCH_WIDTH;
		//GUI_LCD_Rectangle(x0,view->Y, GUI_COMBOBOX_SWITCH_WIDTH,view->height, vcolor, Transparent);
		if(view->isCheackable)
			GUI_LCD_Rectangle(x0,view->Y, GUI_COMBOBOX_SWITCH_WIDTH,view->height, dcolor, dcolor);
		x0+=GUI_DEFAULT_PADDING;
		y0=view->Y+GUI_DEFAULT_PADDING; x1 = x0+GUI_COMBOBOX_SWITCH_WIDTH-2*GUI_DEFAULT_PADDING; y1 = view->Y+view->height/2;
		GUI_LCD_Line(x0,y0,x1,y1,vcolor);
		GUI_LCD_Line(x0,y0+1,x1-1,y1+1,vcolor);
		y0=view->Y+view->height - 1-GUI_DEFAULT_PADDING;
		GUI_LCD_Line(x0,y0,x1,y1,vcolor);
		GUI_LCD_Line(x0,y0-1,x1-1,y1-1,vcolor);
		break;
	}
	}
}

int viewDrawQueuePointerSet = 0;
int viewDrawQueuePointerGet = 0;
#define GUI_QUEUE__SIZE 32
View *viewDrawQueue[GUI_QUEUE__SIZE];
char viewDrawQueueWorking = 0;

void GUI_drawView(View *view) // thread safe
{
	if(view!=SCREENBACKGROUND && view!=CONTEXTMENUACKGROUND && isViewFromCurrentScreen(view)==GUI_FALSE)
		return;
	if(GUI_viewContextMenu && view->type!=ViewContextMenuItem)
		return;

	viewDrawQueue[viewDrawQueuePointerSet] = view;
	viewDrawQueuePointerSet++;
	if(viewDrawQueuePointerSet>=GUI_QUEUE__SIZE)
		viewDrawQueuePointerSet = 0;
	if(viewDrawQueueWorking)
		return;

	viewDrawQueueWorking = 1;
	while(viewDrawQueuePointerGet!=viewDrawQueuePointerSet) {
		View *v = viewDrawQueue[viewDrawQueuePointerGet];
		viewDrawQueuePointerGet++;
		if(viewDrawQueuePointerGet>=GUI_QUEUE__SIZE)
			viewDrawQueuePointerGet = 0;
		GUI_drawViewUnSafe(v);
	}
	viewDrawQueueWorking = 0;
}

void GUI_drawContextMenu(View *view)
{
	if(view->contextMenu==0 || &GUI_contextMenuScreen == GUI_currentScreen)
		return;
	GUI_contextMenuReturnScreen = GUI_currentScreen;
	GUI_contextMenuScreen.bgColor = GUI_currentScreen->bgColor;
	GUI_contextMenuScreen.firstView = GUI_currentScreen->firstView;
	GUI_viewContextMenu = view;
	GUI_drawScreen(&GUI_contextMenuScreen);
}

void GUI_drawCloceContextMenu()
{
	if(GUI_viewContextMenu) {
		GUI_viewContextMenu = 0;
		GUI_drawScreen(GUI_contextMenuReturnScreen);
	}
}
