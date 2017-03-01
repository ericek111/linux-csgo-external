#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#include <algorithm>

#include "log.hpp"
#include "draw.hpp"
#include "hack.hpp"
#include "remote.hpp"


#include "ewindows/EWindowsManager.hpp"

#define MSGFUNC_SERVERRANKREVEALALL_SIGNATURE "\x55\x48\x89\xE5\x53\x48\x89\xFB\x48\x83\xEC\x08\xE8\x00\x00\x00\x00\x48\x8D\x00\x00\x00\x00\x00\x48\x8B\x10\x48\x89\xC7\xFF\x52\x28"
#define MSGFUNC_SERVERRANKREVEALALL_MASK "xxxxxxxxxxxxx????xx?????xxxxxxxxx"
#define GLOWOBJECT_SIGNATURE "\xE8\x00\x00\x00\x00\x48\x8B\x3D\x00\x00\x00\x00\xBE\x01\x00\x00\x00\xC7"
#define GLOWOBJECT_MASK "x????xxx????xxxxxx"
#define PLAYERRESOURCES_SIGNATURE "\x48\x8B\x05\x00\x00\x00\x00\x55\x48\x89\xE5\x48\x85\xC0\x74\x10\x48"
#define PLAYERRESOURCES_MASK "xxx????xxxxxxxxxx"
#define OVERRIDEPOSTPROCESSINGDISABLE_SIGNATURE "\x80\x3D\x00\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00\x85\xC9"
#define OVERRIDEPOSTPROCESSINGDISABLE_MASK "xx????xxx????xx"
#define ENTITYLIST_SIGNATURE "\x48\x8B\x15\x00\x00\x00\x00\x0F\xB7\xC0\x48\xC1\xE0\x04"
#define ENTITYLIST_MASK "xxx????xxxxxxx"
// nDeltaTick: https://www.unknowncheats.me/forum/1654637-post783.html
#define DELTATICK_SIGNATURE "\xc7\x83\x00\x00\x00\x00\xff\xff\xff\xff\x48\x8b\x5d\x00\x48\x8d\x3d"
#define DELTATICK_MASK "xx????xxxxxxx?xxx"

typedef bool (*MsgFunc_ServerRankRevealAllFn) (float*);
MsgFunc_ServerRankRevealAllFn MsgFunc_ServerRankRevealAll;

using namespace std;

bool shouldGlow = true;
bool shouldTrigger = true;
bool shouldCaptureInput = false;
bool shouldDisablePostProcessing = false;
bool shouldNoHands = false;

Draw draw;
remote::Handle csgo;
remote::MapModuleMemoryRegion client;
EWindowsManager* winmgr;
pthread_t bhopthread;
pthread_t crosshairthread;

string csgoranks[] = { "", "S1", "S2", "S3", "S4", "SE", "SEM", "GN1", "GN2", "GN3", "GN4", "MG1", "MG2", "MGE", "DMG", "LE", "LEM", "SMFC", "GE" };
hack::CSPlayerResource playerResourceObj;

