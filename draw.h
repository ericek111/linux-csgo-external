#pragma once

#include <iostream>
#include <vector>
#include <bitset>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h>
#include <sys/time.h>
#include <climits>
#include <chrono>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <math.h>
#include <sstream>
#include <string>

using namespace std;

#define ALIGN_LEFT 0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT 2

// Events for normal windows
#define BASIC_EVENT_MASK (StructureNotifyMask|ExposureMask|PropertyChangeMask|EnterWindowMask|LeaveWindowMask|KeyPressMask|KeyReleaseMask|KeymapStateMask)
#define NOT_PROPAGATE_MASK (KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|ButtonMotionMask)
#define SHAPE_MASK ShapeNotifyMask

#define FPSMETERSAMPLE 100
#define CLEARSCREEN_ITERATIONS 2000

// http://www-h.eng.cam.ac.uk/help/tpl/graphics/X/X11R5/node16.html

class Draw {

	public:
	Draw();
	void drawString(const char * text, int x, int y, XColor fgcolor, XColor bgcolor, int align);
	void clearArea(int x, int y, int width, int height);
	void fillRectangle(int x, int y, int width, int height, XColor color);
	void drawLine(int x1, int y1, int x2, int y2, XColor color);
	XColor createXColorFromRGBA(short red, short green, short blue, short alpha);
	XColor createXColorFromRGB(short red, short green, short blue);
	void addCaptureArea(XRectangle rect);
	void clearCaptureAreas();
	void createShapedWindow();
	void openDisplay();
	void allow_input_passthrough (Window w);
	void list_fonts();
	void init();
	void halt();
	void toggleoverlay(bool state);
	void toggleoverlay();
	void startCaptureInput();
	void stopCaptureInput();
	void setCaptureInput(bool state);
	void clearscreen();
	void startdraw();
	void enddraw();
	Display *g_display;
	int      g_screen;
	Window   g_win;
	int      g_disp_width;
	int      g_disp_height;
	Colormap g_colormap;
	bool overlayenabled = false;
	bool overlayavailable = false;
	bool doesCaptureInput = false;
	std::vector<XRectangle> captureAreas;
	GC       gc;
	XGCValues   gcv;
	XFontStruct * font;
	XColor red, black, blacka, blackla, white, transparent, ltblue, blue, yellow, grey, ltgrey, ltred, ltyellow, green, blackma;

	const char * font_name = "9x15bold";
	const int font_width = 9;
	const int font_height = 15;

	// The window size
	int WIDTH  = 1920;
	int HEIGHT = 1080;

	int posx = 0;
	int posy = 0;


	private:
	long event_mask = (StructureNotifyMask|ExposureMask|PropertyChangeMask|EnterWindowMask|LeaveWindowMask|KeyRelease|ButtonPress|ButtonRelease|KeymapStateMask);
	int shape_event_base;
	int shape_error_base;
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	int fpsmeterc = 0;
	uint64_t duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
	string fpsstring = "";
	int renderi = 0;
};