#include "guilib.h"
#include "guiprivate.h"

static GUI_BOOL checkViewTouch(View *view, u16 x, u16 y)
{
	if(view->visible==GUI_TRUE && view->X<=x && view->Y<=y && (view->X+view->width)>x && (view->Y+view->height)>y)
		return GUI_TRUE;
	else
		return GUI_FALSE;
}

static GUITouchEventReturn viewHandleTouch(View *view, u16 x, u16 y, GUITouchEventType type)
{
	if(view->contextMenu && type==GUITS_ContextMenu) {
		GUI_drawContextMenu(view);
		view->isClicked = GUI_FALSE;
		if(view->type==ViewComboBox)
			view->isCheackable = GUI_FALSE;
		guiEvent(view, GUITS_ContextMenu);
		return GUITS_NotInteresting;
	}
	switch(view->type){
	case ViewLabel:
	case ViewProgressBar:
		return GUITS_NotInteresting;
	case ViewContextMenuItem:
	{
		switch(type){
		case GUITS_Down:
			view->isClicked = GUI_TRUE;
			GUI_drawView(view);
			break;
		case GUITS_Up:
		{
			view->isClicked = GUI_FALSE;
			GUI_BOOL event = GUI_FALSE;
			event=checkViewTouch(view, x,y);
			if(event==GUI_TRUE) {
				if(view->isCheackable==GUI_TRUE){
					if(view->value == GUI_FALSE)
						view->value = GUI_TRUE;
					else
						view->value = GUI_FALSE;
				}
			}
			GUI_drawView(view);
			if(event==GUI_TRUE && view->isCheackable==GUI_FALSE) {
				GUI_drawCloceContextMenu();
				guiEvent(view, GUITS_ContextMenu);
			}
			break;
		}
		case GUITS_Move:
		{
			GUI_BOOL res = checkViewTouch(view, x,y);
			GUI_BOOL old = view->isClicked;
			view->isClicked = res;
			if(old!=res) {
				GUI_drawView(view);
			}
			break;
		}
		default:
			;
		}
		return GUITS_InterestingMovementAndUp;
	}
	case ViewButton:
	{
		switch(type){
		case GUITS_Down:
			view->isClicked = GUI_TRUE;
			GUI_drawView(view);
			guiEvent(view, type);
			break;
		case GUITS_Up:
		{
			view->isClicked = GUI_FALSE;
			GUI_BOOL event = GUI_FALSE;
			event=checkViewTouch(view, x,y);
			if(event==GUI_TRUE) {
				if(view->isCheackable==GUI_TRUE){
					if(view->value == GUI_FALSE)
						view->value = GUI_TRUE;
					else
						view->value = GUI_FALSE;
				}
			}
			GUI_drawView(view);
			if(event==GUI_TRUE)
				guiEvent(view, type);
			break;
		}
		case GUITS_Move:
		{
			GUI_BOOL res = checkViewTouch(view, x,y);
			GUI_BOOL old = view->isClicked;
			view->isClicked = res;
			if(old!=res) {
				GUI_drawView(view);
				guiEvent(view, type);
			}
			break;
		}
		default:
			;
		}
		return GUITS_InterestingMovementAndUp;
	}
	case ViewSlider:
	{
		GUI_BOOL draw = GUI_FALSE;
		switch(type){
		case GUITS_Down:
			view->isClicked = GUI_TRUE;
			draw = GUI_TRUE;
			break;
		case GUITS_Up:
			view->isClicked = GUI_FALSE;
			GUI_drawView(view);
			return GUITS_InterestingMovementAndUp;
		case GUITS_Move:
			break;
		default:
			return GUITS_InterestingMovementAndUp;
		}
		GUI_BOOL changed = GUI_FALSE;
		if(x<view->X+GUI_SLIDER_SLIDE_WIDTH/2) {
			if(view->value!=0) {
				changed = GUI_TRUE;
				view->value =  0;
			}
		} else if(x>view->X+view->width-GUI_SLIDER_SLIDE_WIDTH/2) {
			if(view->value!=GUI_SLIDER_MAX_VALUE) {
				changed = GUI_TRUE;
				view->value =  GUI_SLIDER_MAX_VALUE;
			}
		} else {
			u16 newValue = (x - view->X - GUI_SLIDER_SLIDE_WIDTH/2)*GUI_SLIDER_MAX_VALUE/(view->width-GUI_SLIDER_SLIDE_WIDTH);
			if(view->value!=newValue) {
				changed = GUI_TRUE;
				view->value =  newValue;
			}
		}

		if(draw==GUI_TRUE || changed==GUI_TRUE)
			GUI_drawView(view);
		if(changed)
			guiEvent(view, type);
		return GUITS_InterestingMovementAndUp;
	}
	case ViewComboBox:
	{
		switch(type){
		case GUITS_Down:
			if((x-view->X)<view->width/2 && (x-view->X)<GUI_COMBOBOX_SWITCH_WIDTH*2) {
				GUI_BOOL changed = GUI_TRUE;
				view->isClicked = GUI_TRUE;
				if(view->value>0)
					view->value--;
				else
					changed = GUI_FALSE;
				GUI_drawView(view);
				if(changed == GUI_TRUE)
					guiEvent(view, type);
			} else if(x>(view->X+view->width/2) && x>(view->X+view->width-GUI_COMBOBOX_SWITCH_WIDTH*2)) {
				GUI_BOOL changed = GUI_TRUE;
				view->isCheackable = GUI_TRUE;
				u16 count = 0;
				const char *test = view->text;
				while ((*test)!='\0' || (*(test+1))!='\0'){
					if((*test)=='\0')
						count++;
					test++;
				}
				if(view->value<count)
					view->value++;
				else
					changed = GUI_FALSE;
				GUI_drawView(view);
				if(changed == GUI_TRUE)
					guiEvent(view, type);
			}
			break;
		case GUITS_Up:
			view->isClicked = GUI_FALSE;
			view->isCheackable = GUI_FALSE;
			GUI_drawView(view);
			break;
		case GUITS_Move:
			break;
		default:
			;
		}
		return GUITS_InterestingUpOnly;
	}
	}
	return GUITS_NotInteresting;
}

