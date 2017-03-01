#ifndef EVIEW_H
#define EVIEW_H

#include <memory>
#include <vector>
#include "EComponent.hpp"

using namespace std;

class EView : public EComponent {
	typedef std::vector< shared_ptr< EComponent > >::iterator ECompIterator;

	public:
		EView(Draw* cdraw, ETheme* ctheme): EComponent( cdraw, ctheme ) {};
		void onMouseEvent(XEvent xe);
		void onDraw(int ox, int oy);
		void addComponent(EComponent* ec);
		void init();
		int width = 300;
		int height = 300;
		int posx = -1;
		int posy = -1;
		std::vector< shared_ptr<EComponent> > components;
		ETheme* theme;
};

#endif // EVIEW_H