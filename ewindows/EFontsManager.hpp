#ifndef EFONTSMANAGER_H
#define EFONTSMANAGER_H

#include <X11/Xlib.h>
#include <vector>
#include "EFont.hpp"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class EFontsManager {
	public:
		EFontsManager(Display *cg_display): g_display( cg_display ) {};
		void startdraw();
		void enddraw();
		void init();
		void reloadFonts();
		void registerFont(const char * font_name, int font_width, int font_height);
		void list_fonts();
		EFont* getFont(int i);
		EFont* getFont();
		std::vector<EFont*> efonts;
		Display *g_display;

	private:
};

#endif // EFONTSMANAGER_H