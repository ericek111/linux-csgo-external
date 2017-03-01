#include "ELabel.hpp"

void ELabel::onDraw(int ox, int oy) {
	draw->drawString(text, ox + posx, oy + posy, theme->text_main, draw->transparent, align);
	cout << " >>> Drwstr.\n";
}