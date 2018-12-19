#include "hack.h"

struct iovec g_remote[1024], g_local[1024];
struct hack::GlowObjectDefinition_t g_glow[1024];

int count = 0;
unsigned char spotted = 1;

void hack::Glow(remote::Handle* csgo, remote::MapModuleMemoryRegion* client, Draw* draw) {
	if (!csgo || !client)
		return;

	size_t writeCount = 0;

	csgo->Read((void*) csgo->m_addressOfLocalPlayer, &csgo->m_localPlayer, sizeof(long));
	if (!csgo->m_localPlayer)
		return;

	// Reset
	bzero(g_remote, sizeof(g_remote));
	bzero(g_local, sizeof(g_local));
	bzero(g_glow, sizeof(g_glow));

	hack::CGlowObjectManager manager;
	if (!csgo->Read((void*) csgo->m_addressOfGlowPointer, &manager, sizeof(hack::CGlowObjectManager))) {
		Logger::error ("Failed to read glowClassAddress");
		throw 1;
	}

	size_t count = manager.m_GlowObjectDefinitions.Count;
	void* data_ptr = (void*) manager.m_GlowObjectDefinitions.DataPtr;

	if (!csgo->Read(data_ptr, g_glow, sizeof(hack::GlowObjectDefinition_t) * count)) {
		Logger::error ("Failed to read m_GlowObjectDefinitions");
		throw 1;
	}

	QAngle toread;
	csgo->Read((void*) (csgo->m_localPlayer + 0x3700 + 0x74), &toread, sizeof(QAngle));
	//draw->clearArea(draw->WIDTH-300, 200, 300, 400);
	//draw->drawString((std::string("Velocity: ")+to_string((toread.x))+std::string(" / ")+to_string((toread.y))+std::string(" / ")+to_string((toread.z))).c_str(), draw->WIDTH/2+100, draw->HEIGHT-draw->font_height*1-15, draw->ltblue, draw->blacka, ALIGN_LEFT);

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

	for (unsigned int i = 0; i < count; i++) {
		if (g_glow[i].m_pEntity != NULL) {
			if (g_glow[i].m_pEntity) {
				int team, ihealth;
				csgo->Read((void*) ((uintptr_t) g_glow[i].m_pEntity + csgo->offsets.m_iTeamNum), &team, sizeof(int));
				csgo->Read((void*) ((uintptr_t) g_glow[i].m_pEntity + csgo->offsets.m_iHealth), &ihealth, sizeof(int));

				csgo->Write((void*) ((unsigned long) g_glow[i].m_pEntity + 0xECD), &spotted, sizeof(unsigned char));

				g_glow[i].m_bRenderWhenOccluded = 1;
				g_glow[i].m_bRenderWhenUnoccluded = 0;

				float fhealth = ihealth > 100 ? 100 : ihealth;
				
				if (team == 2) {
					g_glow[i].m_flGlowRed = 1.0f;
					g_glow[i].m_flGlowGreen = fhealth != 0 ? 1.0f-fhealth/100.0f : 0.0f;
					g_glow[i].m_flGlowBlue = 0.0f;
					g_glow[i].m_flGlowAlpha = 0.55f;
				} else if (team == 3) {
					g_glow[i].m_flGlowRed = 0.0f;
					g_glow[i].m_flGlowGreen = fhealth != 0 ? (1.0f-(fhealth/100.0f)) : 0.0f;
					g_glow[i].m_flGlowBlue = 1.0f;
					g_glow[i].m_flGlowAlpha = 0.55f;
				} else {
					g_glow[i].m_flGlowRed = 0.0f;
					g_glow[i].m_flGlowGreen = 1.0f;
					g_glow[i].m_flGlowBlue = 1.0f;
					g_glow[i].m_flGlowAlpha = 0.4f;
				}
				if (!csgo->glowOthers && fhealth < 1) {
					g_glow[i].m_bRenderWhenOccluded = 0;
					g_glow[i].m_bRenderWhenUnoccluded = 0;
				}
				if (fhealth < 3 && fhealth > 0) {
					g_glow[i].m_flGlowRed = 0.0f;
					g_glow[i].m_flGlowGreen = 1.0f;
					g_glow[i].m_flGlowBlue = 0.0f;
					g_glow[i].m_flGlowAlpha = 0.7f;
				}
				if (!csgo->shouldGlow) {
					if (fhealth > 0)
						continue;
					g_glow[i].m_bRenderWhenOccluded = 0;
					g_glow[i].m_bRenderWhenUnoccluded = 0;
				}
				g_glow[i].m_bFullBloomRender = 0;
			}

		}

		if (csgo->shouldGlow || csgo->justglowoff || csgo->justglowothersoff) {
			size_t bytesToCutOffEnd = sizeof(hack::GlowObjectDefinition_t) - g_glow[i].writeEnd();
			size_t bytesToCutOffBegin = (size_t) g_glow[i].writeStart();
			size_t totalWriteSize = (sizeof(hack::GlowObjectDefinition_t) - (bytesToCutOffBegin + bytesToCutOffEnd));

			g_remote[writeCount].iov_base =
				((uint8_t*) data_ptr + (sizeof(hack::GlowObjectDefinition_t) * i)) + bytesToCutOffBegin;
			g_local[writeCount].iov_base = ((uint8_t*) &g_glow[i]) + bytesToCutOffBegin;
			g_remote[writeCount].iov_len = g_local[writeCount].iov_len = totalWriteSize;

			writeCount++;
		}
	}

	process_vm_writev(csgo->GetPid(), g_local, writeCount, g_remote, writeCount, 0);
}
