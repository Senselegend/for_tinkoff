#include "sdk.h"
#include "hooks.h"
#include "Menu.h"
#include "global.h"
#include "Hitmarker.h"
#include "ESP.h"
#include "imgui\imconfig.h"
#include "ImGUI\imgui.h"
#include "ImGUI\imgui_internal.h"
#include "ImGUI\stb_rect_pack.h"
#include "ImGUI\stb_textedit.h"
#include "ImGUI\stb_truetype.h"
#include "ImGUI\imgui_impl_dx9.h"
#include "Items.h"
#include "event_log.h"
#include "Config.h"
#include "imgui\ColorVar.h"
#include "GameUtils.h"

#include "ESP.h"



const char* KeyStrings[] = {
	"",
	"Mouse 1",
	"Mouse 2",
	"Cancel",
	"Middle Mouse",
	"Mouse 4",
	"Mouse 5",
	"",
	"Backspace",
	"Tab",
	"",
	"",
	"Clear",
	"Enter",
	"",
	"",
	"Shift",
	"Control",
	"Alt",
	"Pause",
	"Caps",
	"",
	"",
	"",
	"",
	"",
	"",
	"Escape",
	"",
	"",
	"",
	"",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left",
	"Up",
	"Right",
	"Down",
	"",
	"",
	"",
	"Print",
	"Insert",
	"Delete",
	"",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"",
	"",
	"",
	"",
	"",
	"Numpad 0",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
	"Multiply",
	"Add",
	"",
	"Subtract",
	"Decimal",
	"Divide",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",

};



PresentFn oPresent;

tReset oResetScene;

ImFont* def = nullptr;

