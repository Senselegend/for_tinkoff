#include "hooks.h"
#include "Menu.h"
#include "global.h"
#include "MaterialHelper.h"
#include "xor.h"
#include "BacktrackingHelper.h"
template<class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}


#include "hooks.h"
#include "Menu.h"
#include "global.h"
#include "MaterialHelper.h"
#include "xor.h"
#include "Chams.h"

Chams::Chams()
{
	std::ofstream("csgo\\materials\\simple_regular.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
	std::ofstream("csgo\\materials\\simple_ignorez.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
	std::ofstream("csgo\\materials\\simple_flat.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
	std::ofstream("csgo\\materials\\simple_flat_ignorez.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";


	std::ofstream("csgo\\materials\\simple_ignorez_reflective.vmt") << R"#("VertexLitGeneric"
{

  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       "env_cubemap"
  "$normalmapalphaenvmapmask"  "1"
  "$envmapcontrast"             "1"
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

	std::ofstream("csgo\\materials\\simple_regular_reflective.vmt") << R"#("VertexLitGeneric"
{

  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       "env_cubemap"
  "$normalmapalphaenvmapmask"  "1"
  "$envmapcontrast"             "1"
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

	materialRegular = g_pMaterialSystem->FindMaterial("simple_regular", TEXTURE_GROUP_MODEL);
	materialRegularIgnoreZ = g_pMaterialSystem->FindMaterial("simple_ignorez", TEXTURE_GROUP_MODEL);
	materialFlatIgnoreZ = g_pMaterialSystem->FindMaterial("simple_flat_ignorez", TEXTURE_GROUP_MODEL);
	materialFlat = g_pMaterialSystem->FindMaterial("simple_flat", TEXTURE_GROUP_MODEL);
	materialMetall = g_pMaterialSystem->FindMaterial("simple_ignorez_reflective", TEXTURE_GROUP_MODEL);
	materialMetallnZ = g_pMaterialSystem->FindMaterial("simple_regular_reflective", TEXTURE_GROUP_MODEL);
}

Chams::~Chams()
{
	std::remove("csgo\\materials\\simple_regular.vmt");
	std::remove("csgo\\materials\\simple_ignorez.vmt");
	std::remove("csgo\\materials\\simple_flat.vmt");
	std::remove("csgo\\materials\\simple_flat_ignorez.vmt");
	std::remove("csgo\\materials\\simple_ignorez_reflective.vmt");
	std::remove("csgo\\materials\\simple_regular_reflective.vmt");
}

void Chams::OverrideMaterial(bool ignoreZ, bool flat, bool wireframe, bool glass, bool metallic, const Color& rgba)
{


	IMaterial* material = nullptr;

	if (flat) {
		if (ignoreZ)
			material = materialFlatIgnoreZ;
		else
			material = materialFlat;
	}
	else {
		if (ignoreZ)
			material = materialRegularIgnoreZ;
		else
			material = materialRegular;
	}


	if (glass) {
		material = materialFlat;
		material->AlphaModulate(0.45f);
	}
	else {
		material->AlphaModulate(
			rgba.a() / 255.0f);
	}

	if (metallic) {
		if (ignoreZ)
			material = materialMetall;
		else
			material = materialMetallnZ;

		material->AlphaModulate(0.7f);
	}
	else {
		g_pModelRender->ForcedMaterialOverride(material);
	}

	material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
	material->ColorModulate(
		rgba.r() / 255.0f,
		rgba.g() / 255.0f,
		rgba.b() / 255.0f);

	g_pModelRender->ForcedMaterialOverride(material);
}

void __fastcall Hooks::scene_end(void* thisptr, void* edx) {

	static auto scene_end_o = renderviewVMT->GetOriginalMethod< decltype(&scene_end) >(9);
	scene_end_o(thisptr, edx);



	if (Clientvariables->Visuals.ChamsEnable)
	{
		//float Red, Green, Blue, RedZ, GreenZ, BlueZ;


		std::ofstream("csgo\\materials\\simple_flat_ignorez.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

		static IMaterial*	materialFlatIgnoreZ = g_pMaterialSystem->FindMaterial("simple_flat_ignorez", TEXTURE_GROUP_MODEL);

		std::ofstream("csgo\\materials\\simple_ignorez_reflective.vmt") << R"#("VertexLitGeneric"
{

  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       "env_cubemap"
  "$normalmapalphaenvmapmask"  "1"
  "$envmapcontrast"             "1"
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

		std::ofstream("csgo\\materials\\simple_regular_reflective.vmt") << R"#("VertexLitGeneric"
{

  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       "env_cubemap"
  "$normalmapalphaenvmapmask"  "1"
  "$envmapcontrast"             "1"
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

		static IMaterial* CoveredLit = g_MaterialHelper->CreateMaterial(true);
		static IMaterial* OpenLit = g_MaterialHelper->CreateMaterial(false);
		static IMaterial* CoveredFlat = g_MaterialHelper->CreateMaterial(true, false);
		static IMaterial* OpenFlat = g_MaterialHelper->CreateMaterial(false, false);

		static IMaterial* backtrack = g_MaterialHelper->CreateMaterial(true, false);

		static IMaterial* materialMetall = g_pMaterialSystem->FindMaterial("simple_ignorez_reflective", TEXTURE_GROUP_MODEL);
		static IMaterial* materialMetallnZ = g_pMaterialSystem->FindMaterial("simple_regular_reflective", TEXTURE_GROUP_MODEL);

		IMaterial *covered;
		IMaterial *open;
		//	IMaterial *ebopen = g_pMaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_OTHER);

		switch (Clientvariables->Visuals.ChamsStyle)
		{
		case 0:
			covered = CoveredLit;
			open = OpenLit;
			break;
		case 1:
			covered = CoveredFlat;
			open = OpenFlat;
			break;
		case 2:
			covered = materialMetall;
			open = materialMetallnZ;
			break;

		}


		auto b_IsThirdPerson = *reinterpret_cast<bool*>(reinterpret_cast<DWORD>(g_pInput) + 0xA5);

		for (int i = 1; i < g_pEngine->GetMaxClients(); ++i) {
			CBaseEntity* ent = (CBaseEntity*)g_pEntitylist->GetClientEntity(i);

			if (ent == G::LocalPlayer && G::LocalPlayer != nullptr)
			{
				if (G::LocalPlayer->isAlive())
				{
					if (b_IsThirdPerson && Clientvariables->Visuals.ChamsL)
					{
						Vector orig = G::LocalPlayer->GetAbsOrigin();


						g_pRenderView->SetColorModulation(Clientvariables->Colors.PlayerChamsl);

						g_pModelRender->ForcedMaterialOverride(open);


						G::LocalPlayer->draw_model(0x1/*STUDIO_RENDER*/, 255);
						g_pModelRender->ForcedMaterialOverride(nullptr);
					}
				}
			}
			if (ent->IsValidRenderable() && Clientvariables->Visuals.ShowBacktrack)
			{
				Vector orig = ent->GetAbsOrigin();

				if (g_BacktrackHelper->PlayerRecord[i].records.size() > 0)
				{
					tick_record record = g_BacktrackHelper->PlayerRecord[i].records.at(0);

					g_pRenderView->SetColorModulation(Clientvariables->Colors.ChamsHistory);


					ent->SetAbsOrigin(record.m_vecAbsOrigin);
					g_pModelRender->ForcedMaterialOverride(covered);
					ent->draw_model(0x1, 255);
					g_pModelRender->ForcedMaterialOverride(nullptr);
					ent->SetAbsOrigin(orig);


				}
			}


			if (ent->IsValidRenderable() && Clientvariables->Visuals.ChamsPlayer)
			{
				//Vector orig = ent->GetAbsOrigin();
				if (Clientvariables->Visuals.ChamsPlayerWall)
				{
					g_pRenderView->SetColorModulation(Clientvariables->Colors.PlayerChamsWall);
					g_pModelRender->ForcedMaterialOverride(covered);
					ent->draw_model(0x1/*STUDIO_RENDER*/, 255);
					g_pModelRender->ForcedMaterialOverride(nullptr);
				}
				g_pRenderView->SetColorModulation(Clientvariables->Colors.PlayerChams);
				g_pModelRender->ForcedMaterialOverride(open);
				ent->draw_model(0x1/*STUDIO_RENDER*/, 255);
				g_pModelRender->ForcedMaterialOverride(nullptr);
				//ent->SetAbsOrigin(orig);
			}
		}
	}
}

void __fastcall Hooks::DrawModelExecute(void* ecx, void* edx, void* * ctx, void *state, const ModelRenderInfo_t &pInfo, VMatrix *pCustomBoneToWorld)
{

	if (!G::LocalPlayer)
	{
		modelrenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(ecx, ctx, state, pInfo, pCustomBoneToWorld);
		return;
	}


	auto ent = g_pEntitylist->GetClientEntity(pInfo.entity_index);

	const char* ModelName = g_pModelInfo->GetModelName((model_t*)pInfo.pModel);


	if (ent == G::LocalPlayer)
	{
		if (G::LocalPlayer->IsScoped())
		{
			g_pRenderView->SetBlend(0.5f);
			g_pModelRender->ForcedMaterialOverride(nullptr);
		}
		/*else
		g_pRenderView->SetBlend(0);*/
	}


	if (Clientvariables->Visuals.setblend)
	{
		if (ent == G::LocalPlayer)
		{

			g_pRenderView->SetBlend(0.5f);
			g_pModelRender->ForcedMaterialOverride(nullptr);

			/*else
			g_pRenderView->SetBlend(0);*/
		}
	}


	const auto mdl = pInfo.pModel;

	if (Clientvariables->Colors.Props) {
		if (strstr(ModelName, "models/props")) {
			g_pRenderView->SetBlend(0.5f);
		}
	}

	if (Clientvariables->Misc.WireHand) {
		if (strstr(ModelName, "arms")) {
			IMaterial* mat = g_MaterialHelper->CreateMaterial(true, false, true);
			mat->ColorModulate(0, 255, 0);
			g_pModelRender->ForcedMaterialOverride(mat);
		}
	}

	/*if (strstr(ModelName, "models/player") != nullptr && ent && G::LocalPlayer && ent->IsValidRenderable() && Clientvariables->Visuals.ShowBacktrack) {
	if (g_BacktrackHelper->PlayerRecord[ent->Index()].records.size() > 0)
	{
	tick_record record = g_BacktrackHelper->PlayerRecord[ent->Index()].records.at(0);
	/*	if (record.m_vecAbsOrigin == ent->GetAbsOrigin()) {
	modelrenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(ecx, ctx, state, pInfo, pCustomBoneToWorld);
	return;
	}
	modelrenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(ecx, ctx, state, pInfo, record.boneMatrix);


	}
	}*/


	modelrenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(ecx, ctx, state, pInfo, pCustomBoneToWorld);
	g_pModelRender->ForcedMaterialOverride(NULL);
}
