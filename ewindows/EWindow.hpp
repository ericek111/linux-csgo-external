#ifndef EWINDOW_H
#define EWINDOW_H

#include <memory>
#include "../hack.hpp"
#include "../draw.hpp"
#include "EView.hpp"
#include "ETheme.hpp"

using namespace std;

class EWindow {
	typedef std::vector< shared_ptr< EComponent > >::iterator ECompIterator;

	public:
		EWindow(Draw* cdraw, ETheme* ctheme): draw( cdraw ), theme( ctheme ) {};
		void onMouseDown(int x, int y, int state, int button);
		void onMouseUp(int x, int y, int state, int button);
		void onMouseMove(int x, int y, int state);
		void onDraw();
		void init();
		void addComponent(EComponent* ec);
		int width = 300;
		int height = 300;
		int posx = -1;
		int posy = -1;
		int zindex = -1;

		int fwidth = -1;
		int fheight = -1;
		int fposx = -1;
		int fposy = -1;
		int oposx = -1;
		int oposy = -1;
		int initwidth;
		int initheight;
		std::vector< shared_ptr<EComponent> > components;
		const char * title = "[untitled]";
		ETheme* theme;

	private:
		Draw* draw;
};

#endif // EWINDOW_H