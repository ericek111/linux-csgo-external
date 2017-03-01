#ifndef EWINDOWSMANAGER_H
#define EWINDOWSMANAGER_H

#include "../hack.hpp"
#include "../draw.hpp"
#include <vector>
#include <map>
#include "ETheme.hpp"
#include "EView.hpp"
#include "ELabel.hpp"
#include "EWindow.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

class EWindowsManager {
	typedef vector<shared_ptr<EWindow>>::iterator EWinIterator;

	public:
		EWindowsManager(Draw* cdraw): draw( cdraw ) {};
		void drawWindows();
		void init();
		int registerWindow(EWindow* ewin);
		int updateFullPos(EWindow* ewin);
		void setInputAreas();
		vector< shared_ptr<EWindow> > windowsarr;

		int totalwinc = 0;
		int topzindex = 0;
		int lastcreatedx = 40;
		int lastcreatedy = 40;
		int createposxystep = 40;
		ETheme* theme;

		EWindow* activewin;
		int mb1lastdownx = 0;
		int mb1lastdowny = 0;

	private:
		Draw* draw;
};

#endif // EWINDOWSMANAGER_H