View *lastTouchedView = 0;

GUITouchEventReturn GUI_HandleTouch(u16 x, u16 y, GUITouchEventType type)
{
	if(type==GUITS_DrawCalibration) {
		int i;
		GUI_LCD_Clear(White);
		GUI_LCD_Print(70, 90, "Touch screen calibration", &lucidaConsole_12pt, Black);
		// kind of animation :) to notice a point on screen
		GUI_LCD_Rectangle(x-GUI_CALIBRATION_DRAW_SIZE, y-GUI_CALIBRATION_DRAW_SIZE, GUI_CALIBRATION_DRAW_SIZE*2, GUI_CALIBRATION_DRAW_SIZE*2, Red, Red);
		for(i=0; i<1800000; i++);
		GUI_LCD_Rectangle(x-GUI_CALIBRATION_DRAW_SIZE, y-GUI_CALIBRATION_DRAW_SIZE, GUI_CALIBRATION_DRAW_SIZE*2, GUI_CALIBRATION_DRAW_SIZE*2, White, White);

		GUI_LCD_Line(x-GUI_CALIBRATION_DRAW_SIZE, y, x+GUI_CALIBRATION_DRAW_SIZE, y, Blue);
		GUI_LCD_Line(x, y-GUI_CALIBRATION_DRAW_SIZE, x, y+GUI_CALIBRATION_DRAW_SIZE, Blue);
		return GUITS_NotInteresting;
	} else if(type==GUITS_FinishCalibration) {
		if(GUI_currentScreen==0)
			GUI_LCD_Clear(White);
		else
			GUI_repaintScreen();
		return GUITS_NotInteresting;
	}

	if(type!=GUITS_Down && lastTouchedView!=0)
		return viewHandleTouch(lastTouchedView, x, y, type);

	if(GUI_currentScreen==0)
		return GUITS_NotInteresting;

	View *next = GUI_viewContextMenu?GUI_viewContextMenu->contextMenu:GUI_currentScreen->firstView;
	while(next) {
		if(checkViewTouch(next,x,y)==GUI_TRUE) {
			if(type==GUITS_Down)
				lastTouchedView = next;
			return viewHandleTouch(next, x, y, type);
		}
		next = next->nextView;
	}
	if(GUI_viewContextMenu)
		GUI_drawCloceContextMenu();
	return GUITS_NotInteresting;
}
