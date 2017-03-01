#ifndef DRAWMANAGER_H
#define DRAWMANAGER_H

#include "draw.hpp"

// Events for normal windows
#define BASIC_EVENT_MASK (StructureNotifyMask|ExposureMask|PropertyChangeMask|EnterWindowMask|LeaveWindowMask|KeyPressMask|KeyReleaseMask|KeymapStateMask)
#define NOT_PROPAGATE_MASK (KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|ButtonMotionMask)
#define SHAPE_MASK ShapeNotifyMask

#define FPSMETERSAMPLE 100
#define CLEARSCREEN_ITERATIONS 2000

using namespace std;

class DrawManager {
	public:
	DrawManager();

	XColor createXColorFromRGBA(short red, short green, short blue, short alpha);
    XColor createXColorFromRGB(short red, short green, short blue);

    void drawString(const char * text, int x, int y, XColor fgcolor, XColor bgcolor, int align);
    void clearArea(int x, int y, int width, int height);
    void fillRectangle(int x, int y, int width, int height, XColor color);
    void clearscreen();
    void toggleoverlay(bool state);
    void toggleoverlay();

	void startdraw();
	void enddraw();
	void init();
	void halt();
   	void list_fonts();
    void startCaptureInput();
    void stopCaptureInput();
    void setCaptureInput(bool state);

    Display *g_display;
    int      g_screen;
    Window   g_win;
    int      g_disp_width;
    int      g_disp_height;
    Colormap g_colormap;

    //EFontsManager *efm;
    bool overlayenabled = true;
    bool doesCaptureInput = false;


	XColor bgcolor;
	XColor red, black, blacka, blackla, white, transparent, ltblue, blue, yellow, grey, ltgrey, ltred, ltyellow;

	int font_width;
	int font_height;
	int WIDTH  = 1920;
   	int HEIGHT = 1080;
    int DEFX = 0;
    int DEFY = 0;
   	XFontStruct *font;
	Draw *draw;

	private:
	void createShapedWindow();
    bool openDisplay();
    void allow_input_passthrough (Window w);

    long event_mask = (StructureNotifyMask|ExposureMask|PropertyChangeMask|EnterWindowMask|LeaveWindowMask|KeyRelease | ButtonPress|ButtonRelease|KeymapStateMask);
    int shape_event_base;
    int shape_error_base;
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    int fpsmeterc = 0;
    uint64_t duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    string fpsstring = "";
    int renderi = 0;

};

#endif // DRAWMANAGER_H