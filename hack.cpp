#include "hack.hpp"

struct iovec g_remote[1024], g_local[1024];
struct hack::GlowObjectDefinition_t g_glow[1024];

int cachedSpottedAddress = -1;
int count = 0;
unsigned char spotted = 1;
unsigned int crossHairId = 0;
unsigned int lastcrossHairId = 0;
unsigned int attack = 0x5;
unsigned int release = 0x4;
unsigned int kills = 0;
unsigned int lkills = 0;

/*void mouseClick(int button, Display* display) {
    XEvent clickEvent;
    memset(&clickEvent, 0x00, sizeof(clickEvent));
    clickEvent.type = ButtonPress;
    clickEvent.xbutton.button = button;
    clickEvent.xbutton.same_screen = True;
    XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &clickEvent.xbutton.root, &clickEvent.xbutton.window, &clickEvent.xbutton.x_root, &clickEvent.xbutton.y_root, &clickEvent.xbutton.x, &clickEvent.xbutton.y, &clickEvent.xbutton.state);
    clickEvent.xbutton.subwindow = clickEvent.xbutton.window;
    while(clickEvent.xbutton.subwindow) {
        clickEvent.xbutton.window = clickEvent.xbutton.subwindow;
        XQueryPointer(display, clickEvent.xbutton.window, &clickEvent.xbutton.root, &clickEvent.xbutton.subwindow, &clickEvent.xbutton.x_root, &clickEvent.xbutton.y_root, &clickEvent.xbutton.x, &clickEvent.xbutton.y, &clickEvent.xbutton.state);
    }

    if(XSendclickEvent(display, PointerWindow, True, 0xfff, &clickEvent) == 0) fprintf(stderr, "Error\n");
    XFlush(display);
    sleep(10);

    clickEvent.type = ButtonRelease;
    clickEvent.xbutton.state = 0x100;
    if(XSendEvent(display, PointerWindow, True, 0xfff, &clickEvent) == 0) fprintf(stderr, "Error\n");
    XFlush(display);
}*/
void Radar(remote::Handle* csgo, remote::MapModuleMemoryRegion* client, void* ent, hack::Entity* entity) {
    csgo->Write((void*) ((unsigned long) ent + 0xECD), &spotted, sizeof(unsigned char));
}

void NoFlash(remote::Handle* csgo, remote::MapModuleMemoryRegion* client) {
    float fFlashAlpha = 100.0f;
    float fFlashAlphaFromGame = 0.0f;

    csgo->Read((void*) (csgo->localPlayerOffset+0xABE4), &fFlashAlphaFromGame, sizeof(float));
    //std::cout << "FlashA: " << fFlashAlphaFromGame << "\n";

    /*if(fFlashAlphaFromGame > 70.0f)
        csgo->Write((void*) (csgo->localPlayerOffset+0xABE4), &fFlashAlpha, sizeof(float));*/
}

