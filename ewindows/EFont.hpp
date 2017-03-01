#ifndef EFONT_H
#define EFONT_H

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class EFont {
	public:
		EFont(Display *cg_display, const char * cfont_name, int cfont_width, int cfont_height);
		void startdraw(Display *g_display);
		void enddraw();
		void init();
		const char * font_name;
		int font_width;
		int font_height;
		Display *g_display;
		XFontStruct * font;
		int fid;

	private:
};

#endif // EFONT_H