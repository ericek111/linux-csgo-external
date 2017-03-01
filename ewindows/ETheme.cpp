#include "ETheme.hpp"
ETheme::ETheme() {}
void ETheme::init() {
    g_display = XOpenDisplay(0);

    if (!g_display) {
    	fprintf(stderr, "Failed to open X display\n");
        return;
    }

    g_screen    = DefaultScreen(g_display);

	window_background = createXColorFromRGBA(255, 255, 255, 100);
    window_border = createXColorFromRGBA(0, 0, 0, 255);
    window_titlebg = createXColorFromRGBA(0, 255, 255, 150);
    window_title = createXColorFromRGBA(0, 0, 0, 255);
    text_main = createXColorFromRGBA(255, 0, 0, 255);
    text_inactive = createXColorFromRGBA(180, 50, 50, 255);
    text_label = createXColorFromRGBA(0, 0, 0, 255);
    but_border = createXColorFromRGBA(255, 0, 255, 255);
    but_text = createXColorFromRGBA(0, 0, 0, 255);
    but_bg = createXColorFromRGBA(190, 190, 190, 255);
    but_hover_text = createXColorFromRGBA(255, 0, 0, 255);
    but_hover_bg = createXColorFromRGBA(220, 220, 255, 255);
    but_hover_border = createXColorFromRGBA(255, 0, 255, 255);
    but_pressed_text = createXColorFromRGBA(210, 180, 0, 255);
    but_pressed_bg = createXColorFromRGBA(255, 220, 0, 255);
    but_pressed_border = createXColorFromRGBA(255, 0, 0, 255);
}

XColor ETheme::createXColorFromRGBA(short red, short green, short blue, short alpha) {
    XColor color;

    // m_color.red = red * 65535 / 255;
    color.red = (red * 0xFFFF) / 0xFF;
    color.green = (green * 0xFFFF) / 0xFF;
    color.blue = (blue * 0xFFFF) / 0xFF;
    color.flags = DoRed | DoGreen | DoBlue;

    if (!XAllocColor(g_display, DefaultColormap(g_display, g_screen), &color)) {
        fprintf(stderr, "createXColorFromRGB: Cannot create color\n");
        exit(-1);
    }

    *(&color.pixel) = ((*(&color.pixel)) & 0x00ffffff) | (alpha << 24);
    return color;
}