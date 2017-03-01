#include "DrawManager.hpp"

DrawManager::DrawManager() {

}
void DrawManager::allow_input_passthrough (Window w) {
    XserverRegion region = XFixesCreateRegion (g_display, NULL, 0);
    //XFixesSetWindowShapeRegion (g_display, w, ShapeBounding, 0, 0, 0);
    XFixesSetWindowShapeRegion (g_display, w, ShapeInput, 0, 0, region);
    XFixesDestroyRegion (g_display, region);
}
// Create a XColor from 3 byte tuple (0 - 255, 0 - 255, 0 - 255).
XColor DrawManager::createXColorFromRGB(short red, short green, short blue) {
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
// Create a XColor from 3 byte tuple (0 - 255, 0 - 255, 0 - 255).
XColor DrawManager::createXColorFromRGBA(short red, short green, short blue, short alpha) {
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

bool DrawManager::openDisplay() {
    g_display = XOpenDisplay(0);

    if (!g_display) {
        cerr << "Failed to open X display" << endl;
        return false;
    }

    g_screen    = DefaultScreen(g_display);

    g_disp_width  = DisplayWidth(g_display, g_screen);
    g_disp_height = DisplayHeight(g_display, g_screen);


    // Has shape extions?
    if (!XShapeQueryExtension (g_display, &shape_event_base, &shape_error_base)) {
       cerr << "NO shape extension in your system !" << endl;
       return false;
    }

    return true;

}

// Create a window
void DrawManager::createShapedWindow() {
    XSetWindowAttributes wattr;
    XColor bgcolor = createXColorFromRGBA(0, 0, 0, 0);

    Window root    = DefaultRootWindow(g_display);
    Visual *visual = DefaultVisual(g_display, g_screen);

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

    g_win = XCreateWindow(g_display, root, DEFX, DEFY, WIDTH, HEIGHT, 0, vinfo.depth, InputOutput, vinfo.visual, mask, &attr);

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
void DrawManager::startdraw() {
    if(!overlayenabled) return;

    fpsmeterc++;
    if(fpsmeterc == FPSMETERSAMPLE) {
        fpsmeterc = 0;
        t1 = t2;
        t2 = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        fpsstring = /*to_string(duration) + " / " +*/ to_string(duration/FPSMETERSAMPLE);
    }

    /*// Line width and type
    gcv.line_width = 1;
    gcv.line_style = LineSolid;
    gcv.foreground = red.pixel;

    unsigned long mask = GCLineWidth | GCLineStyle | GCForeground;
    gc = XCreateGC(g_display, g_win, mask, &gcv);*/
    /*XSetBackground (g_display, gc, white.pixel); 
    XSetForeground (g_display, gc, red.pixel);*/

    cout << "drawelele!\n";
    draw->startdraw();
    //XSetBackground (g_display, draw->gc, white.pixel); 
    XSetForeground (g_display, *draw->gc, red.pixel);
    cout << "fontelele!\n";
    //XFontStruct *fontb = XLoadQueryFont (g_display, "9x15bold");
    XSetFont (g_display, *draw->gc, font->fid);
    cout << "passelele!\n";

    if(doesCaptureInput) draw->fillRectangle(100, 100, 400, 400, blackla);
    
}
void DrawManager::enddraw() {
    if(!overlayenabled) return;

    if(duration > 0.0f) draw->drawString(fpsstring.c_str(), WIDTH/2, 44, red, blacka, ALIGN_CENTER);
    renderi++;
    if(renderi == CLEARSCREEN_ITERATIONS) {
        clearscreen();
        renderi = 0;
    }
    cout << "END DRAW!\n";
    draw->enddraw();

    //XFreeGC(g_display, gc);
    XEvent ev;

while (XPending(g_display) > 0) {
    // http://www.df.unipi.it/~moruzzi/xlib-programming.html
    // https://tronche.com/gui/x/xlib/events/keyboard-pointer/keyboard-pointer.html#XButtonEvent
    XNextEvent(g_display, &ev);
    if(ev.type == ButtonPress) {
        cout << "PRESSED! " << ev.xkey.x << "," << ev.xkey.y << " > " << ev.xkey.state << " / " << ev.xkey.keycode << "\n";
    } else if(ev.type == ButtonRelease) {
        cout << "released! " << ev.xkey.x << "," << ev.xkey.y << " > " << ev.xkey.state << " / " << ev.xkey.keycode << "\n";
    } else if(ev.type == MotionNotify) {
        cout << ">> MOVED! " << ev.xkey.x << "," << ev.xkey.y << " > " << ev.xkey.state << " / " << ev.xkey.keycode << "\n";
    }
}

}
void DrawManager::startCaptureInput() {
    XRectangle rect;
    rect.x = 100;
    rect.y = 100;
    rect.width = 400;
    rect.height = 400;
    XserverRegion region = XFixesCreateRegion(g_display, &rect, 1);
    XFixesSetWindowShapeRegion (g_display, g_win, ShapeInput, 0, 0, region);
    XFixesDestroyRegion (g_display, region);
    XSelectInput(g_display, g_win, PointerMotionMask | ButtonPressMask | ButtonReleaseMask); // KeyPressMask
    XAllowEvents(g_display, AsyncBoth, CurrentTime);

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
void DrawManager::stopCaptureInput() {
    allow_input_passthrough(g_win);
}
void DrawManager::setCaptureInput(bool state) {
    if(state) startCaptureInput();
    else stopCaptureInput();
    doesCaptureInput = state;
}
void DrawManager::init() {
    cout << "Opening dpy...\n";
    if(!openDisplay()) {
        fprintf (stderr, "Overlay is not availalbe!\n");
        return;
    }
    cout << "Opened dpy!\n";

    createShapedWindow();

    red = createXColorFromRGBA(255, 0, 0, 255);
    ltblue = createXColorFromRGBA(0, 255, 255, 255);
    blue = createXColorFromRGBA(0, 0, 255, 255);
    black = createXColorFromRGBA(0, 0, 0, 255);
    blacka = createXColorFromRGBA(0, 0, 0, 150);
    blackla = createXColorFromRGBA(0, 0, 0, 80);
    grey = createXColorFromRGBA(110, 110, 110, 255);
    ltgrey = createXColorFromRGBA(170, 170, 170, 255);
    white = createXColorFromRGBA(255, 255, 255, 255);
    ltred = createXColorFromRGBA(255, 110, 130, 255);
    yellow = createXColorFromRGBA(255, 255, 0, 255);
    ltyellow = createXColorFromRGBA(255, 255, 80, 255);
    transparent = createXColorFromRGBA(255, 255, 255, 0);

    /*EFontsManager efmp(g_display);
    efm = &efmp;

    efm->init();
    EFont* efp = efm->getFont();*/

    font = XLoadQueryFont (g_display, "9x15bold");
    font_width = 9;
    font_height = 15;

    Draw drawp(g_display, g_win, font, font_width, font_height);
    draw = &drawp;

    draw->overlayavailable = true;


}
void DrawManager::drawString(const char * text, int x, int y, XColor fgcolor, XColor bgcolor, int align) { draw->drawString(text, x, y, fgcolor, bgcolor, align); }
void DrawManager::clearArea(int x, int y, int width, int height) { draw->clearArea(x, y, width, height); }
void DrawManager::fillRectangle(int x, int y, int width, int height, XColor color) { draw->fillRectangle(x, y, width, height, color); }
void DrawManager::clearscreen() { draw->clearscreen(); }
void DrawManager::toggleoverlay(bool state) { if(draw->overlayavailable) draw->overlayenabled = state; }
void DrawManager::toggleoverlay() { if(draw->overlayavailable) draw->overlayenabled = !draw->overlayenabled; }
void DrawManager::halt() { if(!draw->overlayavailable) XCloseDisplay(g_display); }