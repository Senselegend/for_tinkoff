#include "hooks.h"
#include "GameUtils.h"
#include "Draw.h"
#include "Menu.h"
#include "ESP.h"
#include "Misc.h"
#include "global.h"
#include "SpoofedConvar.h"
#include "Hitmarker.h"
#include "GrenadePrediction.h"
#include "global.h"
#include "AW_hitmarker.h"
#include "event_log.h"

#define TICK_INTERVAL			( g_pGlobals->interval_per_tick )
#define TIME_TO_TICKS( dt )		( floorf(( 0.5f + (float)(dt) / TICK_INTERVAL ) ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )

bool first_frame_passed = false;
std::string sPanel = XorStr("FocusOverlayPanel");

void __fastcall Hooks::PaintTraverse(void* ecx/*thisptr*/, void* edx, unsigned int vgui_panel, bool force_repaint, bool allow_force) // cl
{
	if (Clientvariables->Visuals.EspEnable && Clientvariables->Visuals.Noscope && strcmp("HudZoom", g_pPanel->GetName(vgui_panel)) == 0)
		return;

	panelVMT->GetOriginalMethod<PaintTraverseFn>(41)(ecx, vgui_panel, force_repaint, allow_force);

	static bool bSpoofed = false;
	static bool bSpoofed1 = false;

	if (Clientvariables->Misc.TPKey > 0 && !bSpoofed)
	{
		ConVar* svcheats = g_pCvar->FindVar("sv_cheats");
		SpoofedConvar* svcheatsspoof = new SpoofedConvar(svcheats);
		svcheatsspoof->SetInt(1);
		bSpoofed = true;
	}


	const char* pszPanelName = g_pPanel->GetName(vgui_panel);

	if (!strstr(pszPanelName, sPanel.data()))
		return;

	int cur_height, cur_width; g_pEngine->GetScreenSize(cur_width, cur_height);

	if (!first_frame_passed || cur_width != G::Screen.width || cur_height != G::Screen.height)
	{
		first_frame_passed = true;
		g_Draw.Init();

		g_pEngine->GetScreenSize(cur_width, cur_height);
		G::Screen.height = cur_height;
		G::Screen.width = cur_width;
	}
	if (g_pEngine->IsInGame() && g_pEngine->IsConnected())
	{
		g_Draw.StringA(g_Draw.font_espinfo, false, 1, G::Screen.height - 188, 155, 111, 0, 255, "%s", ".");

	}

	if (g_pEngine->IsInGame() && g_pEngine->IsConnected() && G::LocalPlayer)
	{

		Eventlog::Draw();


		if (Clientvariables->Antiaim.Indicator)
		{
			auto iFlagsBackup = *G::LocalPlayer->GetFlags();
			auto bChocked = TIME_TO_TICKS(G::LocalPlayer->GetSimulationTime() - G::LocalPlayer->GetOldSimulationTime()) > 1;
			auto dsd = G::LocalPlayer->GetEyeAnglesPtr();

			bool BrokenLBY = (abs(G::RealAngle.y - G::LocalPlayer->LowerBodyYaw()) > 35.f);

			float real_yaw;

			if (G::LocalPlayer->GetHealth() > 0) {
				if (G::LocalPlayer->GetVelocity().Length() < 0.1f)
				{
					if (BrokenLBY)
						g_Draw.StringA(g_Draw.font_build, false, 10, G::Screen.height - 78, 125, 176, 17, 255, "%s", "LBY");
					else
						g_Draw.StringA(g_Draw.font_build, false, 10, G::Screen.height - 78, 241, 33, 13, 255, "%s", "LBY");
				}
				else
				{
					g_Draw.StringA(g_Draw.font_build, false, 10, G::Screen.height - 78, 241, 33, 13, 255, "%s", "LBY");
				}
			}
			if (Clientvariables->Antiaim.breakaa == 1 && G::LocalPlayer->GetHealth() > 0 && Clientvariables->Misc.AntiUT)
			{
				int w, h;
				int centerW, centerh, topH;
				g_pEngine->GetScreenSize(w, h);
				centerW = w / 2;
				centerh = h / 2;

				int Right, Left, Back;
				Right = VK_RIGHT;
				Left = VK_LEFT;
				Back = VK_DOWN;

				static bool side1 = false;
				static bool side2 = false;
				static bool inside1 = false;
				static bool inside2 = false;
				static bool back1 = false;
				static bool back2 = false;

				if (GetAsyncKeyState(Right)) {
					side1 = true; //v
					side2 = false;//v
					inside1 = true; //f
					inside2 = false; //f
					back1 = false; //v
					back2 = true;//f
				}
				if (GetAsyncKeyState(Left)) {
					side1 = false; //v
					side2 = true; //v
					inside1 = false; //f
					inside2 = true; //f
					back1 = false;
					back2 = true;
				}
				if (GetAsyncKeyState(Back)) {
					side1 = false;
					side2 = false;
					inside1 = true;
					inside2 = true;
					back1 = true;
					back2 = false;
				}

				if (side1) //v
					g_Draw.StringA(g_Draw.font_build, false, centerW + 58, centerh - 18, 0, 111, 255, 255, "%s", ">");
				if (inside1) //f
					g_Draw.StringA(g_Draw.font_build, false, centerW - 80, centerh - 18, 255, 255, 255, 255, "%s", "<");
				if (side2) //v
					g_Draw.StringA(g_Draw.font_build, false, centerW - 80, centerh - 18, 0, 111, 255, 255, "%s", "<");
				if (inside2)//f
					g_Draw.StringA(g_Draw.font_build, false, centerW + 58, centerh - 18, 255, 255, 255, 255, "%s", ">");
				if (back1) //v
					g_Draw.StringA(g_Draw.font_build, false, centerW - 10, centerh + 45, 0, 111, 255, 255, "%s", "V");
				if (back2) //f
					g_Draw.StringA(g_Draw.font_build, false, centerW - 10, centerh + 45, 255, 255, 255, 255, "%s", "V");
			}
		}

		if (Clientvariables->Visuals.DamageIndicators)
		{
			G::CurrTime = g_pGlobals->interval_per_tick * (G::LocalPlayer->GetTickBase() + 1);

			if (!G::DamageHit.empty())
			{
				for (auto it = G::DamageHit.begin(); it != G::DamageHit.end();) {
					if (G::CurrTime > it->ExpireTime) {
						it = G::DamageHit.erase(it);
						continue;
					}
					it->Draw();
					++it;
				}
			}
		}

		if (Clientvariables->Visuals.RemoveParticles)
		{
			auto postprocessing = g_pCvar->FindVar("mat_postprocess_enable");
			auto postprocessingspoof = new SpoofedConvar(postprocessing);
			postprocessingspoof->SetInt(0);
		}

		if (Clientvariables->Visuals.Noscope)
		{
			g_ESP->DrawScope(G::LocalPlayer);
			//auto postprocessing = g_pCvar->FindVar("mat_postprocess_enable");
			//auto postprocessingspoof = new SpoofedConvar(postprocessing);
			//postprocessingspoof->SetInt(0);
		}


		if (Clientvariables->Visuals.crosssnip)
		{
			static auto cross1 = g_pCvar->FindVar(("crosshair"));
			{
				if (G::LocalPlayer->IsScoped())
				{
					cross1->SetValue(0);
				}
				else
				{
					cross1->SetValue(1);
				}

				if (!G::LocalPlayer->IsScoped() && G::LocalPlayer->isAlive())
				{
					auto cross = g_pCvar->FindVar("weapon_debug_spread_show");
					auto cross_spoofed = new SpoofedConvar(cross);
					cross_spoofed->SetInt(3);
				}
			}
		}
		g_ESP->Loop();
		grenade_prediction::instance().Paint();
	}
}