void GUI_Init() // Setup for imgui
{
	ImGui::CreateContext();
	ImGui_ImplDX9_Init(FindWindow("Valve001", 0), **reinterpret_cast< IDirect3DDevice9*** >(FindPatternIDA("shaderapidx9.dll", "88 56 10") + 17));
	ImGui::StyleColorsDark();

	ImVec4 *colors = ImGui::GetStyle().Colors;

	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 0.7f;
	style.WindowPadding = ImVec2(8, 8);
	style.WindowMinSize = ImVec2(32, 32);
	style.WindowRounding = 0.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ChildRounding = 0.0f;
	style.FramePadding = ImVec2(4, 3);
	style.FrameRounding = 0.0f;
	style.ItemSpacing = ImVec2(8, 4);
	style.ItemInnerSpacing = ImVec2(4, 4);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 3.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 0.0f;
	style.FrameBorderSize = 1.0f;
	style.ChildBorderSize = 1.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.DisplayWindowPadding = ImVec2(22, 22);
	style.DisplaySafeAreaPadding = ImVec2(4, 4);
	style.AntiAliasedLines = true;
	style.AntiAliasedFill = true;
	style.CurveTessellationTol = 1.25f;

	ColorVar mainColor = ImColor(54, 54, 54, 255);
	ColorVar bodyColor = ImColor(24, 24, 24, 240);
	ColorVar fontColor = ImColor(255, 255, 255, 255);

	ImVec4 mainColorHovered = ImVec4(mainColor.Color().Value.x + 0.1f, mainColor.Color().Value.y + 0.1f, mainColor.Color().Value.z + 0.1f, mainColor.Color().Value.w);
	ImVec4 mainColorActive = ImVec4(mainColor.Color().Value.x + 0.2f, mainColor.Color().Value.y + 0.2f, mainColor.Color().Value.z + 0.2f, mainColor.Color().Value.w);
	ImVec4 menubarColor = ImVec4(bodyColor.Color().Value.x, bodyColor.Color().Value.y, bodyColor.Color().Value.z, bodyColor.Color().Value.w - 0.8f);
	ImVec4 frameBgColor = ImVec4(bodyColor.Color().Value.x, bodyColor.Color().Value.y, bodyColor.Color().Value.z, bodyColor.Color().Value.w + .1f);
	ImVec4 tooltipBgColor = ImVec4(bodyColor.Color().Value.x, bodyColor.Color().Value.y, bodyColor.Color().Value.z, bodyColor.Color().Value.w + .05f);


	style.Colors[ImGuiCol_Text] = fontColor.Color();
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = bodyColor.Color();
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(.0f, .0f, .0f, .0f);
	style.Colors[ImGuiCol_PopupBg] = tooltipBgColor;
	style.Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 255);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = frameBgColor;
	style.Colors[ImGuiCol_FrameBgHovered] = mainColorHovered;
	style.Colors[ImGuiCol_FrameBgActive] = mainColorActive;
	style.Colors[ImGuiCol_TitleBg] = mainColor.Color();
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = mainColor.Color();
	style.Colors[ImGuiCol_MenuBarBg] = menubarColor;
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(frameBgColor.x + .05f, frameBgColor.y + .05f, frameBgColor.z + .05f, frameBgColor.w);
	style.Colors[ImGuiCol_ScrollbarGrab] = mainColor.Color();
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = mainColor.Color();
	style.Colors[ImGuiCol_ScrollbarGrabActive] = mainColor.Color();
	style.Colors[ImGuiCol_CheckMark] = mainColor.Color();
	style.Colors[ImGuiCol_SliderGrab] = mainColor.Color();
	style.Colors[ImGuiCol_SliderGrabActive] = mainColorActive;
	style.Colors[ImGuiCol_Button] = mainColor.Color();
	style.Colors[ImGuiCol_ButtonHovered] = mainColorHovered;
	style.Colors[ImGuiCol_ButtonActive] = mainColorActive;
	style.Colors[ImGuiCol_Header] = mainColor.Color();
	style.Colors[ImGuiCol_HeaderHovered] = mainColorHovered;
	style.Colors[ImGuiCol_HeaderActive] = mainColorActive;
	style.Colors[ImGuiCol_Column] = mainColor.Color();
	style.Colors[ImGuiCol_ColumnHovered] = mainColorHovered;
	style.Colors[ImGuiCol_ColumnActive] = mainColorActive;
	style.Colors[ImGuiCol_ResizeGrip] = ImColor(0, 0, 0, 0);
	style.Colors[ImGuiCol_ResizeGripHovered] = mainColorHovered;
	style.Colors[ImGuiCol_ResizeGripActive] = mainColorActive;
	style.Colors[ImGuiCol_PlotLines] = mainColor.Color();
	style.Colors[ImGuiCol_PlotLinesHovered] = mainColorHovered;
	style.Colors[ImGuiCol_PlotHistogram] = mainColor.Color();
	style.Colors[ImGuiCol_PlotHistogramHovered] = mainColorHovered;
	style.Colors[ImGuiCol_TextSelectedBg] = mainColorHovered;
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.0f, 0.0f, 0.0f, 0.75f);
	style.Colors[ImGuiCol_WindowBorder] = ImColor(0, 125, 255, 0);
	G::Init = true;
}


namespace gui
{
	void begin() {
		if (g_pEngine->IsInGame() && g_pEngine->IsConnected())
			ImGui::GetIO().MouseDrawCursor = G::Opened;
		else
			ImGui::GetIO().MouseDrawCursor = true;
		ImGui_ImplDX9_NewFrame();
	}

