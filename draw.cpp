#include "draw.h"

Draw::Draw() {
}
void Draw::allow_input_passthrough (Window w) {
	XserverRegion region = XFixesCreateRegion (g_display, NULL, 0);
	//XFixesSetWindowShapeRegion (g_display, w, ShapeBounding, 0, 0, 0);
	XFixesSetWindowShapeRegion (g_display, w, ShapeInput, 0, 0, region);
	XFixesDestroyRegion (g_display, region);
}
void Draw::list_fonts() {
	char **fontlist;
	int num_fonts;
	fontlist = XListFonts (g_display, "*", 1000, &num_fonts);
	for (int i = 0; i < num_fonts; ++i) {
		fprintf(stdout, "> %s\n", fontlist[i]);
	}
}
XColor Draw::createXColorFromRGB(short red, short green, short blue) {
	XColor color;

	// m_color.red = red * 65535 / 255;
	color.red = (red * 0xFFFF) / 0xFF;
	color.green = (green * 0xFFFF) / 0xFF;
	color.blue = (blue * 0xFFFF) / 0xFF;
	color.flags = DoRed | DoGreen | DoBlue;

	if (!XAllocColor(g_display, DefaultColormap(g_display, g_screen), &color)) {
		std::cerr << "createXColorFromRGB: Cannot create color" << endl;
		exit(-1);
	}
	return color;
}
XColor Draw::createXColorFromRGBA(short red, short green, short blue, short alpha) {
	XColor color;

	// m_color.red = red * 65535 / 255;
	color.red = (red * 0xFFFF) / 0xFF;
	color.green = (green * 0xFFFF) / 0xFF;
	color.blue = (blue * 0xFFFF) / 0xFF;
	color.flags = DoRed | DoGreen | DoBlue;

	if (!XAllocColor(g_display, DefaultColormap(g_display, g_screen), &color)) {
		std::cerr << "createXColorFromRGB: Cannot create color" << endl;
		exit(-1);
	}

	*(&color.pixel) = ((*(&color.pixel)) & 0x00ffffff) | (alpha << 24);
	return color;
}
void Draw::openDisplay() {
	g_display = XOpenDisplay(0);

	if (!g_display) {
		cerr << "Failed to open X display" << endl;
		return;
	}

	g_screen    = DefaultScreen(g_display);

	g_disp_width  = DisplayWidth(g_display, g_screen);
	g_disp_height = DisplayHeight(g_display, g_screen);

	// Has shape extions?
	if (!XShapeQueryExtension (g_display, &shape_event_base, &shape_error_base)) {
	   cerr << "NO shape extension in your system !" << endl;
	   return;
	}

	overlayavailable = true;
}
// Create a window
void Draw::createShapedWindow() {
	XSetWindowAttributes wattr;
	XColor bgcolor = createXColorFromRGBA(0, 0, 0, 0);

	Window root    = DefaultRootWindow(g_display);
	//Visual *visual = DefaultVisual(g_display, g_screen);

	XVisualInfo vinfo;
	XMatchVisualInfo(g_display, DefaultScreen(g_display), 32, TrueColor, &vinfo);
	g_colormap = XCreateColormap(g_display, DefaultRootWindow(g_display), vinfo.visual, AllocNone);

	XSetWindowAttributes attr;
	attr.background_pixmap = None;
	attr.background_pixel = bgcolor.pixel;
	attr.border_pixel=0;
	attr.win_gravity=NorthWestGravity;
	attr.bit_gravity=ForgetGravity;
	attr.save_under=1;
	attr.event_mask=BASIC_EVENT_MASK;
	attr.do_not_propagate_mask=NOT_PROPAGATE_MASK;
	attr.override_redirect=1; // OpenGL > 0
	attr.colormap = g_colormap;

	//unsigned long mask = CWBackPixel|CWBorderPixel|CWWinGravity|CWBitGravity|CWSaveUnder|CWEventMask|CWDontPropagate|CWOverrideRedirect;
	unsigned long mask = CWColormap | CWBorderPixel | CWBackPixel | CWEventMask | CWWinGravity|CWBitGravity | CWSaveUnder | CWDontPropagate | CWOverrideRedirect;

	g_win = XCreateWindow(g_display, root, 1920, 0, WIDTH, HEIGHT, 0, vinfo.depth, InputOutput, vinfo.visual, mask, &attr);

	//XShapeCombineMask(g_display, g_win, ShapeBounding, 900, 500, g_bitmap, ShapeSet);
	XShapeCombineMask(g_display, g_win, ShapeInput, 0, 0, None, ShapeSet );

	// We want shape-changed event too
	XShapeSelectInput (g_display, g_win, SHAPE_MASK );

	// Tell the Window Manager not to draw window borders (frame) or title.
	wattr.override_redirect = 1;
	XChangeWindowAttributes(g_display, g_win, CWOverrideRedirect, &wattr);
	allow_input_passthrough(g_win);

	// Show the window
	XMapWindow(g_display, g_win);
}
// Draw on the shaped window.
// Yes it's possible, but only pixels that hits the mask are visible.
// A hint: You can change the mask during runtime if you like.
void Draw::startdraw() {
	if (!overlayenabled)
		return;

	fpsmeterc++;
	if(fpsmeterc == FPSMETERSAMPLE) {
		fpsmeterc = 0;
		t1 = t2;
		t2 = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
		fpsstring = /*to_string(duration) + " / " +*/ to_string(duration/FPSMETERSAMPLE);
	}

	// Line width and type
	gcv.line_width = 1;
	gcv.line_style = LineSolid;
	gcv.foreground = red.pixel;

	unsigned long mask = GCLineWidth | GCLineStyle | GCForeground;
	gc = XCreateGC(g_display, g_win, mask, &gcv);

	XSetBackground (g_display, gc, white.pixel); 
	XSetForeground (g_display, gc, red.pixel);
	font = XLoadQueryFont (g_display, font_name);
	if (!font) {
		cout << "font " << font_name << " not available!" << endl;
	}
	XSetFont (g_display, gc, font->fid);


	if(doesCaptureInput) fillRectangle(100, 100, 400, 400, blackla);
	
}
bool isSameColor(XColor col1, XColor col2) {
	return (*(&col1.pixel) == *(&col2.pixel));
}
void Draw::drawString(const char * text, int x, int y, XColor fgcolor, XColor bgcolor, int align) {
	if (!this->overlayenabled)
		return;

	int tlen = strlen(text);
	if(!isSameColor(transparent, bgcolor)) {
		XSetForeground (g_display, gc, bgcolor.pixel);
		XFillRectangle(g_display, g_win, gc, x - (align == ALIGN_CENTER ? tlen*font_width/2 : (align == ALIGN_RIGHT ? tlen*font_width : 0)) - 4, y, tlen*font_width+8, font_height+4);
	}
	XSetForeground (g_display, gc, fgcolor.pixel);
	XDrawString(g_display, g_win, gc, x - (align == ALIGN_CENTER ? tlen*font_width/2 : (align == ALIGN_RIGHT ? tlen*font_width : 0)), y+font_height, text, tlen);
}
void Draw::enddraw() {
	if (!this->overlayenabled)
		return;

	if(duration > 0.0f) drawString(fpsstring.c_str(), WIDTH/2, 44, red, blacka, ALIGN_CENTER);
	renderi++;
	if(renderi == CLEARSCREEN_ITERATIONS) {
		clearscreen();
		renderi = 0;
	}
	XFreeFont(g_display, font);
	XFreeGC(g_display, gc);
}
void Draw::clearscreen() {
	if (!this->overlayenabled)
		return;
	XClearArea(g_display, g_win, 0, 0, WIDTH, HEIGHT, false);
}
void Draw::clearArea(int x, int y, int width, int height) {
	if (!this->overlayenabled)
		return;
	XClearArea(g_display, g_win, x, y, width, height, false);
}
void Draw::drawLine(int x1, int y1, int x2, int y2, XColor color) {
	if (!this->overlayenabled)
		return;
	XSetForeground (g_display, gc, color.pixel);
	XDrawLine(g_display, g_win, gc, x1, y1, x2, y2);
}
void Draw::fillRectangle(int x, int y, int width, int height, XColor color) {
	if (!this->overlayenabled)
		return;
	XSetForeground (g_display, gc, color.pixel);
	XFillRectangle(g_display, g_win, gc, x, y, width, height);
}
void Draw::startCaptureInput() {
	XRectangle rect;
	rect.x = 0;
	rect.y = 0;
	rect.width = 0;
	rect.height = 10;
	XserverRegion dst = XFixesCreateRegion(g_display, &rect, 1);
	for (auto &rect : captureAreas) {
		XserverRegion capreg = XFixesCreateRegion(g_display, &rect, 1);
		XFixesUnionRegion(g_display, dst, dst, capreg);
	}
	XFixesSetWindowShapeRegion (g_display, g_win, ShapeInput, 0, 0, dst);

	XSelectInput(g_display, g_win, PointerMotionMask | ButtonPressMask | ButtonReleaseMask); // KeyPressMask
	XAllowEvents(g_display, AsyncBoth, CurrentTime);

	XFixesDestroyRegion (g_display, dst);

	/*Pixmap pm = XCreatePixmap(g_display, g_win, 200, 200, 1);
	XShapeCombineMask(g_display, g_win, ShapeInput, 100, 100, pm, ShapeSet);
	XShapeSelectInput (g_display, g_win, SHAPE_MASK );*/
   // XShapeCombineMask(g_display, g_win, ShapeBounding, 100, 100, pm, ShapeSet);

	/*
	XserverRegion region = XFixesCreateRegion(g_display, &r, 1);
	XFixesSetWindowShapeRegion (g_display, g_win, ShapeInput, 0, 0, region);*/

	/*XShapeCombineRectangles(g_display, g_win, ShapeBounding, 0, 0, &r, 1, ShapeSet, Unsorted);

	//XserverRegion region = XFixesCreateRegion (g_display, NULL, 0);
	XRectangle rectangle; 
	Region region = XCreateRegion();
	rectangle.x = (short) 50;
	rectangle.y = (short) 50;
	rectangle.width = (unsigned short) 300;
	rectangle.height = (unsigned short) 300;
	XUnionRectWithRegion(&rectangle, region, region);
	XFixesSetWindowShapeRegion (g_display, g_win, ShapeInput, 0, 0, region);
	XDestroyRegion(region);*/
}
void Draw::stopCaptureInput() {
	allow_input_passthrough(g_win);
}
void Draw::addCaptureArea(XRectangle rect) {
	captureAreas.push_back(rect);  
}
void Draw::clearCaptureAreas() {
	captureAreas.clear();
}
void Draw::setCaptureInput(bool state) {
	if(state) startCaptureInput();
	else stopCaptureInput();
	doesCaptureInput = state;
}
void Draw::init() {
	openDisplay();
	if(!this->overlayavailable) {
		fprintf (stderr, "Overlay is not availalbe!\n");
		return;
	}

	createShapedWindow();
	gc = XCreateGC (g_display, g_win, 0, 0);
	font = XLoadQueryFont (g_display, font_name);

	red = createXColorFromRGBA(255, 0, 0, 255);
	ltblue = createXColorFromRGBA(0, 255, 255, 255);
	blue = createXColorFromRGBA(0, 0, 255, 255);
	black = createXColorFromRGBA(0, 0, 0, 255);
	blacka = createXColorFromRGBA(0, 0, 0, 150);
	blackma = createXColorFromRGBA(0, 0, 0, 180);
	blackla = createXColorFromRGBA(0, 0, 0, 80);
	grey = createXColorFromRGBA(110, 110, 110, 255);
	ltgrey = createXColorFromRGBA(170, 170, 170, 255);
	white = createXColorFromRGBA(255, 255, 255, 255);
	ltred = createXColorFromRGBA(255, 110, 130, 255);
	yellow = createXColorFromRGBA(255, 255, 0, 255);
	ltyellow = createXColorFromRGBA(255, 255, 80, 255);
	green = createXColorFromRGBA(0, 255, 0, 255);
	transparent = createXColorFromRGBA(255, 255, 255, 0);

	// const char * fontname = "-misc-fixed-bold-r-normal--18-120-100-100-c-90-iso8859-2";
	// const char * fontname = "rk24"; // ~ chinese shit
	// 
	font = XLoadQueryFont(g_display, font_name);
	/* If the font could not be loaded, revert to the "fixed" font. */
	if (!font) {
		fprintf (stderr, "Unable to load font %s > using default (fixed)\n", font_name);
		font = XLoadQueryFont(g_display, "fixed");
		list_fonts();
	}

}
void Draw::toggleoverlay(bool state) { this->overlayenabled = state; }
void Draw::toggleoverlay() { this->overlayenabled = !this->overlayenabled; }
void Draw::halt() { if(!this->overlayavailable) XCloseDisplay(g_display); }