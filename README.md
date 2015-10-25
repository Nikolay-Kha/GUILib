GUI Library for STM32 microcontrollers. Can draw nice controls on your embedded device. Examples are written with CooCox ( http://www.coocox.org/ )

Features:  
- Light, simple, colorful, reliable.  
- Support LCDs and Touchscreens.  
- Multiple screen support.  
- Controls: label, button, progressbar, slider, combobox, checkbox.  
- Context menu support.  
- Press and release events.  
- Touch screen calibration support.  
- 3 fonts.  
- Crossplatofrm architecture.  

![](examples/photo1.jpg?raw=true)

# Video demo
https://youtu.be/sASculXDCO0

# Usage
Attach library to your project (see FAQ section below). Include guilib/guilib.h. Mostly all that you need in this file. Now you can create one or few "screen" with function:  
void GUI_initScreen(Screen *screen, u16 color);  
Screen uses for grouping other controls and simultaneous displaying on LCD. Screens also have background color. You can create few screens and switch between them with calling function:  
GUI_drawScreen(Screen *screen);  
Each screen contains child objects: controls. This controls are called views.You can create control with calling functions:  
void GUI_initXXX(View *view, Screen *screen, ...);
where XXX is a type of view. See examples and sources for details which arguments for each type can be passed. This view would be associated with screen and it draws with screen. You can create few views on one screen. If you need to update view (for example change text of the label) call:
void GUI_drawView(View *view);  
All views possible options are in View struct. See guilib.h for details.
To handle button events implement this function:  
void guiEvent(View *view, GUITouchEventType type);  
Each call of this function means that user is interracted with control. To determine which view causes event, compare a pointer to view. See GUITouchEventType for determine what event is hapend.
Each view also main contain child context menu. To add items to this menu call:  
void GUI_initContextMenuItem(View *item, View *view, char *text, const FONT_INFO *font, u16 color);  
but context menu is not shown automatically. To show it, call:  
void GUI_drawContextMenu(View *view);  
The same thing with closing it, call:  
void GUI_drawCloceContextMenu();  
Best practise would be catch TS_LongPress event (or it's renames in lcdProxy.h to GUITS_ContextMenu) to show it. Closing can be applyed on contex item (which is actualy another view) release event.

# Supported hardware
The main part of the library is written on pure C and can be used almost with any MCU. But library needs driver for LCD and touchscreen. Current driver implementation supports:  
TSC2046 touchscreen  
ILI9325 and ILI9344 LCD  
To connect to this devices library implements hardware access layer (HAL) for SPI and FSMC interfaces for stm32f103x MCUs. See below if you want to use library with your MCU.

# Overall diagram
This scheme provides internal structure of library. You can use this information if you want to add another hardware support.
![](overview.png?raw=true)
Hardware - it can be whatever you want which can be connected to your device in electrical way.  
HAL - hardware access layer for your hardware interface, i.e driver for SPI or I2C or any other bus.  
halProxy - connector between HAL and driver. Reimplementing this provides ability yo use another hardware interface.  
Driver - driver for touchscreen or LCD that can send command to hardware via HAL to draw some graphic primitives and catch touch events.  
lcdProxy - connector between the drivers and guilib. Reimplementing this part allows to connect any other hardware with any other driver structure for any other MCU.  
guilib - the main part of library. Draws controls. It is written on pure C and can be attach alone to the project with another set of hardware drivers.  
App - application whic uses library.  

# FAQ
Q: How can I add this libto project?  
A: You can copy one of example projects, remove everything that you don't need and use it as clear CooCox IDE project.  
But if you would like to create realy clean project:
1. attath *.c and *.h files from driver, fonts, guilib, hal to your project. 
2. Create halProxy.c, halProxy.h, lcdProxy.c and lcdProxy.h (or just copy it from example), implement functions and attach to project.
3. Implement somewhere in project this function:  
void guiEvent(View *view, GUITouchEventType type)  
and now you can start programming your own graphic interface.  

Q: How to change some view's value, for example label's text?  
A: Set pointer to new text in object->text field and repaint view with:  
void GUI_drawView(View *view);  

Q: Can I rotate the screen?  
A: Yes, you can. Translate cooridinates in lcdProxy.c  

Q: Which hardware are supported?  
A: Browse drivers directory to find out which hardware are supported. But you can always add your hardware(see above).  

Q: Can I use another hardware?  
A: Yes, sure but you need to implement functions in lcdProxy.c for calling methods for your driver. Just few methods are needed.  

Q: How to create checkbox?  
A: Create a button, and pass "cheackable" argument as true.  

Q: How to create ratiobutton?  
A: Create few checkboxes and handle it manually.  
 
Q: How to calibrate touchscreen?  
A: Call TSCalibrate() from driver (for example from tsc2046.h). It automatically display points for pressing and exit from this function only ofter calibration will be done or TSInteruptCallibration() is called. Then save results somewhere in flash with data from a pointer that is taken by calling:
TSGetCalibrationDataPtr()  
wtich size equal sizeof(TS_CALIBRATION_DATA). Now restore this data to the same pointer each boot.

Q: Can I draw something on screen?  
A: Yes, sure. Screen repaintg is completly up to you, so you can use space without view drawing anything you want with methods from LCD driver for example.  

Q: Do I need to calculate width and height each time manually for each view?  
A: No, you can use GUI_CONTENT constant and size will be chosen in optimal way automatically.

# Author
Nikolay Khabarov