	void aimbot()
	{
		ImGui::Checkbox("Enable aim", &Clientvariables->Ragebot.EnableAimbot);
		if (Clientvariables->Ragebot.EnableAimbot)
		{
			if (ImGui::CollapsingHeader("Main"))
			{
				ImGui::Checkbox("Silent aim", &Clientvariables->Ragebot.SilentAimbot);
				ImGui::Checkbox("NoRecoil", &Clientvariables->Ragebot.NoRecoil);
				if (!Clientvariables->Misc.AntiUT) {
					ImGui::Checkbox("NoSpread", &Clientvariables->Ragebot.NoSpread);
				}
				ImGui::Checkbox("Auto Revolver", &Clientvariables->Ragebot.AutomaticRevolver);
				ImGui::Checkbox("Auto Fire", &Clientvariables->Ragebot.AutomaticFire);
				ImGui::Checkbox("Auto Scope", &Clientvariables->Ragebot.AutomaticScope);
				ImGui::Checkbox("Auto Penetration", &Clientvariables->Ragebot.Autowall);
				ImGui::SliderInt("Hitchance", &Clientvariables->Ragebot.Minhitchance, 0, 100, "%.0f%%");
				ImGui::SliderInt("Min damage", &Clientvariables->Ragebot.Mindamage, 0, 100, "%.0f%%");
			}
			if (ImGui::CollapsingHeader("Other"))
			{
				ImGui::Checkbox("Accuracy Boost", &Clientvariables->Ragebot.PositionAdjustment);
				ImGui::Checkbox("Resolver", &Clientvariables->Ragebot.AutomaticResolver);
				ImGui::Checkbox("Fake Ping", &Clientvariables->Ragebot.FakeLatency);
				ImGui::SliderFloat(" ", &Clientvariables->Ragebot.FakeLatencyAmount, 0, 1, "%.2f");

				ImGui::Combo("Target Selection", &Clientvariables->Ragebot.AimbotSelection, SelectionMode, ARRAYSIZE(SelectionMode));
				ImGui::Combo("Prefer Baim", &Clientvariables->Ragebot.Preferbodyaim, PreferBodyaim, ARRAYSIZE(PreferBodyaim));
			}
			if (ImGui::CollapsingHeader("Hitscan"))
			{
				ImGui::Combo("Hitbox", &Clientvariables->Ragebot.Hitbox, HitboxMode, ARRAYSIZE(HitboxMode));
				ImGui::Combo("Hitscan##typehitscanyifFFf", &Clientvariables->Ragebot.Hitscan, HitscanMode, ARRAYSIZE(HitscanMode));

				ImGui::Checkbox("Hitscan", &Clientvariables->Ragebot.AutowallHitscan);


				ImGui::Checkbox("HeadAim if Walking", &Clientvariables->Ragebot.headaimwalking);

				ImGui::SliderFloat("Head Scale", &Clientvariables->Ragebot.Headscale, 0.f, 100.f, "%.2f%%");
				ImGui::SliderFloat("Body Scale", &Clientvariables->Ragebot.Bodyscale, 0.f, 100.f, "%.2f%%");

				ImGui::Combo("Accuracy Boost##typehitscanyifFFf", &Clientvariables->Ragebot.pzj, pzj, ARRAYSIZE(pzj));

			}
		}
		if (ImGui::CollapsingHeader("Antiaim"))
		{
			ImGui::Checkbox("Active###_anti_aim", &Clientvariables->Antiaim.AntiaimEnable);
			//ImGui::Checkbox("Antiaim lines", &Clientvariables->Antiaim.Lines);
			ImGui::Checkbox("LBY indicator", &Clientvariables->Antiaim.Indicator);

			ImGui::Combo("Pitch", &Clientvariables->Antiaim.Pitch, AntiaimbotPitch, ARRAYSIZE(AntiaimbotPitch));

			if (Clientvariables->Misc.AntiUT) {
				ImGui::Combo("Yaw", &Clientvariables->Antiaim.Yaw, AntiaimbotYaw, ARRAYSIZE(AntiaimbotYaw));

				ImGui::Combo("Fake", &Clientvariables->Antiaim.FakeYaw, AntiaimbotYaw, ARRAYSIZE(AntiaimbotYaw));

				ImGui::Combo("Direction", &Clientvariables->Antiaim.breakaa, bbb, ARRAYSIZE(bbb));

			}

			ImGui::SliderInt("LBY Delta", &Clientvariables->Antiaim.FreestandingDelta, -180, 180);


			ImGui::Checkbox("Enable Fakelag", &Clientvariables->Misc.FakelagEnable);

			if (Clientvariables->Misc.FakelagEnable) {
				ImGui::SliderInt("Factor ##amount_fakelag", &Clientvariables->Misc.FakelagAmount, 1, 15);

				ImGui::Combo("Mode###fake_lag_mode", &Clientvariables->Misc.FakelagMode, FakelagMode, ARRAYSIZE(FakelagMode));
				ImGui::Checkbox("On Ground", &Clientvariables->Misc.FakelagOnground);
			}
		}

	}