void *bhoploop(void *arg) {
    Display *g_display = XOpenDisplay(0);
    if (!g_display) {
        cerr << "Failed to open X display" << endl;
        return NULL;
    }
    int spaceKey = XKeysymToKeycode(draw.g_display, 0x020);
    unsigned int alt1 = 0;
    unsigned long hopc = 0;
    unsigned int jump = 5;
    while( csgo.running ) {
        csgo.Read((void*) csgo.addressOfAlt1, &alt1, sizeof(unsigned int));
        if(alt1 == 5) {
            unsigned long m_fFlags;
            csgo.Read((void*) (csgo.localPlayerOffset+0x138), &m_fFlags, sizeof(unsigned long));        
            if(m_fFlags & FL_ONGROUND) {
                //hopc++;
                XFlush(g_display);
                /*jump = 5;
                csgo.Write((void*) (csgo.m_oAddressOfForceJump), &jump, sizeof(int));
                usleep(1000);
                jump = 4;
                csgo.Write((void*) (csgo.m_oAddressOfForceJump), &jump, sizeof(int));*/
                XTestFakeKeyEvent(g_display, spaceKey, true, 0);
                usleep(500);
                XTestFakeKeyEvent(g_display, spaceKey, false, 0);
                usleep(10000);
                //cout << "Hopa! " << std::bitset<32>(m_fFlags) << " > " << hopc << "\n";
            }
        }

        usleep(1000);

/*        XEvent xevent;
        XNextEvent(draw.g_display, &xevent);
        switch (xevent.type) {
            case MotionNotify:
                printf("Mouse move      : [%d, %d]\n", xevent.xmotion.x_root, xevent.xmotion.y_root);
            case ButtonPress:
                printf("Button pressed  : %s\n", key_name[xevent.xbutton.button - 1]);
            case ButtonRelease:
                printf("Button released : %s\n", key_name[xevent.xbutton.button - 1]);
        }*/
    }
    return NULL;
}
void *crosshairloop(void *arg) {
    hack::QAngle toread;
    
//    draw->drawString((std::string("Velocity: ")+to_string((toread.x))+std::string(" / ")+to_string((toread.y))+std::string(" / ")+to_string((toread.z))).c_str(), draw->WIDTH/2+100, draw->HEIGHT-draw->font_height*1-15, draw->ltblue, draw->blacka, ALIGN_LEFT);
    
    int ScreenWidth = draw.WIDTH, ScreenHeight = draw.HEIGHT;
    int fov = 90;

    int x = (int) (ScreenWidth * 0.5f);
    int y = (int) (ScreenHeight * 0.5f);
    int dx = ScreenWidth / fov;
    int dy = ScreenHeight / fov;

    while( csgo.running ) {
        csgo.Read((void*) (csgo.localPlayerOffset+0x36f0+0x74), &toread, sizeof(hack::QAngle));

        int crosshairX = (int) (x - (dx * toread.y));
        int crosshairY = (int) (y + (dy * toread.x));
        draw.clearArea(crosshairX-30, crosshairY-50, 50, 70);

        draw.fillRectangle(crosshairX-6, crosshairY-1, 12, 3, draw.blackma);
        draw.fillRectangle(crosshairX-1, crosshairY-6, 3, 12, draw.blackma);

        draw.drawLine(crosshairX - 5, crosshairY, crosshairX + 5, crosshairY, draw.white);
        draw.drawLine(crosshairX, crosshairY + 5, crosshairX, crosshairY - 5, draw.white);

        draw.fillRectangle(x-2, y-2, 4, 4, draw.blacka);
        draw.fillRectangle(x-1, y-1, 2, 2, draw.ltblue);
        usleep(1000);
    }
    return NULL;
}
bool entityScoreboardSort (int x, int y) { return ( playerResourceObj.m_iScore[x] > playerResourceObj.m_iScore[y] ); }

