#include "EWindowsManager.hpp"
void EWindowsManager::drawWindows() {
	XEvent ev;

	while (XPending(draw->g_display) > 0) {
    // http://www.df.unipi.it/~moruzzi/xlib-programming.html
    // https://tronche.com/gui/x/xlib/events/keyboard-pointer/keyboard-pointer.html#XButtonEvent
    XNextEvent(draw->g_display, &ev);
	std::map<int, shared_ptr<EWindow>> hitwins;
    for (auto &win : windowsarr) {
    	if( (ev.xkey.x >= win->fposx && ev.xkey.x <= win->fposx + win->fwidth) && (ev.xkey.y >= win->fposy && ev.xkey.y <= win->fposy + win->fheight) ) {
    		hitwins.insert( pair<int, shared_ptr<EWindow>>(win->zindex, make_shared<EWindow>(*win)) );
    	}
    }
    
    bool hittitle = ( ev.xkey.x > activewin->posx  && ev.xkey.x < activewin->posx + activewin->width ) && ( ev.xkey.y > activewin->posy && ev.xkey.y < activewin->posy + theme->titleheight );
    bool wastitle = ( mb1lastdownx > activewin->oposx  && mb1lastdownx < activewin->oposx + activewin->width ) && ( mb1lastdowny > activewin->oposy && mb1lastdowny < activewin->oposy + theme->titleheight );

    	
    	if(ev.type == ButtonPress) {
    		mb1lastdownx = ev.xkey.x;
    		mb1lastdowny = ev.xkey.y;
    		if(hitwins.size() > 0) {
    			EWindow* hitwin = &*(hitwins.rbegin()->second);
    			if(topzindex != hitwin->zindex) topzindex++;
    			hitwin->zindex = topzindex;
    			activewin = hitwin;
    			activewin->oposx = activewin->posx;
    			activewin->oposy = activewin->posy;
        		cout << "PRESSED! " << ev.xkey.x << "," << ev.xkey.y << " > " << ev.xkey.state << " / " << ev.xkey.keycode << " @ WIN: " << hitwin->title << " > topping to " << hitwin->zindex << "\n";
        		cout << "Hit top " << hitwin->title << "\n";
        	}
    	} else if(ev.type == ButtonRelease) {
        	cout << "released! " << ev.xkey.x << "," << ev.xkey.y << " > " << ev.xkey.state << " / " << ev.xkey.keycode << "\n";
        	if(hittitle) {
    			activewin->oposx = activewin->posx;
    			activewin->oposy = activewin->posy;
    			setInputAreas();
    		}
    	} else if(ev.type == MotionNotify) {	
    		if(ev.xkey.state & Button1MotionMask) {

    			//cout << "activewin @ " << activewin->fposx << "," << activewin->fposy << " / " << (hittitlex ? "1" : "0") << "," << (hittitley ? "1" : "0") << "\n";
    			// > if started dragging on window's title
    			if(wastitle) {
    				cout << ">> Moving... " << ev.xkey.x << "," << ev.xkey.y << " > " << (std::bitset<32>(ev.xkey.state).to_string()) << " FROM " << mb1lastdownx << "," << mb1lastdowny << " _ " << (ev.xkey.x - mb1lastdownx) << "," << (ev.xkey.y - mb1lastdowny) << "\n";
    				if(ev.xkey.x < draw->WIDTH) activewin->posx = activewin->oposx + (ev.xkey.x - mb1lastdownx);
    				if(ev.xkey.y < draw->HEIGHT) activewin->posy = activewin->oposy + (ev.xkey.y - mb1lastdowny);
    				updateFullPos(activewin);
    			}
    		}
    		/*if(hitwins.size() > 0) {
    			EWindow* hitwin = &*(hitwins.rbegin()->second);
	        	cout << ">> MOVED! " << ev.xkey.x << "," << ev.xkey.y << " > " << (std::bitset<32>(ev.xkey.state).to_string()) << "\n";
	    	}*/
    	}
}

	std::map<int, shared_ptr<EWindow>> renderwins;
	for (auto &win : windowsarr) {
		renderwins.insert( pair<int, shared_ptr<EWindow>>(win->zindex, shared_ptr<EWindow>(win)) );
	}

	for(auto iter = renderwins.begin(); iter != renderwins.end(); iter++) {
		EWindow* win = &(*(iter->second));
		draw->fillRectangle(win->fposx, win->fposy, win->fwidth, win->fheight, theme->window_border);
		draw->fillRectangle(win->posx, win->posy, win->width, win->height, theme->window_background);
		draw->fillRectangle(win->posx+1, win->posy+1, win->width-2, theme->titleheight, theme->window_titlebg);
		draw->drawString(win->title, win->posx + win->width/2, win->posy+1, theme->window_title, draw->transparent, ALIGN_CENTER);
		win->onDraw();
	}
}
void EWindowsManager::setInputAreas() {
	draw->clearCaptureAreas();
	for (auto &win : windowsarr) {
		XRectangle rect;
		rect.x = win->fposx;
		rect.y = win->fposy;
		rect.width = win->fwidth;
		rect.height = win->fheight;
		draw->addCaptureArea(rect);
    }
    draw->startCaptureInput();
}
int EWindowsManager::registerWindow(EWindow *ewin) {

	totalwinc++;
	if(ewin->posx == -1 || ewin->posy == -1) {
		ewin->posx = lastcreatedx;
		ewin->posy = lastcreatedx;
		if(lastcreatedy > createposxystep*5) {
			lastcreatedy = 0;
			lastcreatedx = 0;
		}
	}
	lastcreatedx = ewin->posx + createposxystep;
	lastcreatedy = ewin->posy + createposxystep;
	ewin->zindex = totalwinc;
	topzindex++;
	updateFullPos(ewin);
	windowsarr.push_back(make_shared<EWindow>(*ewin));
	activewin = ewin;
	return windowsarr.size();
}
int EWindowsManager::updateFullPos(EWindow *ewin) {
	ewin->fposx = (ewin->posx - theme->border_left);
	ewin->fposy = (ewin->posy - theme->border_top);
	ewin->fwidth = (ewin->width + theme->border_left + theme->border_right);
	ewin->fheight = (ewin->height + theme->border_top + theme->border_bottom);
}
void EWindowsManager::init() {
	theme = new ETheme();
	theme->init();

	EWindow* ewin = new EWindow(draw, theme);
	ewin->title = "Cyka Blyat!";

	EWindow* ewin2 = new EWindow(draw, theme);
	ewin2->title = "Window 2";
	ewin2->posx = 200;
	ewin2->posy = 140;

	EWindow* ewin3 = new EWindow(draw, theme);
	ewin3->title = "WIDEEE WINDOW";
	ewin3->width = 400;
	ewin3->height = 140;

	EView* eview3 = new EView(draw, theme);
	ELabel* elab3 = new ELabel(draw, theme);
	elab3->text = "Label 1!";
	elab3->posx = 10;
	elab3->posy = 20;
	cout << "adding elab3 to eview3:\n";
	eview3->addComponent(elab3);

	cout << "adding eview3 to ewin3:\n";
	ewin3->addComponent(eview3);
	
	registerWindow(ewin3);
	//registerWindow(ewin2);
	//registerWindow(ewin3);
}