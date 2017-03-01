#include "EFontsManager.hpp"

void EFontsManager::registerFont(const char * font_name, int font_width, int font_height) {
    EFont *ef = new EFont(g_display, font_name, font_width, font_height);
    //efonts[font_name] = ef;
    efonts.push_back(ef);
    fprintf(stdout, "registering %s - %i x %u!\n", font_name, font_width, font_height);

}
EFont* EFontsManager::getFont(int i) {
    return efonts.at(i);
}
EFont* EFontsManager::getFont() {
    return efonts.at(0);
}
void EFontsManager::init() {
    registerFont("9x15bold", 9, 15);
    reloadFonts();
}
void EFontsManager::reloadFonts() {
    for (auto& efont : efonts) {
        efont->init();
    }
}
void EFontsManager::startdraw() {
    for (auto& efont : efonts) {
        fprintf(stdout, "S > fdraw!\n");
        efont->startdraw(g_display);
        fprintf(stdout, "E > fdraw!\n");
    }
}
void EFontsManager::enddraw() {
    for (auto& efont : efonts) {
        efont->enddraw();
    }
}
void EFontsManager::list_fonts() {
    char **fontlist;
    int num_fonts;
    fontlist = XListFonts (g_display, "*", 1000, &num_fonts);
    for (int i = 0; i < num_fonts; ++i) { 
        fprintf(stderr, "> %s\n", fontlist[i]);
    }
}