void hack::Glow(remote::Handle* csgo, remote::MapModuleMemoryRegion* client, Draw* draw) {
    if (!csgo || !client)
        return;

    // Reset
    bzero(g_remote, sizeof(g_remote));
    bzero(g_local, sizeof(g_local));
    bzero(g_glow, sizeof(g_glow));

    hack::CGlowObjectManager manager;

    if (!csgo->Read((void*) csgo->addressOfGlowPointer, &manager, sizeof(hack::CGlowObjectManager))) {
        // std::cout << "Failed to read glowClassAddress" << std::endl;
        return;
    }

    size_t count = manager.m_GlowObjectDefinitions.Count;

    void* data_ptr = (void*) manager.m_GlowObjectDefinitions.DataPtr;

    if (!csgo->Read(data_ptr, g_glow, sizeof(hack::GlowObjectDefinition_t) * count)) {
        // std::cout << "Failed to read m_GlowObjectDefinitions" << std::endl;
        return;
    }

    size_t writeCount = 0;
    hack::Entity lp;
    csgo->Read(((void*) csgo->localPlayerOffset), &lp, sizeof(hack::Entity));

    //draw->drawString(std::bitset<32>(lp.m_iName).to_string().c_str(), draw->WIDTH/2, draw->HEIGHT-19, draw->white, draw->blacka, ALIGN_CENTER);
    //for (int i = 0; i < sizeof(lp.m_iName); ++i) {
        //draw->drawString(string(&lp.m_iName[i]).c_str(), i*9, draw->HEIGHT-19-40, draw->white, draw->blacka, ALIGN_CENTER);
    //}
    uint32_t activeweaponentityindex;
    csgo->Read(((void*) csgo->localPlayerOffset + 0x3628), &activeweaponentityindex, sizeof(uint32_t));
    activeweaponentityindex &= 0xFFF;


    unsigned long weaponptr2;

    uint32_t weaponindex;
    
    // m_AttributeManager + m_Item + m_iItemDefinitionIndex
    unsigned long weaponentity;
    //csgo->Read((void *)(csgo->addressOfEntityList + ((activeweaponentityindex - 1) * 0x20)), &weaponentity, sizeof(unsigned long));

    //csgo->Read((void *)(weaponentity + 0x34c0 + 0x60 + 0x268), &weaponindex, sizeof(uint32_t));
    

    //std::cout << std::hex << weaponptr << " > " << weaponptr2 << " / " << weaponindex << "\n";
    draw->drawString(to_string(activeweaponentityindex).c_str(), 500, 50, draw->white, draw->blacka, ALIGN_CENTER);
    //draw->drawString(to_string(weaponindex).c_str(), 500, 68, draw->white, draw->blacka, ALIGN_CENTER);

    if(csgo->localPlayerOffset != 0 && csgo->noFlash) {
        NoFlash(csgo, client); 
    }

    if(false) {
        Vector localPos = lp.m_vecOrigin;
        std::cout << "=============================== " << "\n";
        std::cout << std::fixed << std::setprecision(3) << "L: m_vecOrigin: " << lp.m_vecOrigin.x << " / " << lp.m_vecOrigin.y << " / " << lp.m_vecOrigin.z << "\n";
        std::cout << std::fixed << std::setprecision(3) << "L: m_vecViewOffset: " << lp.m_vecViewOffset.x << " / " << lp.m_vecViewOffset.y << " / " << lp.m_vecViewOffset.z << "\n";
        std::cout << std::fixed << std::setprecision(3) << "L: m_angRotation: " << lp.m_angRotation.x << " / " << lp.m_angRotation.y << " / " << lp.m_angRotation.z << "\n";
        std::cout << std::fixed << std::setprecision(3) << "L: m_angNetworkAngles: " << lp.m_angNetworkAngles.x << " / " << lp.m_angNetworkAngles.y << " / " << lp.m_angNetworkAngles.z << "\n";
        //std::cout << std::dec << " [" << lp.m_iEntityId << "] team: (" << localTeam << ") " << lp.m_iTeamNum << " / health: " << lp.m_iHealth << "\n";
    }
    int lasti = 0;
    std::stringstream ss;
    //draw->clearArea(draw->WIDTH/2-400, draw->HEIGHT-150, 800, 150);
    draw->clearArea(draw->WIDTH-300, 200, 300, 400);
    csgo->Read((void*) (csgo->localPlayerOffset+0xB380), &crossHairId, sizeof(int));
    if(crossHairId != 0) lastcrossHairId = crossHairId;

    /*Vector toread = lp.m_vecViewOffset;
    draw->drawString((std::string("m_vecViewOffset: ")+to_string(int(toread.x))+std::string(" / ")+to_string(int(toread.y))+std::string(" / ")+to_string(int(toread.z))).c_str(), draw->WIDTH/2, draw->HEIGHT-draw->font_height*7, draw->ltblue, draw->blacka, ALIGN_LEFT);
    toread = lp.m_vecVelocity;
    draw->drawString((std::string("m_vecVelocity: ")+to_string(int(toread.x))+std::string(" / ")+to_string(int(toread.y))+std::string(" / ")+to_string(int(toread.z))).c_str(), draw->WIDTH/2, draw->HEIGHT-draw->font_height*6, draw->ltblue, draw->blacka, ALIGN_LEFT);
    toread = lp.m_vecBaseVelocity;
    draw->drawString((std::string("m_vecBaseVelocity: ")+to_string(int(toread.x))+std::string(" / ")+to_string(int(toread.y))+std::string(" / ")+to_string(int(toread.z))).c_str(), draw->WIDTH/2, draw->HEIGHT-draw->font_height*5, draw->ltblue, draw->blacka, ALIGN_LEFT);
    toread = lp.m_angNetworkAngles;
    draw->drawString((std::string("m_angNetworkAngles: ")+to_string(int(toread.x))+std::string(" / ")+to_string(int(toread.y))+std::string(" / ")+to_string(int(toread.z))).c_str(), draw->WIDTH/2, draw->HEIGHT-draw->font_height*4, draw->ltblue, draw->blacka, ALIGN_LEFT);
    toread = lp.m_vecNetworkOrigin;
    draw->drawString((std::string("m_vecNetworkOrigin: ")+to_string(int(toread.x))+std::string(" / ")+to_string(int(toread.y))+std::string(" / ")+to_string(int(toread.z))).c_str(), draw->WIDTH/2, draw->HEIGHT-draw->font_height*3, draw->ltblue, draw->blacka, ALIGN_LEFT);

    if(csgo->drawVelocity && false) {
    toread = lp.m_vecAbsVelocity;
    float vel2d = hypot(toread.x, toread.z);
    draw->drawString((std::string("m_vecAbsVelocity: ")+to_string(int(toread.x))+std::string(" / ")+to_string(int(toread.y))+std::string(" / ")+to_string(int(toread.z))).c_str(), draw->WIDTH/2, draw->HEIGHT-draw->font_height*7, draw->ltblue, draw->blacka, ALIGN_LEFT);
    float vel3d = hypot(toread.y, hypot(toread.x, toread.z));
    draw->drawString((std::string("2D Velocity: ")+to_string(vel2d)).c_str(), draw->WIDTH/2, draw->HEIGHT-draw->font_height*6, draw->ltred, draw->blacka, ALIGN_LEFT);
    draw->drawString((std::string("3D Velocity: ")+to_string(vel3d)).c_str(), draw->WIDTH/2, draw->HEIGHT-draw->font_height*5, draw->ltred, draw->blacka, ALIGN_LEFT);
    vel2d = vel2d > 254 ? 254 : vel2d;
    if(vel2d != 254 && vel2d > 1) {
        draw->fillRectangle(draw->WIDTH/2-120, draw->HEIGHT/2+120, 240, 20, csgo->speedgradient[int(vel2d)]);
        draw->fillRectangle(draw->WIDTH/2-120, draw->HEIGHT/2-140, 240, 20, csgo->speedgradient[int(vel2d)]);
    } else {
        draw->clearArea(draw->WIDTH/2-120, draw->HEIGHT/2+120, 240, 20);
        draw->clearArea(draw->WIDTH/2-120, draw->HEIGHT/2-140, 240, 20);
    }
	}
	/*std::stringstream oss;
	oss << "Obs: " << std::hex << lp.m_hObserverTarget << " / " << lp.m_iObserverMode;//(reinterpret_cast<std::size_t>(g_glow[i].m_pEntity))
	draw->drawString(oss.str().c_str(), 0, 230, draw->ltred, draw->blacka, ALIGN_LEFT);*/


    /*toread = lp.m_vecAbsOrigin;
    draw->drawString((std::string("m_vecAbsOrigin: ")+to_string(int(toread.x))+std::string(" / ")+to_string(int(toread.y))+std::string(" / ")+to_string(int(toread.z))).c_str(), draw->WIDTH/2, draw->HEIGHT-draw->font_height*6, draw->ltblue, draw->blacka, ALIGN_LEFT);
   
    toread = lp.m_vecOrigin;
    draw->drawString((std::string("m_vecOrigin: ")+to_string(int(toread.x))+std::string(" / ")+to_string(int(toread.y))+std::string(" / ")+to_string(int(toread.z))).c_str(), draw->WIDTH/2, draw->HEIGHT-draw->font_height*5, draw->ltblue, draw->blacka, ALIGN_LEFT);
    toread = lp.m_vecAngVelocity;
    draw->drawString((std::string("m_vecAngVelocity: ")+to_string(int(toread.x))+std::string(" / ")+to_string(int(toread.y))+std::string(" / ")+to_string(int(toread.z))).c_str(), draw->WIDTH/2, draw->HEIGHT-draw->font_height*4, draw->ltblue, draw->blacka, ALIGN_LEFT);
    toread = lp.m_angAbsRotation;
    draw->drawString((std::string("m_angAbsRotation: ")+to_string(int(toread.x))+std::string(" / ")+to_string(int(toread.y))+std::string(" / ")+to_string(int(toread.z))).c_str(), draw->WIDTH/2, draw->HEIGHT-draw->font_height*3, draw->ltblue, draw->blacka, ALIGN_LEFT);
    toread = lp.m_angRotation;
    draw->drawString((std::string("m_angRotation: ")+to_string(int(toread.x))+std::string(" / ")+to_string(int(toread.y))+std::string(" / ")+to_string(int(toread.z))).c_str(), draw->WIDTH/2, draw->HEIGHT-draw->font_height*2, draw->ltblue, draw->blacka, ALIGN_LEFT);
*/
//Vector velocity; velocity.x = 
    Vector eyepos;
    //QAngle toread = lp.m_aimPunchAngle;
    //draw->drawString((std::string("Eyepos: ")+to_string(int(toread.x))+std::string(" / ")+to_string(int(toread.y))+std::string(" / ")+to_string(int(toread.z))).c_str(), draw->WIDTH/2+100, draw->HEIGHT-draw->font_height*2, draw->ltblue, draw->blacka, ALIGN_LEFT);
    /*toread.x = lp.m_aimPunchAngle.x * 1000.0f;
    toread.y = lp.m_aimPunchAngle.y * 1000.0f;
    toread.z = lp.m_aimPunchAngle.z * 1000.0f;*/
    //toread = lp.m_aimPunchAngle * 100.0f;
    QAngle toread;
    csgo->Read((void*) (csgo->localPlayerOffset+0x36f0+0x74), &toread, sizeof(QAngle));
//    draw->drawString((std::string("Velocity: ")+to_string((toread.x))+std::string(" / ")+to_string((toread.y))+std::string(" / ")+to_string((toread.z))).c_str(), draw->WIDTH/2+100, draw->HEIGHT-draw->font_height*1-15, draw->ltblue, draw->blacka, ALIGN_LEFT);
    
    int ScreenWidth = draw->WIDTH, ScreenHeight = draw->HEIGHT;
    int fov = 90;

    int x = (int) (ScreenWidth * 0.5f);
    int y = (int) (ScreenHeight * 0.5f);
    int dx = ScreenWidth / fov;
    int dy = ScreenHeight / fov;

    int crosshairX = (int) (x - (dx * toread.y));
    int crosshairY = (int) (y + (dy * toread.x));
    draw->clearArea(crosshairX-30, crosshairY-50, 50, 70);

    draw->fillRectangle(crosshairX-6, crosshairY-1, 12, 3, draw->blackma);
    draw->fillRectangle(crosshairX-1, crosshairY-6, 3, 12, draw->blackma);

    draw->drawLine(crosshairX - 5, crosshairY, crosshairX + 5, crosshairY, draw->white);
    draw->drawLine(crosshairX, crosshairY + 5, crosshairX, crosshairY - 5, draw->white);

    draw->fillRectangle(x-2, y-2, 4, 4, draw->blacka);
    draw->fillRectangle(x-1, y-1, 2, 2, draw->ltblue);

    int spectc = -1;

    /*if(lp.m_iEFlags & FL_ONGROUND) {
    	draw->clearArea(0, 220, 50, 100);
    } else {
    	if(abs(lp.m_vecAbsVelocity.y) < 30) draw->fillRectangle(0, 220, 50, 100, draw->green);
    	else draw->fillRectangle(0, 220, 50, 100, draw->red);
    }*/
    for (unsigned int i = 0; i < count; i++) {
        if (g_glow[i].m_pEntity != NULL) {
            hack::Entity ent;

            if (csgo->Read(g_glow[i].m_pEntity, &ent, sizeof(hack::Entity))) {
                if ((ent.m_iTeamNum != 2 && ent.m_iTeamNum != 3 ||
                    ent.m_bDormant == 1) && false) {
                    g_glow[i].m_bRenderWhenOccluded = 0;
                    g_glow[i].m_bRenderWhenUnoccluded = 0;
                    continue;
                }

    /*if(lastcrossHairId == ent.m_iEntityId) {
        ss << std::dec << lastcrossHairId << " @ " << g_glow[i].m_pEntity;
        draw->drawString( ss.str().c_str(), draw->WIDTH/2, draw->HEIGHT-draw->font_height*1, draw->yellow, draw->blacka, ALIGN_CENTER);
        ss.str("");
    } */
   

                if(ent.m_vecOrigin.x != 0 && ent.m_vecOrigin.z != 0 && ent.m_iHealth > 0 && false) {
                    Vector toread = ent.m_vecOrigin;
                    /*std::ios oldState(nullptr);
                    oldState.copyfmt(std::cout);
                    int vt; 
                    int fn; 
                    int cls; 
                    int clsn; 
                    int clsid;
                    csgo->Read((data_ptr + (sizeof(hack::GlowObjectDefinition_t) * i) + 0x8), &vt, sizeof(int));
                    csgo->Read((void*) (vt + 2 * 0x4), &fn, sizeof(int));
                    csgo->Read((void*) (fn + 1), &cls, sizeof(int));
                    csgo->Read((void*) (cls + 0x14), &clsid, sizeof(int));
                    csgo->Read((void*) (cls + 0x14), &clsn, sizeof(int));
                    //std::cout << std::showbase << std::internal << std::setfill('0') << std::hex << std::setw(16) << (reinterpret_cast<std::size_t>(g_glow[i].m_pEntity));
                    std::cout.copyfmt(oldState);*/

                    // CPlantedC4 + m_bBombTicking
                    //hack::CPlantedC4 bomb;
                    //csgo->Read(g_glow[i].m_pEntity, &bomb, sizeof(hack::CPlantedC4));
                    //std::cout << "Bomb ticking: " << bomb.m_bBombTicking << "\n";
                    //if(bomb.m_bBombTicking > 0) {
                    //    std::cout << "Timer: " << std::fixed << std::setprecision(3) << bomb.m_flTimerLength << " / m_flDefuseCountDown: " << bomb.m_flDefuseCountDown << " / defuser: " << bomb.m_hBombDefuser << " / m_flC4Blow: " << bomb.m_flC4Blow << " / m_bStartedArming: " << bomb.m_bStartedArming << "\n";
                    //}

                    std::cout << std::dec << " < " << ent.m_iEntityId << " >>> " /*<< "(" << clsid << " / " << clsn << ") > x: "*/ << toread.x << " y: " << toread.y << " z: " << toread.z << "\n";
                }

                //csgo->Read((void*) ((unsigned long)(g_glow[i].m_pEntity)+0x94), &entityId, sizeof(int));

                /*if(csgo->localPlayerOffset != 0 && csgo->shouldTrigger == true) {
                    if(ent.m_iTeamNum != lp.m_iTeamNum) {
                        csgo->Read((void*) (csgo->localPlayerOffset+0xB380), &crossHairId, sizeof(int));
                        if(crossHairId != 0) lastcrossHairId = crossHairId;
                        if(crossHairId == ent.m_iEntityId) {
                            usleep(100);
                            csgo->Write((void*) (csgo->addressOfForceAttack), &attack, sizeof(int));
                            usleep(100);
                            csgo->Write((void*) (csgo->addressOfForceAttack), &release, sizeof(int));
                        }
                    }
                }  */
                Vector vec1 = ent.m_vecOrigin - lp.m_vecOrigin;
                if(false && abs(vec1.x) < 64 && abs(vec1.z) < 64 && abs(vec1.y) < 64 && abs(vec1.x) > 0) {
                    spectc++;
                    unsigned long long vtable;
                    unsigned long long idfn;
                    unsigned long long idcls;
                    int clsid;
                    csgo->Read((void*) (g_glow[i].m_pEntity + 0x8), &vtable, sizeof(unsigned long long));
                    csgo->Read((void*) (vtable + (0x8 * 2)), &idfn, sizeof(unsigned long long));
                    csgo->Read((void*) (idfn + 0x1), &idcls, sizeof(unsigned long long));
                    csgo->Read((void*) (idcls + 0x14), &clsid, sizeof(int));
                    if(clsid != 0)
                    std::cout << vec1.x << "," << vec1.y << "," << vec1.z << " > Class ID: " << clsid << std::endl;
                }

                if(csgo->shouldGlow) Radar(csgo, client, g_glow[i].m_pEntity, &ent);

                g_glow[i].m_bRenderWhenOccluded = 1;
                g_glow[i].m_bRenderWhenUnoccluded = 0;
                
                if(ent.m_iHealth > 0 && (ent.m_iTeamNum == 2 || ent.m_iTeamNum == 3)) {
                    //ss << std::dec << (ent.m_bDormant ? "1" : "0") << " [" << ent.m_iEntityId << "] " << "health: " << ent.m_iHealth << " @ " << g_glow[i].m_pEntity;
                    unsigned long long m_hObserverTarget;
                    int m_iObserverMode;
                    csgo->Read((void*) (csgo->localPlayerOffset+0x3b74), &m_hObserverTarget, sizeof(unsigned long long));
                    csgo->Read((void*) (csgo->localPlayerOffset+0x3b60), &m_iObserverMode, sizeof(int));
                    //int kills[64];
                    //csgo->Read((void*) (csgo->addressOfPlayerResources+0x1fd8), &kills, sizeof(int[64]));
                    ss << std::dec << ent.m_iEntityId << " > " << ent.m_iHealth;// << " / S: " << kills[ent.m_iEntityId];// << " @ " << m_iObserverMode << " / " << std::hex << m_hObserverTarget;// (client->start - (unsigned long)g_glow[i].m_pEntity);// << " / " << ent.m_flFlashMaxAlpha << " / " << ent.m_flFlashDuration << " >" << std::hex << sizeof(hack::Entity);
                    draw->drawString(ss.str().c_str(), draw->WIDTH, 200+lasti*19, (ent.m_bDormant ? draw->grey : (lastcrossHairId == ent.m_iEntityId) ? draw->ltyellow : (ent.m_iTeamNum == 2 ? draw->red : draw->ltblue)), draw->blacka, ALIGN_RIGHT);
                    ss.str("");
                    lasti++;
                }
                if(g_glow[i].m_bRenderWhenOccluded == 1 && (!csgo->justglowoff || !csgo->justglowothersoff) && false) {
                  continue;
                }

                float health = ent.m_iHealth > 100 ? 100 : ent.m_iHealth;

                if (ent.m_iTeamNum == 2) {
                    g_glow[i].m_flGlowRed = 1.0f;
                    g_glow[i].m_flGlowGreen = health != 0 ? 1.0f-health/100.0f : 0.0f;
                    g_glow[i].m_flGlowBlue = 0.0f;
                    g_glow[i].m_flGlowAlpha = 0.55f;
                } else if (ent.m_iTeamNum == 3) {
                    g_glow[i].m_flGlowRed = 0.0f;
                    g_glow[i].m_flGlowGreen = health != 0 ? (1.0f-(health/100.0f)) : 0.0f;
                    g_glow[i].m_flGlowBlue = 1.0f;
                    g_glow[i].m_flGlowAlpha = 0.55f;
                } else {
                    g_glow[i].m_flGlowRed = 0.0f;
                    g_glow[i].m_flGlowGreen = 1.0f;
                    g_glow[i].m_flGlowBlue = 1.0f;
                    g_glow[i].m_flGlowAlpha = 0.4f;
                    if(!csgo->glowOthers) {
                        g_glow[i].m_flGlowRed = 0.0f;
                        g_glow[i].m_flGlowGreen = 0.0f;
                        g_glow[i].m_flGlowBlue = 0.0f;
                        g_glow[i].m_flGlowAlpha = 0.0f;
                        g_glow[i].m_bRenderWhenOccluded = 0;
                        g_glow[i].m_bRenderWhenUnoccluded = 0;
                    }
                }
                if(health < 3 && health > 0) {
                        g_glow[i].m_flGlowRed = 0.0f;
                        g_glow[i].m_flGlowGreen = 1.0f;
                        g_glow[i].m_flGlowBlue = 0.0f;
                        g_glow[i].m_flGlowAlpha = 0.7f;
                }
                if(!csgo->shouldGlow) {
                    if(health > 0) continue;
                    g_glow[i].m_bRenderWhenOccluded = 0;
                    g_glow[i].m_bRenderWhenUnoccluded = 0;
                    g_glow[i].m_flGlowAlpha = 0.0f;
                    g_glow[i].m_flGlowRed = 0.0f;
                    g_glow[i].m_flGlowGreen = 0.0f;
                    g_glow[i].m_flGlowBlue = 0.0f;
                }
                g_glow[i].m_bFullBloomRender = 0;
            }
        }
        if(spectc > 0) draw->drawString(to_string(spectc).c_str(), 0, 200, draw->ltblue, draw->blacka, ALIGN_LEFT);

        size_t bytesToCutOffEnd = sizeof(hack::GlowObjectDefinition_t) - g_glow[i].writeEnd();
        size_t bytesToCutOffBegin = (size_t) g_glow[i].writeStart();
        size_t totalWriteSize = (sizeof(hack::GlowObjectDefinition_t) - (bytesToCutOffBegin + bytesToCutOffEnd));

        g_remote[writeCount].iov_base =
                ((uint8_t*) data_ptr + (sizeof(hack::GlowObjectDefinition_t) * i)) + bytesToCutOffBegin;
        g_local[writeCount].iov_base = ((uint8_t*) &g_glow[i]) + bytesToCutOffBegin;
        g_remote[writeCount].iov_len = g_local[writeCount].iov_len = totalWriteSize;

        writeCount++;
    }
    process_vm_writev(csgo->GetPid(), g_local, writeCount, g_remote, writeCount, 0);
}
