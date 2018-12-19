#include <iostream>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <X11/keysym.h>
#include <X11/keysymdef.h>

#include <X11/extensions/XTest.h>

#include <unistd.h>
#include <string>
#include <cstdio>
#include <cstdlib>

#include <chrono>
#include <thread>

#include "remote.h"
#include "netvar.h"
#include "hack.h"
#include "logger.h"
#include "draw.h"

using namespace std;

Draw draw;
remote::Handle csgo;
remote::MapModuleMemoryRegion client;
pthread_t bhopthread;

void *bhoploop(void *arg) {
	unsigned int alt1 = 0;
	unsigned int jump = 6;
	unsigned long flags;
	ptrdiff_t m_fFlags = netvar::GetOffset("CBasePlayer", "m_fFlags");

	while (csgo.running) {
		csgo.Read((void*) (csgo.m_addressOfAlt1), &alt1, sizeof(int));

		if (alt1 == 5) {
			csgo.Read((void*) (csgo.m_localPlayer + m_fFlags), &flags, sizeof(unsigned long));        
			if(flags & FL_ONGROUND) {
				csgo.Write((void*) (csgo.m_oAddressOfForceJump), &jump, sizeof(int));
			}
		}
		usleep(1000);
	}
	return NULL;
}

int main(int argc, char const *argv[]) {
	Logger::init();

	if (getuid() != 0) {
		Logger::error(string("Cannot start linux-csgo-external as ") + UNDERLINE + "NON ROOT" RESET RED " user.");
		return 0;
	}

	Display* g_display = XOpenDisplay(0);
	draw.init();
	draw.toggleoverlay(true);

	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "-noover") == 0) {
			draw.toggleoverlay(false);
		}
	}

	int keycodeGlow = XKeysymToKeycode(g_display, XK_Alt_L);
	int keycodeGlowOthers = XKeysymToKeycode(g_display, XK_Control_R);
	int keycodeDisablePostProcessing = XKeysymToKeycode(g_display, XK_KP_1);

	while (true) {
		if (remote::FindProcessByName("csgo_linux64", &csgo))
			break;

		usleep(500);
	}

	stringstream ss;
	ss << "\t  CSGO Process ID:\t [" << csgo.GetPid() << "]";
	Logger::normal (ss.str());

	client.start = 0;

	while (client.start == 0) {
		if (!csgo.IsRunning()) {
			Logger::error("The game was closed before I could find the client library inside of csgo");
			return 0;
		}

		csgo.ParseMaps();

		for (auto region : csgo.regions) {
			if (region.filename.compare("client_panorama_client.so") == 0 && region.executable) {
				client = region;
				break;
			}
		}

		usleep(500);
	}
	client.client_start = client.start;

	unsigned long pEngine = remote::getModule("engine_client.so", csgo.GetPid());

	if (pEngine == 0) {
		Logger::error("Couldn't find engine module inside of csgo");
		return 0;
	}
	csgo.a_engine_client = pEngine;

	Logger::address ("client_client.so:\t", client.start);
	Logger::address ("engine_client.so:\t", pEngine);

	unsigned long codeOfClientClassHead = (long)client.find(csgo,
		"\x44\x89\xEA\xB8\x01\x00\x00\x00\x44\x89\xE9\xC1\xFA\x05\xD3\xE0\x48\x63\xD2\x41\x09\x04\x91\x48\x8B\x05\x00\x00\x00\x00\x8B\x53\x14\x48\x8B\x00\x48\x85\xC0\x75\x1B\xE9",
		"xxxxxxxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxx");

	csgo.m_addressOfClientClassHead = csgo.GetAbsoluteAddress((void*) (codeOfClientClassHead + 23), 3, 7);
	csgo.Read((void*) csgo.m_addressOfClientClassHead, &csgo.m_addressOfClientClassHead, sizeof(void*));
	csgo.Read((void*) csgo.m_addressOfClientClassHead, &csgo.m_addressOfClientClassHead, sizeof(void*));
	Logger::address ("ClientClass offset:\t", csgo.m_addressOfClientClassHead);

	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "-d") == 0) {
			string ntdump = netvar::DumpAll(csgo, client);
			cout << ntdump << endl;
		}
	}
	
	unsigned long foundGlowPointerCall = (long)client.find(csgo,
				"\xE8\x00\x00\x00\x00\x49\x8B\x7D\x00\xC7\x40\x38\x00\x00\x00\x00\x48\x8B\x07\xFF\x90", // 2018-12-19
				"x????xxxxxxxxxxxxxxxx") - 1;

	unsigned long glowFunctionCall = csgo.GetAbsoluteAddress((void*)(foundGlowPointerCall), 1, 5);
	Logger::address ("Glow function call:\t", glowFunctionCall);
	
	csgo.m_addressOfGlowPointer = csgo.GetAbsoluteAddress((void*)(glowFunctionCall + 9), 3, 7);
	Logger::address ("GlowObjMan pointer:\t", csgo.m_addressOfGlowPointer);

	unsigned long foundLocalPlayerLea = (long)client.find(csgo,
		"\x48\x89\xe5\x74\x0e\x48\x8d\x05\x00\x00\x00\x00", //27/06/16
		"xxxxxxxx????") + 7;

	csgo.m_addressOfLocalPlayer = csgo.GetCallAddress((void*) foundLocalPlayerLea);
	Logger::address ("LocalPlayer address:\t", csgo.m_addressOfLocalPlayer);

	unsigned long foundAttackMov = (long)client.find(csgo,
		"\x89\xD8\x83\xC8\x01\xF6\xC2\x03\x0F\x45\xD8\x44\x89\x00\x83\xE0\x01\xF7\xD8\x83\xE8\x03", // 2018-07-07
		"xxxxxxxxxxxxx?xxxxxxxx") - 7;
	csgo.m_addressOfForceAttack = csgo.GetAbsoluteAddress((void*) foundAttackMov, 3, 7);
	Logger::address ("Force Attack:\t\t", csgo.m_addressOfForceAttack);

	unsigned long foundAlt1Mov = (long)client.find(csgo,
		"\x89\xD8\x80\xCC\x40\xF6\xC2\x03\x0F\x45\xD8\x44\x89\x00\xC1\xE0\x11\xC1\xF8\x1F\x83\xE8\x03", // 2018-07-07
		"xxxxxxxxxxxxx?xxxxxxxxx") - 7;

	csgo.m_addressOfAlt1 = csgo.GetAbsoluteAddress((void*) foundAlt1Mov, 3, 7);
	Logger::address ("alt1 address:\t\t", csgo.m_addressOfAlt1);	

	csgo.m_oAddressOfForceJump = csgo.m_addressOfAlt1 + 0xC * 5;
	Logger::address ("Force Jump:\t\t", csgo.m_oAddressOfForceJump);

	unsigned long inCrossMov = (long)client.find(csgo,
		"\x31\xC0\x0F\x2F\x83\x00\x00\x00\x00\x76\x14\x8B\x83\x00\x00\x00\x00\x85\xC0\x75\x0A\x8B\x93", // 2018-11-10
		"xxxxx????xxxx????xxxxxx") + 13;

	csgo.Read((void*) inCrossMov, &csgo.m_inCrossOffset, sizeof(int));
	Logger::address ("inCross offset:\t", csgo.m_inCrossOffset);

	unsigned long foundOverridePostProcessingDisableInstr = (long)client.find(csgo,
		"\x55\x48\x89\xE5\x41\x57\x41\x56\x41\x55\x41\x54\x53\x48\x81\xEC\x00\x00\x00\x00\x89\xBD\x18\xFF\xFF\xFF\x80\x3D\x00\x00\x00\x00\x00\x89\xB5\x14\xFF\xFF\xFF",
		"xxxxxxxxxxxxxxxx????xxxxxxxx????xxxxxxx") + 26;

	csgo.m_addressOfOverridePostProcessingDisable = csgo.GetAbsoluteAddress((void*) foundOverridePostProcessingDisableInstr, 2, 7);
	Logger::address ("disablePP offset:\t", csgo.m_addressOfOverridePostProcessingDisable);

	while (!netvar::Cache(csgo, client)) {
		if (!csgo.IsRunning()) {
			cout << "Exited game before netvars could be cached, terminating" << endl;
			return 0;
		}
		usleep(5000);
	}
	cout << "Cached " << std::dec << netvar::GetAllClasses().size() << " networked classes." << endl;

	csgo.offsets.m_iHealth = netvar::GetOffset("CBasePlayer", "m_iHealth");
	csgo.offsets.m_iTeamNum = netvar::GetOffset("CBaseEntity", "m_iTeamNum");

	char keys[32];
	char lastkeys[32];

	pthread_create(&bhopthread, NULL, bhoploop, NULL);

	while (csgo.IsRunning()) {

		XQueryKeymap(g_display, keys);
		for (unsigned i = 0; i < sizeof(keys); ++i) {
			if (keys[i] != lastkeys[i]) {
				// check which key got changed
				for (unsigned j = 0, test = 1; j < 8; ++j, test *= 2) {
					// if the key was pressed, and it wasn't before, print this
					if ((keys[i] & test) && ((keys[i] & test) != (lastkeys[i] & test))) {
						const int code = i * 8 + j;
						if(code == keycodeGlow) {
							cout << (csgo.shouldGlow ? "00000 GLOW OFF" : ">>>>> GLOW ON") << endl;
							csgo.shouldGlow = !csgo.shouldGlow;
							csgo.justglowoff = true;
							//csgo.debugoff += 0x18;
							//cout << hex << csgo.debugoff << endl;
							draw.clearscreen();
						} else if(code == keycodeGlowOthers) {
							cout << (csgo.glowOthers ? "00000 Glow -> OTHERS OFF" : ">>>>> Glow -> OTHERS ON") << endl;
							csgo.glowOthers = !csgo.glowOthers;
							csgo.justglowothersoff = true;
							draw.clearscreen();
						} else if(code == keycodeDisablePostProcessing) {
							cout << (csgo.shouldDisablePostProcessing ? "00000 PostProcessing OFF" : ">>>>> PostProcessing ON") << endl;
							csgo.shouldDisablePostProcessing = !csgo.shouldDisablePostProcessing;
						}
					}
				}
			}
			lastkeys[i] = keys[i];
		}
		
		draw.startdraw();
		hack::Glow(&csgo, &client, &draw);
		usleep(10000);

		if(draw.overlayenabled) {
			string toggledstr = ( string(csgo.shouldGlow ? (csgo.glowOthers ? "GLOW All" : "GLOW Players") : ""));
			if(toggledstr != "") {
				draw.drawString(toggledstr.c_str(), draw.WIDTH/2, draw.HEIGHT-draw.font_height*2, draw.ltblue, draw.blacka, 1);
			}
		}
		if(csgo.shouldDisablePostProcessing) {
			bool disable = true;
			csgo.Write((void*) (csgo.m_addressOfOverridePostProcessingDisable), &disable, sizeof(bool));
		} else {
			bool disable = false;
			csgo.Write((void*) (csgo.m_addressOfOverridePostProcessingDisable), &disable, sizeof(bool));
		}

		draw.enddraw();

		csgo.justglowoff = false;
		csgo.justglowothersoff = false;
	}

	csgo.running = false;
	draw.halt();

	return 0;
}
