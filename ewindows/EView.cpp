#include "EView.hpp"

void EView::onMouseEvent(XEvent xe) {

}
void EView::addComponent(EComponent* ec) {
	cout << "Pushing EComponent to EView <\n";
	components.push_back(shared_ptr<EComponent>(ec));
}
void EView::init() {
	for (auto &comp : components) {
		comp->init();
	}
}
void EView::onDraw(int ox, int oy) {
	cout << "Drawing EView: " << ox << " / " << oy << "\n";
	for ( ECompIterator it = components.begin(); it != components.end(); it++ ) {
		(*it)->onDraw(ox, oy);
	}
}