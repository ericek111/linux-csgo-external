#include "EFont.hpp"
EFont::EFont(Display *cg_display, const char * cfont_name, int cfont_width, int cfont_height) {
    font_name = cfont_name;
    font_width = cfont_width;
    font_height = cfont_height;
    font_name = "9x15bold";
    font_width = 9;
    font_height = 15;
    g_display = cg_display;
    fprintf(stdout, "> Creating %s - %i x %u!\n", font_name, font_width, font_height);
}

void EFont::init() {
    // const char * fontname = "-misc-fixed-bold-r-normal--18-120-100-100-c-90-iso8859-2";
    // const char * fontname = "rk24"; // ~ chinese shit
    // list_fonts(); 
    XFontStruct *fontb = XLoadQueryFont (g_display, "9x15bold");
    fprintf (stdout, "qed! \n");

    font = fontb;
    /* If the font could not be loaded, revert to the "fixed" font. */
    if (!font) {
        fprintf (stdout, "Unable to load font %s > using default (fixed)\n", font_name);
        font = XLoadQueryFont (g_display, "fixed");
    }
    fprintf (stdout, "INIT! %s\n", font_name);
    fid = font->fid;
}
void EFont::startdraw(Display *cg_display) {
        //fprintf(stdout, "drawing %s - %i x %u!\n", font_name, font_width, font_height);
    // SIGSEGVs with font_name
    fprintf(stdout, "Sc > efdraw!\n");
    XFontStruct *fontb = XLoadQueryFont (g_display, "9x15bold");
    fprintf(stdout, "Sc > efdraw!\n");
    fprintf(stdout, "Sc > efdraw!\n");
    fid = fontb->fid;
}
void EFont::enddraw() {
	XFreeFont(g_display, font);
    fprintf(stdout, "E > efdraw!\n");
}