	void visuals()
	{
		ImGui::Checkbox("Active###esp_active", &Clientvariables->Visuals.EspEnable);

		if (Clientvariables->Visuals.EspEnable) {

			if (ImGui::CollapsingHeader("Enemy")) {
				//	ImGui::Checkbox("Enemy only", &Clientvariables->Visuals.EnemyOnly);
				ImGui::Checkbox("Name", &Clientvariables->Visuals.Name);
				ImGui::Checkbox("Health", &Clientvariables->Visuals.Health);
				ImGui::Checkbox("Weapon", &Clientvariables->Visuals.Weapon);
				ImGui::Checkbox("Armor", &Clientvariables->Visuals.Armor);
				ImGui::Checkbox("Ammo", &Clientvariables->Visuals.Ammo);
				ImGui::Checkbox("Alpha", &Clientvariables->Visuals.setblend);
				//	ImGui::Checkbox("Override indicator", &Clientvariables->Visuals.Flags);
				ImGui::Checkbox("Bomb", &Clientvariables->Visuals.Bomb);
				ImGui::Checkbox("Show Grenades", &Clientvariables->Visuals.ThrownNades);
				ImGui::Checkbox("Radar", &Clientvariables->Visuals.Radar);
				ImGui::Checkbox("Bounding Box", &Clientvariables->Visuals.BoundingBox);
				ImGui::SameLine(216);
				ImGui::MyColorEdit3("##box_col", Clientvariables->Colors.BoundingBox, 1 << 7);

				ImGui::Checkbox("Skeleton", &Clientvariables->Visuals.Bones);
				ImGui::SameLine(216);
				ImGui::MyColorEdit3("###skeleton_col", Clientvariables->Colors.Skeletons, 1 << 7);

				ImGui::Checkbox("Glow", &Clientvariables->Visuals.Glow);
				ImGui::SameLine(216);
				ImGui::MyColorEdit4("###glow_col", Clientvariables->Colors.Glow, 1 << 7 | 1 << 0);

				ImGui::Checkbox("Damage Indicator", &Clientvariables->Visuals.DamageIndicators);
				ImGui::SameLine(216);
				ImGui::MyColorEdit3("##damageindicator", Clientvariables->Colors.DamageIndicator, 1 << 7);

				//	ImGui::NewLine();

			}
			if (ImGui::CollapsingHeader("Chams")) {
				ImGui::Checkbox("Enabled", &Clientvariables->Visuals.ChamsEnable);
				if (Clientvariables->Visuals.ChamsEnable) {
					ImGui::Combo("Type", &Clientvariables->Visuals.ChamsStyle, ModelsMode, ARRAYSIZE(ModelsMode));

					ImGui::Checkbox("Player", &Clientvariables->Visuals.ChamsPlayer);
					ImGui::SameLine(216);
					ImGui::MyColorEdit3("##chams", Clientvariables->Colors.PlayerChams, 1 << 7);
					ImGui::Checkbox("Player XQZ", &Clientvariables->Visuals.ChamsPlayerWall);
					ImGui::SameLine(216);
					ImGui::MyColorEdit3("##chamswall", Clientvariables->Colors.PlayerChamsWall, 1 << 7);

					ImGui::Checkbox("Local Player##ofodofod", &Clientvariables->Visuals.ChamsL);
					ImGui::SameLine(216);
					ImGui::MyColorEdit3("##tgrtfereaw", Clientvariables->Colors.PlayerChamsl, 1 << 7);

					ImGui::Checkbox("Show Backtrack", &Clientvariables->Visuals.ShowBacktrack);
					ImGui::SameLine(216);
					ImGui::MyColorEdit3("##ShowBacktrack", Clientvariables->Colors.ChamsHistory, 1 << 7);

				}
			}
			if (ImGui::CollapsingHeader("Other")) {
				//	ImGui::NewLine();

				if (ImGui::CollapsingHeader("Local")) {

					ImGui::Checkbox("Bullet Tracers", &Clientvariables->Visuals.BulletTracers);
					ImGui::SameLine(216);
					ImGui::MyColorEdit3("##tracers", Clientvariables->Colors.Bulletracer, 1 << 7);

					ImGui::Checkbox("Grenade trajectory", &Clientvariables->Visuals.GrenadePrediction);
					ImGui::SameLine(216);
					ImGui::MyColorEdit3("##nadetrajectory", Clientvariables->Colors.GrenadePrediction, 1 << 7);

					ImGui::Checkbox("Spread Crosshair", &Clientvariables->Visuals.SpreadCrosshair);
					ImGui::SameLine(216);
					ImGui::MyColorEdit4("##spreadccc", Clientvariables->Colors.SpreadCrosshair, 1 << 7 | 1 << 0);

					//	ImGui::NewLine();
					ImGui::Combo("ThirdPerson", &Clientvariables->Misc.TPKey, KeyStrings, ARRAYSIZE(KeyStrings));
					ImGui::Combo("##angles", &Clientvariables->Misc.TPangles, ThirdpersonAngles, ARRAYSIZE(ThirdpersonAngles));

				}
				if (ImGui::CollapsingHeader("Effects")) {
					ImGui::Checkbox("Hitmarker", &Clientvariables->Visuals.Hitmarker);
					ImGui::Checkbox("Nightmode", &Clientvariables->Visuals.nightmode);
					ImGui::Checkbox("Asus Props", &Clientvariables->Colors.Props);
					ImGui::Checkbox("NoSpread", &Clientvariables->Visuals.Nosmoke);
					ImGui::Checkbox("Remove Vis Recoil", &Clientvariables->Visuals.Novisrevoil);
					ImGui::Checkbox("Remove Scope", &Clientvariables->Visuals.Noscope);
					ImGui::Checkbox("Remove Zoom", &Clientvariables->Misc.static_scope);
					ImGui::Checkbox("Remove Post Processing", &Clientvariables->Visuals.RemoveParticles);
					ImGui::Checkbox("Crosshair on sniper", &Clientvariables->Visuals.crosssnip);
					ImGui::Combo("Skybox", &Clientvariables->Visuals.Skybox, Skyboxmode, ARRAYSIZE(Skyboxmode));
					ImGui::SliderInt("Flash Alpha", &Clientvariables->Visuals.FlashbangAlpha, 0, 255, "%.0f%%");
					ImGui::SliderInt("Player Viewmodel", &Clientvariables->Misc.PlayerViewmodel, 0, 180, "%.0f%%");
					ImGui::SliderInt("Player FOV", &Clientvariables->Misc.PlayerFOV, -50, 50, "%.0f%%");
				}
			}
		}
	}

