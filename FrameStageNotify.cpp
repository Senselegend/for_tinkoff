#include "hooks.h"
#include "global.h"
#include "Menu.h"
#include "BacktrackingHelper.h"
#include "xor.h"
#include <intrin.h>
#include "SpoofedConvar.h"
#include "Math.h"
#include "Skinchanger.h"
#include "GameUtils.h"
#include "Items.h"
#include "Resolver.h"
#include "Aimbot.h"
#define INVALID_EHANDLE_INDEX 0xFFFFFFFF

#define TICK_INTERVAL (g_pGlobals->interval_per_tick)
#define TIME_TO_TICKS(dt) ((int)(0.5f + (float)(dt) / TICK_INTERVAL))
#define TICKS_TO_TIME(t) (TICK_INTERVAL * (t))

ConVar* mp_facefronttime;
ConVar* r_DrawSpecificStaticProp;

int get_model_index(int item_def_index)
{
	int ret = 0;
	switch (item_def_index)
	{
	case KNIFE:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
		break;
	case KNIFE_T:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");
		break;
	case KNIFE_KARAMBIT:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
		break;
	case KNIFE_BAYONET:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
		break;
	case KNIFE_M9_BAYONET:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
		break;
	case KNIFE_TACTICAL:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
		break;
	case KNIFE_GUT:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
		break;
	case KNIFE_FALCHION:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
		break;
	case KNIFE_PUSH:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
		break;
	case KNIFE_BUTTERFLY:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
		break;
	case KNIFE_FLIP:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
		break;
	case KNIFE_BOWIE:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
		break;
	default:
		break;
	}
	return ret;
}
struct ResolverData
{
	Vector last_lby, inverse, inverse_right, inverse_left, lby, back, left, right, backtrack;
	float stored_simtime, last_move_time, pre_anim_lby;
	int last_ammo;
	bool breaking_lby, reset_state, could_be_faking;
	std::vector<float> unresolved_yaw, lby_deltas;
	static bool goodresolve;
	float lastlbystand;
	int unres;
	bool lby_changed;
	bool could_be_slowmo;
	bool is_moving;
	bool is_jumping;
	bool is_crouching;
	bool lby_updated;
	bool using_fake_angles;
	float last_moving_lby;
	float stored_lby;
	float next_lby_update_time;
	float next_lby_update;
	float next_lby_update_to;	
	float lby_timer;
	float lby_proxy;
    float lastlby;
	float flick;
	int stored_missed;
	float simtime, flcycle[13], flprevcycle[13], flweight[13], flweightdatarate[13], fakewalkdetection[2], fakeanglesimtimedetection[2], fakewalkdetectionsimtime[2];
	float yaw, addyaw, lbycurtime;
	float shotsimtime, oldlby, lastmovinglby, balanceadjustsimtime, balanceadjustflcycle;
	int fakeanglesimtickdetectionaverage[4], amountgreaterthan2, amountequal1or2, amountequal0or1, amountequal1, amountequal0, resetmovetick, resetmovetick2;
	int tick, balanceadjusttick, missedshots, activity[13];
	bool bfakeangle, bfakewalk, playerhurtcalled, weaponfirecalled;
	int shotaimangles;
	uint32_t norder[13];
	char* resolvermode = "NONE", *fakewalk = "Not Moving";
} pResolverData[64];;
bool isPartOf(char *a, char *b) {
	if (std::strstr(b, a) != NULL) {    //Strstr says does b contain a
		return true;
	}
	return false;
}
std::vector<const char*> smoke_materials = {
	"particle/beam_smoke_01",
	"particle/particle_smokegrenade",
	"particle/particle_smokegrenade1",
	"particle/particle_smokegrenade2",
	"particle/particle_smokegrenade3",
	"particle/particle_smokegrenade_sc",
	"particle/smoke1/smoke1",
	"particle/smoke1/smoke1_ash",
	"particle/smoke1/smoke1_nearcull",
	"particle/smoke1/smoke1_nearcull2",
	"particle/smoke1/smoke1_snow",
	"particle/smokesprites_0001",
	"particle/smokestack",
	"particle/vistasmokev1/vistasmokev1",
	"particle/vistasmokev1/vistasmokev1_emods",
	"particle/vistasmokev1/vistasmokev1_emods_impactdust",
	"particle/vistasmokev1/vistasmokev1_fire",
	"particle/vistasmokev1/vistasmokev1_nearcull",
	"particle/vistasmokev1/vistasmokev1_nearcull_fog",
	"particle/vistasmokev1/vistasmokev1_nearcull_nodepth",
	"particle/vistasmokev1/vistasmokev1_smokegrenade",
	"particle/vistasmokev1/vistasmokev4_emods_nocull",
	"particle/vistasmokev1/vistasmokev4_nearcull",
	"particle/vistasmokev1/vistasmokev4_nocull"
};
void DrawBeam(Vector src, Vector end, Color color)
{
	BeamInfo_t beamInfo;
	beamInfo.m_nType = TE_BEAMPOINTS;
	beamInfo.m_pszModelName = "sprites/purplelaser1.vmt";
	beamInfo.m_nModelIndex = -1;
	beamInfo.m_flHaloScale = 0.0f;
	beamInfo.m_flLife = 3.0f;
	beamInfo.m_flWidth = 2.0f;
	beamInfo.m_flEndWidth = 2.0f;
	beamInfo.m_flFadeLength = 0.0f;
	beamInfo.m_flAmplitude = 2.0f;
	beamInfo.m_flBrightness = 255.f;
	beamInfo.m_flSpeed = 0.2f;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 0.f;
	beamInfo.m_flRed = color.r();
	beamInfo.m_flGreen = color.g();
	beamInfo.m_flBlue = color.b();
	beamInfo.m_nSegments = 2;
	beamInfo.m_bRenderable = true;
	beamInfo.m_vecStart = src;
	beamInfo.m_vecEnd = end;
	Beam_t* myBeam = g_pViewRenderBeams->CreateBeamPoints(beamInfo);
	if (myBeam)
		g_pViewRenderBeams->DrawBeam(myBeam);
}
bool IsYawSideways(CBaseEntity* entity, float yaw)
{
	auto local_player = G::LocalPlayer;
	if (!local_player)
		return false;

	const auto at_target_yaw = GameUtils::CalculateAngle(local_player->GetOrigin(), entity->GetOrigin()).y;
	const float delta = fabs(Math::NormalizeYaw(at_target_yaw - yaw));

	return delta > 20.f && delta < 160.f;
}