void dumpEntityScoreboard(hack::CSPlayerResource* playerResource, int x, bool iscompetitive = false) {
	bool sument = x < 1;
	if(sument) x = 0;
	int entvalue = -1;
	hack::Vector entvec;
	if(!sument) {
		unsigned long entitypointer;
		csgo.Read((void*) (csgo.addressOfEntityList + x * 0x20), &entitypointer, sizeof(unsigned long));
		csgo.Read((void*) (entitypointer + 0x16c), &entvec, sizeof(hack::Vector));
		csgo.Read((void*) (entitypointer + 0x134), &entvalue, sizeof(int));
	} 
	

    //if(!(playerResource->m_iTeam[x] == 2 || playerResource->m_iTeam[x] == 3)) continue;
    // http://stackoverflow.com/questions/9158150/colored-output-in-c
    if(playerResource->m_iHealth[x]) {
    	if(playerResource->m_iTeam[x] == 2) cout << "\033[38;5;196m";
    	else cout << "\033[38;5;039m";
    } else {
    	cout << "\033[38;5;244m";
    }
    if(sument) cout << "\033[38;5;227m";
    if(sument) cout << "\n" << "SUM:" << " ";
    else cout << "\n  " << setw(2) << x << " ";
    cout
    << setw(3) << playerResource->m_iKills[x]
    << setw(3) << playerResource->m_iAssists[x]
    << setw(3) << playerResource->m_iDeaths[x]
    << setw(3) << playerResource->m_iMVPs[x]
    << setw(4) << playerResource->m_iScore[x]
    << setw(6) << (playerResource->m_iHealth[x] > 0 ? to_string(playerResource->m_iHealth[x]) : "-")
    << setw(6) << (playerResource->m_iArmor[x] > 0 ? to_string(playerResource->m_iArmor[x]) : "-")
    << (playerResource->m_bHasHelmet[x] ? "•" : " ")
    << " " << (playerResource->m_bHasDefuser[x] ? "D" : " ") << " ";

    if(iscompetitive)
	    cout << setw(6) << (sument ? to_string(playerResource->m_iCompetitiveRanking[x]) : csgoranks[playerResource->m_iCompetitiveRanking[x]])
	    << setw(5) << playerResource->m_iCompetitiveWins[x];
    else
    	cout << " ";
    cout << " "
    << setw(3) << (playerResource->m_iPing[x] > 0 ? to_string(playerResource->m_iPing[x]) : " ")
    << setw(5) << (playerResource->m_nPersonaDataPublicLevel[x] > 0 ? to_string(playerResource->m_nPersonaDataPublicLevel[x]) : " ")
    << setw(4) << (playerResource->m_nPersonaDataPublicCommendsLeader[x] >= 0 ? to_string(playerResource->m_nPersonaDataPublicCommendsLeader[x]) : " ")
    << setw(4) << (playerResource->m_nPersonaDataPublicCommendsTeacher[x] >= 0 ? to_string(playerResource->m_nPersonaDataPublicCommendsTeacher[x]) : " ")
    << setw(4) << (playerResource->m_nPersonaDataPublicCommendsFriendly[x] >= 0 ? to_string(playerResource->m_nPersonaDataPublicCommendsFriendly[x]) : " ");
    if(!sument) {
    	cout << "    " << entvalue << " / " << entvec.x << "," << entvec.y << "," << entvec.z;
    }


	cout << "\033[0m";
}