	void misc()
	{
		ImGui::Checkbox("Anti-Untrusted", &Clientvariables->Misc.AntiUT);

		ImGui::Checkbox("Bhop", &Clientvariables->Misc.AutoJump);
		ImGui::Checkbox("AutoStrafe", &Clientvariables->Misc.AutoStrafe);
		ImGui::Combo("BuyBot Primary", &Clientvariables->Misc.buybot1, rifles, ARRAYSIZE(rifles));
		ImGui::Combo("BuyBot Secondary", &Clientvariables->Misc.buybot2, pistols, ARRAYSIZE(pistols));
		ImGui::Combo("BuyBot Other", &Clientvariables->Misc.buybot3, other, ARRAYSIZE(other));

		//ImGui::Checkbox("Clant tag exploit", &Clientvariables->Misc.Clantag);
		ImGui::Checkbox("Styles Hands", &Clientvariables->Misc.WireHand);
		//	ImGui::Checkbox("Spec list", &Clientvariables->Misc.Spectators);


		ImGui::SliderInt("Re-Track Speed", &Clientvariables->Misc.Retrack, 1, 8);
	}

	void cfg() {
		ImGui::Combo("##cfg", &Clientvariables->Misc.ConfigSelection, Configs, ARRAYSIZE(Configs));
		if (ImGui::Button("Save CFG", ImVec2(160, 20))) {
			//Eventlog::Msg("Config saved", false);
			ConSys->SaveConfig();
		}
		if (ImGui::Button("Load CFG", ImVec2(160, 20))) {
			//Eventlog::Msg("Config loaded", false);
			ConSys->LoadConfig();
		}
		if (ImGui::Button("Reset font", ImVec2(160, 20))) {
			g_Draw.Init();
		}
	}

