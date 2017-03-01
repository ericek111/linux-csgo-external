#include "EWindow.hpp"


void EWindow::onMouseDown(int x, int y, int state, int button) {

}
void EWindow::onMouseUp(int x, int y, int state, int button) {

}
void EWindow::onMouseMove(int x, int y, int state) {

}
void EWindow::onDraw() {
	//cout << "Drawing EWindow: " << components.size() << " > " << posx << " / " << (posy + theme->titleheight) << "\n";
	for ( ECompIterator it = components.begin(); it != components.end(); it++ ) {
		//cout << "Drawing EComponent in EWin: " << posx << " / " << (posy + theme->titleheight) << "\n";
		(*it)->onDraw(posx, posy + theme->titleheight);
	}
}
void EWindow::init() {
	for (auto &comp : components) {
		comp->init();
	}
}
void EWindow::addComponent(EComponent* ec) {
	components.push_back(shared_ptr<EComponent>(ec));
	cout << "Pushing EComponent to EWin > " << components.size() << " <\n";
}