void loadPlayerResource(hack::CSPlayerResource* playerResource) {
	csgo.Read((void*) (csgo.addressOfPlayerResources + 0x1180), &playerResource->m_iPing, sizeof(int[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x1284), &playerResource->m_iKills, sizeof(int[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x1388), &playerResource->m_iAssists, sizeof(int[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x148c), &playerResource->m_iDeaths, sizeof(int[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x15d4), &playerResource->m_iTeam, sizeof(int[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x1820), &playerResource->m_iHealth, sizeof(int[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x1d4c), &playerResource->m_iMVPs, sizeof(int[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x1ed4), &playerResource->m_iArmor, sizeof(int[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x1fd8), &playerResource->m_iScore, sizeof(int[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x20dc), &playerResource->m_iCompetitiveRanking, sizeof(int[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x21e0), &playerResource->m_iCompetitiveWins, sizeof(int[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x5124), &playerResource->m_nPersonaDataPublicLevel, sizeof(int[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x5228), &playerResource->m_nPersonaDataPublicCommendsLeader, sizeof(int[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x532c), &playerResource->m_nPersonaDataPublicCommendsTeacher, sizeof(int[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x5430), &playerResource->m_nPersonaDataPublicCommendsFriendly, sizeof(int[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x1e91), &playerResource->m_bHasHelmet, sizeof(bool[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x1e50), &playerResource->m_bHasDefuser, sizeof(bool[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x17dc), &playerResource->m_bAlive, sizeof(bool[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x1590), &playerResource->m_bConnected, sizeof(bool[64]));
    csgo.Read((void*) (csgo.addressOfPlayerResources + 0x1cb4), &playerResource->m_iPlayerC4, sizeof(int));
}

void dumpScoreboardSide(hack::CSPlayerResource* playerResource, vector<int> side, bool iscompetitive = false) {
    int y = 0;
    hack::CSPlayerResource sidesum;
    memset(&sidesum, 0, sizeof sidesum);
    for (auto &x : side) {
    	sidesum.m_iKills[0] += playerResource->m_iKills[x];
    	sidesum.m_iAssists[0] += playerResource->m_iAssists[x];
    	sidesum.m_iDeaths[0] += playerResource->m_iDeaths[x];
    	sidesum.m_iTeam[0] += playerResource->m_iTeam[x];
    	sidesum.m_iHealth[0] += playerResource->m_iHealth[x];
    	sidesum.m_iMVPs[0] += playerResource->m_iMVPs[x];
    	sidesum.m_iArmor[0] += playerResource->m_iArmor[x];
    	sidesum.m_iScore[0] += playerResource->m_iScore[x];
    	sidesum.m_iPing[0] += playerResource->m_iPing[x];
    	sidesum.m_iCompetitiveRanking[0] += playerResource->m_iCompetitiveRanking[x];
    	sidesum.m_iCompetitiveWins[0] += playerResource->m_iCompetitiveWins[x];
    	sidesum.m_nPersonaDataPublicCommendsLeader[0] += playerResource->m_nPersonaDataPublicCommendsLeader[x];
    	sidesum.m_nPersonaDataPublicCommendsTeacher[0] += playerResource->m_nPersonaDataPublicCommendsTeacher[x];
    	sidesum.m_nPersonaDataPublicCommendsFriendly[0] += playerResource->m_nPersonaDataPublicCommendsFriendly[x];

    	if(y % 60 == 0) {
        	cout << "\n   # " << setw(3) << "K" << setw(3) << "A" << setw(3) << "D" << " MVP" << "  S    HP   Arm      " << (iscompetitive ? "Rank Wins " : "  ") << "  P  LVL   L   T   F";
    	}
    	y++;
    	dumpEntityScoreboard(playerResource, x, iscompetitive);
    }
    dumpEntityScoreboard(&sidesum, -1, iscompetitive);
}

int main(int argc, char* argv[]) {
    if (getuid() != 0) {
        cout << "You should run this as root." << endl;
        return 0;
    }

    bool drawOverlay = true;
    bool addrDump = false;
    bool quiet = false;

    if(argc > 1) {
        for (int i = 1; i < argc; i++) {
            if(string(argv[i]) == string("-quiet") || string(argv[i]) == string("-q")) {
                quiet = true;
            }
        }
        for (int i = 1; i < argc; i++) {
            if(string(argv[i]) == string("--help") || string(argv[i]) == string("-h") || string(argv[i]) == string("-help")) {
                cout << "Available options:\n";
                cout << "\t-novel\t - Disable colorful velocity indicators.\n";
                cout << "\t-noover\t - Disable overlay completely.\n";
                cout << "\t-addrdump\t - Show addresses found by signatures.\n";
                cout << "\t-q|-quiet\t - Suppress all output.\n";
            }
            if(string(argv[i]) == string("-novel")) {
                csgo.drawVelocity = false;
                if(!quiet) cout << ">>>>> Disabling Velocity indicator!\n"; 
            } else if(string(argv[i]) == string("-noover")) {
                drawOverlay = false;
                if(!quiet) cout << ">>>>> Disabling overlay!\n"; 
            } else if(string(argv[i]) == string("-addrdump")) {
                addrDump = true;
            }
         }
    }

    //cout << "s0beit linux hack version 1.3" << endl;

    dlog::init(!quiet);
    dlog::put("Hack loaded...");

    Display *g_display = XOpenDisplay(0);

    draw.init();
    draw.toggleoverlay(true);

    // http://tomaka.github.io/glium//x11_dl/keysym/index.html
    int keycodeGlow = XKeysymToKeycode(g_display, XK_Alt_L);
    int keycodeGlowOthers = XKeysymToKeycode(g_display, XK_Control_R);
    int keycodeNoFlash = XKeysymToKeycode(g_display, XK_Control_R);
    int keycodeCaptureInput = XKeysymToKeycode(g_display, XK_Super_R);
    int keycodeDumpPlayers = XKeysymToKeycode(g_display, XK_Tab);
    int keycodeDisablePostProcessing = XKeysymToKeycode(g_display, XK_KP_1);
    int keycodeNoHands = XKeysymToKeycode(g_display, XK_KP_2);
    //int keycodeBHopEnable = XKeysymToKeycode(g_display, XK_F9);
    //int keycodeBHop = XKeysymToKeycode(g_display, XK_space);

    if(!quiet) cout << "Waiting for csgo.";
    while (true) {
        if (remote::FindProcessByName("csgo_linux64", &csgo)) {
            break;
        }
        cout << ".";
        usleep(1000000);
    }

    if(!quiet) cout << endl << "CSGO Process Located [" << csgo.GetPath() << "][" << csgo.GetPid() << "]" << endl << endl;

    client.start = 0;

    while (client.start == 0) {
        if (!csgo.IsRunning()) {
            if(!quiet) cout << "Exited game before client could be located, terminating" << endl;
            return 0;
        }

        csgo.ParseMaps();

        for (auto region : csgo.regions) {
            if (region.filename.compare("client_client.so") == 0 && region.executable) {
                if(addrDump) cout << "client_client.so: [" << std::hex << region.start << "][" << std::hex << region.end << "][" <<
                region.pathname << "]" << endl;
                client = region;
                csgo.addressOfClientModule = region.start;
                break;
            }
        }

        usleep(500);
    }

    if(addrDump) cout << "GlowObject Size: " << std::hex << sizeof(hack::GlowObjectDefinition_t) << endl;

    if(addrDump) cout << "Found client_client.so [" << std::hex << client.start << "]" << endl;
    client.client_start = client.start;

    void* foundGlowPointerCall = client.find(csgo, GLOWOBJECT_SIGNATURE, GLOWOBJECT_MASK);
    if(addrDump) cout << "Glow Pointer Call Reference: " << std::hex << foundGlowPointerCall << " | Offset: " << (unsigned long) foundGlowPointerCall - client.start << endl;
    
    unsigned long call = csgo.GetCallAddress(foundGlowPointerCall);
    if(addrDump) cout << "Glow function address: " << std::hex << call << " - off: " << call - client.start << endl;

    unsigned int addressOfGlowPointerOffset;
    if (!csgo.Read((void*) (call + 0x10), &addressOfGlowPointerOffset, sizeof(unsigned int))) {
        cout << "Unable to read address of glow pointer" << endl;
        return 0;
    }
    if(addrDump) cout << "Glow Array offset: " << std::hex << addressOfGlowPointerOffset << endl << endl;

    csgo.addressOfGlowPointer = (call + 0x10) + addressOfGlowPointerOffset + 0x4;
    if(addrDump) cout << "Glow Array pointer " << std::hex << csgo.addressOfGlowPointer << endl << endl;

    /*** >>> LOCAL PLAYER OFFSET <<< ***/
    long foundLocalPlayerLea = (long)client.find(csgo,
                                             "\x48\x89\xe5\x74\x0e\x48\x8d\x05\x00\x00\x00\x00", //27/06/16
                                             "xxxxxxxx????");
    csgo.addressOfLocalPlayer = csgo.GetCallAddress((void*)(foundLocalPlayerLea+0x7));
    
    // 44 89 e8 c1 e0 11 c1 f8 1f 83 e8 03 45 84 e4 74 ?? 21 d0
    unsigned long foundAlt1Mov = (long)client.find(csgo,
                                             "\x44\x89\xe8\xc1\xe0\x11\xc1\xf8\x1f\x83\xe8\x03\x45\x84\xe4\x74\x00\x21\xd0", //10/07/16
                                             "xxxxxxxxxxxxxxxx?xx");
    unsigned long foundEngineBase = (long)client.find(csgo,
                                             "\xF3\x0F\x11\x80\x00\x00\x00\x00\xD9\x46\x08\xD9\x05\x00\x00\x00\x00\xE8\x00\x00\x00\x00", //10/07/16
                                             "xxxx????xxxxx????x????");
    if(addrDump) cout << ">>> EngineBase: 0x" << std::hex << foundEngineBase << endl << endl;

    csgo.addressOfAlt1 = csgo.GetCallAddress((void*)(foundAlt1Mov+20));
    if(addrDump) cout << ">>> Address of alt1: 0x" << std::hex << csgo.addressOfAlt1 << endl << endl;

    unsigned long foundAttackMov = (long)client.find(csgo,
                                             "\x44\x89\xe8\x83\xe0\x01\xf7\xd8\x83\xe8\x03\x45\x84\xe4\x74\x00\x21\xd0", //10/07/16
                                             "xxxxxxxxxxxxxxx?xx");
    csgo.addressOfForceAttack = csgo.GetCallAddress((void*)(foundAttackMov+19));
    if(addrDump) cout << ">>> Address of forceAttack: 0x" << std::hex << csgo.addressOfForceAttack << endl << endl;


    unsigned long foundForceJumpMov = (long)client.find(csgo,
                                             "\x44\x89\xe8\xc1\xe0\x1d\xc1\xf8\x1f\x83\xe8\x03\x45\x84\xe4\x74\x08\x21\xd0", //01/09/16
                                             "xxxxxxxxxxxxxxxx?xx");
    csgo.m_oAddressOfForceJump = csgo.GetCallAddress((void*)(foundForceJumpMov+26));
    if(addrDump) cout << ">>> Address of forceJump: 0x" << std::hex << csgo.m_oAddressOfForceJump << endl << endl;

    unsigned long foundPlayerResourcesInstr = (long) client.find(csgo, PLAYERRESOURCES_SIGNATURE, PLAYERRESOURCES_MASK);
    if(addrDump) cout << ">>> raw PlayerResources instr. pointer: 0x" << std::hex << foundPlayerResourcesInstr << endl << endl;
    
    csgo.addressOfPlayerResourcesPointer = csgo.GetAbsoluteAddress((void*)(foundPlayerResourcesInstr), 3, 7);
    if(addrDump) cout << ">>> Address of PlayerResources pointer: 0x" << std::hex << csgo.addressOfPlayerResourcesPointer << " - " << csgo.addressOfPlayerResourcesPointer - client.client_start << endl << endl;
    
    unsigned long foundEntityListInstr = (long) client.find(csgo, ENTITYLIST_SIGNATURE, ENTITYLIST_MASK);
    if(addrDump) cout << ">>> raw EntityList instr. pointer: 0x" << std::hex << foundEntityListInstr << endl << endl;
    
    csgo.addressOfEntityList = csgo.GetAbsoluteAddress((void*)(foundEntityListInstr), 3, 7);
    if(addrDump) cout << ">>> Address of EntityList pointer: 0x" << std::hex << csgo.addressOfEntityList << " - " << csgo.addressOfEntityList - client.client_start << endl << endl;
    

    unsigned long foundOverridePostProcessingDisableInstr = (long) client.find(csgo, OVERRIDEPOSTPROCESSINGDISABLE_SIGNATURE, OVERRIDEPOSTPROCESSINGDISABLE_MASK);
    if(addrDump) cout << ">>> raw s_bOverridePostProcessingDisable instr. pointer: 0x" << std::hex << foundOverridePostProcessingDisableInstr << endl << endl;

    /*unsigned long absaddr2;
    csgo.Read((void*) (foundOverridePostProcessingDisableInstr), &absaddr2, sizeof(unsigned long));
    if(addrDump) cout << ">>> #1 s_bOverridePostProcessingDisable pointer: 0x" << std::hex << absaddr2 << endl << endl;
    */
    csgo.addressOfOverridePostProcessingDisablePointer = csgo.GetAbsoluteAddress((void*)(foundOverridePostProcessingDisableInstr), 2, 7);
    if(addrDump) cout << ">>> Address of s_bOverridePostProcessingDisable: 0x" << std::hex << csgo.addressOfOverridePostProcessingDisablePointer << " - " << csgo.addressOfOverridePostProcessingDisablePointer - client.client_start << endl << endl;

    unsigned long foundDeltaTickInstr = (long) client.find(csgo, DELTATICK_SIGNATURE, DELTATICK_MASK);
    if(addrDump) cout << ">>> raw nDeltaTick instr. pointer: 0x" << std::hex << foundDeltaTickInstr << endl << endl;

    /*unsigned long absaddr2;
    csgo.Read((void*) (foundOverridePostProcessingDisableInstr), &absaddr2, sizeof(unsigned long));
    if(addrDump) cout << ">>> #1 s_bOverridePostProcessingDisable pointer: 0x" << std::hex << absaddr2 << endl << endl;
    */
    //csgo.addressOfOverridePostProcessingDisablePointer = csgo.GetAbsoluteAddress((void*)(foundDeltaTickInstr), 2, 7);
    //if(addrDump) cout << ">>> Address of s_bOverridePostProcessingDisable: 0x" << std::hex << csgo.addressOfOverridePostProcessingDisablePointer << " - " << csgo.addressOfOverridePostProcessingDisablePointer - client.client_start << endl << endl;


    cout << "\n";

    

    int m_iPing[64];        // 0x1180
    int m_iKills[64];        // 0x1284
    int m_iAssists[64];        // 0x1388
    int m_iDeaths[64];        // 0x148c
    int m_iTeam[64];        // 0x15d4
    int m_iHealth[64];        // 0x1820
    int m_iMVPs[64];        // 0x1d4c
    int m_iArmor[64];       // 0x1ed4
    bool m_bHasHelmet[64];  // 0x1e91
    bool m_bHasDefuser[64]; // 0x1e50
    int m_iScore[64];       // 0x1fd8
    int m_iCompetitiveRanking[64];       // 0x20dc
    int m_iCompetitiveWins[64];       // 0x21e0

    /*unsigned long rankRevealFunc = (long)client.find(csgo,  MSGFUNC_SERVERRANKREVEALALL_SIGNATURE,  MSGFUNC_SERVERRANKREVEALALL_MASK);
    cout << ">>> Address of rankRevealFunc: 0x" << std::hex << rankRevealFunc << endl << endl;
    MsgFunc_ServerRankRevealAll = reinterpret_cast<MsgFunc_ServerRankRevealAllFn>(rankRevealFunc);*/

    for (int i = 0; i < 255; ++i) {
        csgo.speedgradient[i] = draw.createXColorFromRGBA(i, 255-i, 0, (i-100 < 0 ? 0 : i-100));
    }

    for (int i = 0; i < 255; ++i) {
        csgo.blackgradient[i] = draw.createXColorFromRGBA(0, 0, 0, (i < 70 ? 70 : i));
    }

    XEvent ev;
    char keys[32];
    char lastkeys[32];

    pthread_create( &bhopthread, NULL, bhoploop, NULL);
    //pthread_create( &crosshairthread, NULL, crosshairloop, NULL);

    winmgr = new EWindowsManager(&draw);
    //winmgr->init();
    
    int nohandsi = 0;
    int nohandsorig = 0;
	char nohandsc[ 256 ] = { 0 };
    while (csgo.IsRunning()) {
    	csgo.Read((void*) csgo.addressOfLocalPlayer, &csgo.localPlayerOffset, sizeof(long));
      XQueryKeymap(g_display, keys);
      for (unsigned i = 0; i < sizeof(keys); ++i) {
        if (keys[i] != lastkeys[i]) {
          // check which key got changed
          for (unsigned j = 0, test = 1; j < 8; ++j, test *= 2) {
            // if the key was pressed, and it wasn't before, print this
            if ((keys[i] & test) && ((keys[i] & test) != (lastkeys[i] & test))) {
              const int code = i * 8 + j;
              //cout << code << "\n";

              if(code == keycodeGlow) {
                if(!quiet) cout << (csgo.shouldGlow ? "00000 GLOW OFF" : ">>>>> GLOW ON") << endl;
                csgo.shouldGlow = !csgo.shouldGlow;
                csgo.justglowoff = true;
              } else if(code == keycodeGlowOthers) {
                if(!quiet) cout << (csgo.glowOthers ? "00000 Glow -> OTHERS OFF" : ">>>>> Glow -> OTHERS ON") << endl;
                csgo.glowOthers = !csgo.glowOthers;
                csgo.justglowothersoff = true;
              } else if(code == keycodeNoFlash && false) {
                if(!quiet) cout << (csgo.noFlash ? "00000 NOFLASH OFF" : ">>>>> NOFLASH ON") << endl;
                csgo.noFlash = !csgo.noFlash;
              } else if(code == keycodeCaptureInput) {
                if(!quiet) cout << (shouldCaptureInput ? "00000 Capture Input OFF" : ">>>>> Capture Input ON!") << endl;
                shouldCaptureInput = !shouldCaptureInput;
                if(false && shouldCaptureInput) {
                  winmgr->setInputAreas();
                  draw.startCaptureInput();
                } else {
                  //draw.stopCaptureInput();
                }
                
              } else if(code == keycodeDisablePostProcessing) {
                if(!quiet) cout << (shouldDisablePostProcessing ? "00000 PostProcessing OFF" : ">>>>> PostProcessing ON") << endl;
                shouldDisablePostProcessing = !shouldDisablePostProcessing;
                
              } else if(code == keycodeNoHands) {
              	// m_nModelIndex              	
              	csgo.Read((void*) (csgo.localPlayerOffset + 0x28c), &nohandsorig, sizeof(int));
                if(!quiet) cout << (shouldNoHands ? "00000 NoHands OFF" : ">>>>> NoHands ON") << " > " << dec << nohandsorig << endl;
                if(shouldNoHands) {
                	csgo.Write((void*) (csgo.localPlayerOffset + 0x28c), &nohandsorig, sizeof(int));
                }
                shouldNoHands = !shouldNoHands;

                //nohandsi++;
                //cout << "NoHands: " << nohandsi << endl;
                
              } else if(code == keycodeDumpPlayers) {
                /*cout << "\n       ======== Dumping EntityList ========\n" << dec;
                for (int i = 0; i < 63; ++i) {
                	
                }*/
                cout << "\n       ======== Dumping CSPlayerResource ========\n" << dec;
                csgo.Read((void*) (csgo.addressOfPlayerResourcesPointer), &csgo.addressOfPlayerResources, sizeof(unsigned long));
                csgo.Read((void*) (csgo.addressOfPlayerResources), &playerResourceObj, sizeof(hack::CSPlayerResource));

    /*int m_iPing[64];        // 0x1180
    int m_iKills[64];        // 0x1284
    int m_iAssists[64];        // 0x1388
    int m_iDeaths[64];        // 0x148c
    int m_iTeam[64];        // 0x15d4
    int m_iHealth[64];        // 0x1820
    int m_iMVPs[64];        // 0x1d4c
    int m_iArmor[64];       // 0x1ed4
    bool m_bHasHelmet[64];  // 0x1e91
    bool m_bHasDefuser[64]; // 0x1e50
    int m_iScore[64];       // 0x1fd8
    int m_iCompetitiveRanking[64];       // 0x20dc
    int m_iCompetitiveWins[64];       // 0x21e0
    */
	loadPlayerResource(&playerResourceObj);

    vector<int> Tside;
    vector<int> CTside;
    hack::CSPlayerResource Tsum;
    hack::CSPlayerResource CTsum;

    for (int x = 0; x < 63; ++x) {
    	if(!playerResourceObj.m_bConnected[x]) continue;
    	if(playerResourceObj.m_iTeam[x] == 2) Tside.push_back(x);
    	if(playerResourceObj.m_iTeam[x] == 3) CTside.push_back(x);
    }
    std::sort (Tside.begin(), Tside.end(), entityScoreboardSort);
    std::sort (CTside.begin(), CTside.end(), entityScoreboardSort);

    bool iscompetitive = false;
    for (int x = 0; x < 63; ++x) {
    	if(playerResourceObj.m_iCompetitiveWins[x] > 0) {
    		iscompetitive = true;
    		break;
    	}
    }

    dumpScoreboardSide(&playerResourceObj, CTside, iscompetitive);
    cout << "\n";
    dumpScoreboardSide(&playerResourceObj, Tside, iscompetitive);

                cout << "\n";
              }

            }
          }
        }

        lastkeys[i] = keys[i];
      }

      
      draw.startdraw();

      hack::Glow(&csgo, &client, &draw);
      usleep(1000);
      if(draw.overlayenabled) {
        string toggledstr = ( string(csgo.shouldGlow ? (csgo.glowOthers ? "GLOW All" : "GLOW Players") : "") + string(csgo.noFlash ? " + NF" : "") );
        if(toggledstr != "") {
          draw.drawString(toggledstr.c_str(), draw.WIDTH/2, draw.HEIGHT-draw.font_height*2, draw.ltblue, draw.blacka, 1);
        }
      }

      if(shouldCaptureInput) {
        //winmgr->drawWindows();
      }
      if(shouldNoHands) {
      	csgo.Write((void*) (csgo.localPlayerOffset + 0x28c), &nohandsi, sizeof(int));
      }
      if(shouldDisablePostProcessing) {
      	bool disable = true;
      	csgo.Write((void*) (csgo.addressOfOverridePostProcessingDisablePointer), &disable, sizeof(bool));
      } else {
      	bool disable = false;
      	csgo.Write((void*) (csgo.addressOfOverridePostProcessingDisablePointer), &disable, sizeof(bool));
      }
      
      
      draw.enddraw();

      csgo.justglowoff = false;
      csgo.justglowothersoff = false;
    }

    csgo.running = false;
    pthread_join(bhopthread, NULL);

    if(!quiet) cout << "Game ended." << endl;

    draw.halt();

    return 0;
}
