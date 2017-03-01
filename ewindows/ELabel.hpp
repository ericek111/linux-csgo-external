#ifndef ELABEL_H
#define ELABEL_H

#include <vector>
#include "../draw.hpp"
#include "ETheme.hpp"
#include "EComponent.hpp"

using namespace std;

class ELabel : public EComponent {
	public:
		ELabel(Draw* cdraw, ETheme* ctheme): EComponent( cdraw, ctheme ) {};
		void onMouseEvent(XEvent xe);
		void onDraw(int ox, int oy);
		XColor fgcolor;
		XColor bgcolor;
		const char * text = "";
		int align = ALIGN_CENTER;
		ETheme* theme;

};

#endif // ELABEL_H