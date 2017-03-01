#ifndef ECOMPONENT_H
#define ECOMPONENT_H

#include "../draw.hpp"
#include "ETheme.hpp"

using namespace std;

class EComponent {
	public:
		EComponent(Draw* cdraw, ETheme* ctheme): draw( cdraw ), theme( ctheme ) {};
		void onMouseEvent(XEvent xe);
		virtual void onDraw(int ox, int oy);
		virtual void init();
		int width = 300;
		int height = 300;
		int posx = -1;
		int posy = -1;
		Draw* draw;
		ETheme* theme;

};

#endif // ECOMPONENT_H