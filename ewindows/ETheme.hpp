#ifndef ETHEME_H
#define ETHEME_H

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class ETheme {
	public:
		ETheme();
		void init();
		int g_screen;

		XColor window_background;
		XColor window_border;
		XColor window_titlebg;
		XColor window_title;
		XColor text_main;
		XColor text_inactive;
		XColor text_label;

		XColor but_border;
		XColor but_text;
		XColor but_bg;
		XColor but_hover_text;
		XColor but_hover_bg;
		XColor but_hover_border;
		XColor but_pressed_text;
		XColor but_pressed_bg;
		XColor but_pressed_border;

		int titleheight = 19;
		int border_top = 2;
		int border_right = 2;
		int border_bottom = 2;
		int border_left = 2;

	private:
		XColor createXColorFromRGBA(short red, short green, short blue, short alpha);
		Display *g_display;

};

#endif // ETHEME_H