void __stdcall Hooks::FrameStageNotify(ClientFrameStage_t curStage)
{
	if (curStage == FRAME_RENDER_START)
	{
		for (int i = 1; i < g_pGlobals->maxClients; i++)
		{
			CBaseEntity* pEntity = g_pEntitylist->GetClientEntity(i);
			if (pEntity != nullptr)
			{
				CBasePlayerAnimState* Animationss = pEntity->GetBasePlayerAnimState(); // DONT ASK ABOUT IT LEL
				if (Animationss->m_iLastClientSideAnimationUpdateFramecount)
					Animationss->m_iLastClientSideAnimationUpdateFramecount--;
				if (Animationss->m_flLastClientSideAnimationUpdateTime)
					Animationss->m_flLastClientSideAnimationUpdateTime -= g_pGlobals->interval_per_tick;
			}
		}
	}
	static std::string old_Skyname = "";
	static bool OldNightmode;
	static int OldSky;
	if (!G::LocalPlayer || !g_pEngine->IsConnected() || !g_pEngine->IsInGame())
	{
		clientVMT->GetOriginalMethod<FrameStageNotifyFn>(36)(curStage);
		old_Skyname = "";
		OldNightmode = false;
		OldSky = 0;
		return;
	}
	if (OldNightmode != Clientvariables->Visuals.nightmode)
	{
		if (!r_DrawSpecificStaticProp)
			r_DrawSpecificStaticProp = g_pCvar->FindVar("r_DrawSpecificStaticProp");
		r_DrawSpecificStaticProp->SetValue(0);
		for (MaterialHandle_t i = g_pMaterialSystem->FirstMaterial(); i != g_pMaterialSystem->InvalidMaterial(); i = g_pMaterialSystem->NextMaterial(i))
		{
			IMaterial* pMaterial = g_pMaterialSystem->GetMaterial(i);
		   if (!pMaterial)
				continue;
			if (strstr(pMaterial->GetTextureGroupName(), "World") || strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
			{
				if (Clientvariables->Visuals.nightmode)
					if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
						pMaterial->ColorModulate(0.3f, 0.3f, 0.3f);
					else
						pMaterial->ColorModulate(0.05f, 0.05f, 0.05f);
				else
					pMaterial->ColorModulate(1.0f, 1.0f, 1.0f);
			}
		}
		OldNightmode = Clientvariables->Visuals.nightmode;
	}
	if (OldSky != Clientvariables->Visuals.Skybox)
	{
		auto LoadNamedSky = reinterpret_cast<void(__fastcall*)(const char*)>(FindPatternIDA("engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"));
		if (old_Skyname == "")
		{
			//old_Skyname = g_pCvar->FindVar("sv_skyname")->GetName();
		}
		int type = Clientvariables->Visuals.Skybox;
		/*if(type == 0)
		LoadNamedSky(old_Skyname.c_str());
		else */if (type == 1)
			LoadNamedSky("cs_baggage_skybox_");
		else if (type == 2)
			LoadNamedSky("cs_tibet");
		else if (type == 3)
			LoadNamedSky("italy");
		else if (type == 4)
			LoadNamedSky("jungle");
		else if (type == 5)
			LoadNamedSky("office");
		else if (type == 6)
			LoadNamedSky("sky_cs15_daylight02_hdr");
		else if (type == 7)
			LoadNamedSky("sky_csgo_night02");
		else if (type == 8)
			LoadNamedSky("vertigo");
		OldSky = Clientvariables->Visuals.Skybox;
	}
	static Vector oldViewPunch;
	static Vector oldAimPunch;
	Vector* view_punch = G::LocalPlayer->GetViewPunchPtr();
	Vector* aim_punch = G::LocalPlayer->GetPunchAnglePtr();
	if (curStage == FRAME_RENDER_START && G::LocalPlayer->isAlive())
	{
		static bool enabledtp = false, check = false;

		if (GetAsyncKeyState(Clientvariables->Misc.TPKey) && G::LocalPlayer->isAlive())
		{
			if (!check)
			enabledtp = !enabledtp;
			check = true;
		}
		else
			check = false;
		CBasePlayerAnimState* Animations = G::LocalPlayer->GetBasePlayerAnimState();
		if (enabledtp)
		{
			if (Clientvariables->Misc.TPangles == 0)
			{
				*reinterpret_cast<QAngle*>(reinterpret_cast<DWORD>(G::LocalPlayer + 0x31C0 + 0x8)) = G::FakeAngle;
			}
			else if (Clientvariables->Misc.TPangles == 1)
			{
				*reinterpret_cast<QAngle*>(reinterpret_cast<DWORD>(G::LocalPlayer + 0x31C0 + 0x8)) = G::RealAngle;
			}
			/*	else if (Clientvariables->Misc.TPangles == 2 && Animations->m_bInHitGroundAnimation && !GetAsyncKeyState(0x20))
			{
			*reinterpret_cast<QAngle*>(reinterpret_cast<DWORD>(G::LocalPlayer + 0x31C0 + 0x8)) = QAngle(-1.0f, G::LocalPlayer->LowerBodyYaw(), G::UserCmd->viewangles.z);
			}*/
			else if (Clientvariables->Misc.TPangles == 2)
			{
				*reinterpret_cast<QAngle*>(reinterpret_cast<DWORD>(G::LocalPlayer + 0x31C0 + 0x8)) = QAngle(G::UserCmd->viewangles.x, G::LocalPlayer->LowerBodyYaw(), G::UserCmd->viewangles.z);
			}
		}
		if (view_punch && aim_punch && Clientvariables->Visuals.Novisrevoil)
		{
			oldViewPunch = *view_punch;
			oldAimPunch = *aim_punch;
			view_punch->Init();
			aim_punch->Init();
		}
		if (enabledtp && G::LocalPlayer->isAlive())
		{
			*(bool*)((DWORD)g_pInput + 0xA5) = true;
			*(float*)((DWORD)g_pInput + 0xA8 + 0x8) = 130; // Thirdperson Distance
		}
		else
		{
			*(bool*)((DWORD)g_pInput + 0xA5) = false;
			*(float*)((DWORD)g_pInput + 0xA8 + 0x8);
		}
	}
	if (curStage == FRAME_NET_UPDATE_START)
	{
		if (Clientvariables->Visuals.BulletTracers)
		{
			float Red, Green, Blue;
			Red = Clientvariables->Colors.Bulletracer[0] * 255;
			Green = Clientvariables->Colors.Bulletracer[1] * 255;
			Blue = Clientvariables->Colors.Bulletracer[2] * 255;
			for (unsigned int i = 0; i < trace_logs.size(); i++) {
				auto *shooter = g_pEntitylist->GetClientEntity(g_pEngine->GetPlayerForUserID(trace_logs[i].userid));
				if (!shooter) return;
				Color color;
				if (shooter->GetTeamNum() == 3)
					color = Color(Red, Green, Blue, 210);
				else
					color = Color(Red, Green, Blue, 210);
				DrawBeam(trace_logs[i].start, trace_logs[i].position, color);
				trace_logs.erase(trace_logs.begin() + i);
			}
		}
		for (auto material_name : smoke_materials) {
			IMaterial* mat = g_pMaterialSystem->FindMaterial(material_name, TEXTURE_GROUP_OTHER);
			mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Clientvariables->Visuals.Nosmoke ? true : false);
		}
		if (Clientvariables->Visuals.Nosmoke) {
			static int* smokecount = *(int**)(FindPatternIDA("client.dll", "A3 ? ? ? ? 57 8B CB") + 0x1);
			*smokecount = 0;
		}
		for (int i = 1; i < g_pGlobals->maxClients; i++)
		{
			CBaseEntity* pEntity = g_pEntitylist->GetClientEntity(i);
			if (pEntity)
			{
				if (pEntity->GetHealth() > 0)
				{
					if (i != g_pEngine->GetLocalPlayer())
					{
						VarMapping_t* map = pEntity->GetVarMap();
						if (map)
						{
							if (Clientvariables->Ragebot.PositionAdjustment)
							{
								map->m_nInterpolatedEntries = 0;
							}
							else
							{
								if (map->m_nInterpolatedEntries == 0)
									map->m_nInterpolatedEntries = 6;
							}
						}

					}
				}
			}
		}
	}
	if (curStage == FRAME_RENDER_START)
	{
		for (int i = 1; i < g_pGlobals->maxClients; i++)
		{
			if (i == g_pEngine->GetLocalPlayer())
				continue;
			CBaseEntity* pEntity = g_pEntitylist->GetClientEntity(i);
			if (pEntity)
			{
				if (pEntity->GetHealth() > 0 && !pEntity->IsDormant())
				{
					*(int*)((uintptr_t)pEntity + 0xA30) = g_pGlobals->framecount; //we'll skip occlusion checks now
					*(int*)((uintptr_t)pEntity + 0xA28) = 0;//clear occlusion flags
				}
			}
		}
	}
	if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START && G::LocalPlayer->isAlive())
	{
		for (int i = 1; i < g_pGlobals->maxClients; i++)
		{
			if (i == g_pEngine->GetLocalPlayer())
				continue;
			CBaseEntity* pEntity = g_pEntitylist->GetClientEntity(i);
			if (pEntity && pEntity->GetHealth() > 0)
			{
				if (pEntity->IsDormant())
					continue;
				if (pEntity->GetTeamNum() == G::LocalPlayer->GetTeamNum())
					continue;
				if (Clientvariables->Ragebot.AutomaticResolver)
				{
					if (G::UserCmd->buttons & IN_ATTACK && g_Aimbot->fired_in_that_tick) {
						pResolverData[pEntity->GetIndex()].shotaimangles++;
					}
					if (pResolverData[pEntity->GetIndex()].shotaimangles > 5 || !G::LocalPlayer->isAlive()) {
						pResolverData[pEntity->GetIndex()].shotaimangles = 0;
					}
					Vector* pAngles = pEntity->GetEyeAnglesPtr();
					tick_record rec = g_BacktrackHelper->PlayerRecord[i].records.back();
				    if (pEntity->GetVelocity().Length2D() > 40 && *pEntity->GetFlags() & FL_ONGROUND)
					{
						pResolverData[pEntity->GetIndex()].lastmovinglby = pEntity->LowerBodyYaw();
					    pAngles->y = pEntity->LowerBodyYaw();
					}
					else if (pEntity->GetVelocity().Length2D() < 40 && IsYawSideways(pEntity, pResolverData[pEntity->GetIndex()].lastmovinglby))
					{
						pAngles->y = pResolverData[pEntity->GetIndex()].lastmovinglby;
					}
					else
					{
					  switch (pResolverData[pEntity->GetIndex()].shotaimangles % 3)
					  {
					  case 0:	pAngles->y = pEntity->LowerBodyYaw(); break;
					  case 1:	pAngles->y = pResolverData[pEntity->GetIndex()].lastmovinglby; break;
					  case 3:	pAngles->y = pResolverData[pEntity->GetIndex()].lastmovinglby + 160; break;
					  }
   					}
					for (int j = g_BacktrackHelper->PlayerRecord[pEntity->Index()].records.size() - 1; j >= 0; j--)
					{
					tick_record rec = g_BacktrackHelper->PlayerRecord[i].records.back();
					if (rec.bLowerBodyYawUpdated)
					{
						pAngles->y = pEntity->LowerBodyYaw();
						pResolverData[pEntity->GetIndex()].lby_timer = pEntity->GetSimulationTime() + 1.1f;
					}
					else if (pEntity->GetSimulationTime() >= pResolverData[pEntity->GetIndex()].lby_timer)
					{
						pAngles->y = pEntity->LowerBodyYaw();
						pResolverData[pEntity->GetIndex()].lby_timer = pEntity->GetSimulationTime() + 1.1f;
					}
					}
					if (GetAsyncKeyState(0x56))
					{
						Vector pos_enemy;
						Vector local_target_angle;
						if (GameUtils::WorldToScreen(pEntity->GetOrigin(), pos_enemy)) {
							GameUtils::calculate_angle(G::LocalPlayer->GetOrigin(), pEntity->GetOrigin(), local_target_angle);
							ImVec2 ImGui::GetMousePos();
							float delta = ImGui::GetMousePos().x - pos_enemy.x;
							if (delta < 0) pEntity->GetEyeAnglesPtr()->y = local_target_angle.y + 90;
							else pEntity->GetEyeAnglesPtr()->y = local_target_angle.y - 90;
						}
					}
					}
					if (!Clientvariables->Misc.AntiUT) {
						std::string strPitch = std::to_string(pEntity->GetEyeAnglesPtr()->x);
						if (pEntity->GetEyeAnglesPtr()->x < -179.f)
							pEntity->GetEyeAnglesPtr()->x += 360.f;
						else if (pEntity->GetEyeAnglesPtr()->x > 90.0 || pEntity->GetEyeAnglesPtr()->x < -90.0)
							pEntity->GetEyeAnglesPtr()->x = 89.f;
						else if (pEntity->GetEyeAnglesPtr()->x > 89.0 && pEntity->GetEyeAnglesPtr()->x < 91.0)
							pEntity->GetEyeAnglesPtr()->x -= 90.f;
						else if (pEntity->GetEyeAnglesPtr()->x > 179.0 && pEntity->GetEyeAnglesPtr()->x < 181.0)
							pEntity->GetEyeAnglesPtr()->x -= 180;
						else if (pEntity->GetEyeAnglesPtr()->x > -179.0 && pEntity->GetEyeAnglesPtr()->x < -181.0)
							pEntity->GetEyeAnglesPtr()->x += 180;
						else if (fabs(pEntity->GetEyeAnglesPtr()->x) == 0)
							pEntity->GetEyeAnglesPtr()->x = std::copysign(89.0f, pEntity->GetEyeAnglesPtr()->x);
					}
				}
			else
			{
				g_BacktrackHelper->PlayerRecord[i].records.clear();
			}
		}
	}
	if (curStage == FRAME_RENDER_START)
	{
		if (G::LocalPlayer->GetHealth() > 0) {
			CBasePlayerAnimState* Animations = G::LocalPlayer->GetBasePlayerAnimState();
			if (!Animations->m_bInHitGroundAnimation)
				g_BacktrackHelper->AnimationFix(curStage);
		}
		for (int i = 1; i < g_pGlobals->maxClients; i++)
		{
			CBaseEntity* pEntity = g_pEntitylist->GetClientEntity(i);
			if (pEntity)
			{
				if (pEntity->GetHealth() > 0 && !pEntity->IsDormant())
				{
					g_BacktrackHelper->UpdateBacktrackRecords(pEntity);
					g_BacktrackHelper->UpdateExtrapolationRecords(pEntity);

				}
			}
		}
	}
	clientVMT->GetOriginalMethod<FrameStageNotifyFn>(36)(curStage);
	if (curStage == FRAME_RENDER_START && G::LocalPlayer && G::LocalPlayer->GetHealth() > 0)
	{
		*(Vector*)((DWORD)G::LocalPlayer + 0x31C8) = G::FakeAngle;
		if (Clientvariables->Visuals.Novisrevoil)
		{
			*aim_punch = oldAimPunch;
			*view_punch = oldViewPunch;
		}
	}
}