	void render() {

		static int page = 0;

		if (G::Opened)
			G::ShowMenu = true;
		else
			G::ShowMenu = false;

		if (G::ShowMenu) {
			ImGui::SetNextWindowSize(ImVec2(600, 350), ImGuiCond_FirstUseEver);
			ImGui::Begin("Sourcecode $", &G::Opened, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar /*| ImGuiWindowFlags_NoTitleBar*/); {
				ImVec2 p = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x - 5, p.y - 8), ImVec2(p.x + ImGui::GetWindowWidth() / 2 - 5, p.y - 5), ImColor(55, 177, 218), ImColor(201, 84, 192), ImColor(201, 84, 192), ImColor(55, 177, 218));
				ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x + ImGui::GetWindowWidth() / 2 - 5, p.y - 8), ImVec2(p.x + ImGui::GetWindowWidth() - 5, p.y - 5), ImColor(201, 84, 192), ImColor(204, 227, 54), ImColor(204, 227, 54), ImColor(201, 84, 192));
				ImGui::BeginChild("tabs", ImVec2(0, 20), false);
				if (ImGui::Button("Aimbot", ImVec2(ImGui::GetWindowWidth() / 4 - 6, 20))) page = 0;
				ImGui::SameLine();
				if (ImGui::Button("Visuals", ImVec2(ImGui::GetWindowWidth() / 4 - 6, 20))) page = 1;
				ImGui::SameLine();
				if (ImGui::Button("Misc", ImVec2(ImGui::GetWindowWidth() / 4 - 6, 20))) page = 2;
				ImGui::SameLine();
				if (ImGui::Button("Config", ImVec2(ImGui::GetWindowWidth() / 4 - 6, 20))) page = 3;
				ImGui::EndChild();

				ImGui::BeginChild("main", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar);

				ImGui::PushItemWidth(175);
				switch (page) {
				case 0:
					aimbot();
					break;
				case 1:
					visuals();
					break;
				case 2:
					misc();
					break;
				case 3:
					cfg();
					break;
				}
				ImGui::PopItemWidth();

				ImGui::EndChild();
			}
			ImGui::End();
		}
	}

	void end() {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	void shutdown() {
		ImGui_ImplDX9_Shutdown();
	}

	void create_objects() {
		ImGui_ImplDX9_CreateDeviceObjects();
	}

	void destroy_objects() {
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}
}

HRESULT __stdcall Hooks::D3D9_EndScene(IDirect3DDevice9* pDevice)
{
	HRESULT result = d3d9VMT->GetOriginalMethod<EndSceneFn>(42)(pDevice);
	
		g_ESP->Crosshair(pDevice);


	gui::begin();
	gui::render();
	gui::end();

	return result;
}

HRESULT __stdcall Hooks::hkdReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresParam)
{
	if (!G::Init)
		return oResetScene(pDevice, pPresParam);

	gui::destroy_objects();

	auto hr = oResetScene(pDevice, pPresParam);
	if (hr >= 0) {
		gui::create_objects();
	}

	return hr;
}