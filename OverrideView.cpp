#include "hooks.h"
#include "global.h"
#include "Menu.h"
#include "sdk.h"
#include "GrenadePrediction.h"

ClientVariables* Clientvariables = new ClientVariables();

void __fastcall Hooks::OverrideView(void* _this, void* _edx, CViewSetup* setup)
{
	if (g_pEngine->IsInGame() && g_pEngine->IsConnected())
	{
		if (G::LocalPlayer && G::LocalPlayer->GetHealth() > 0)
		{
			if (!G::LocalPlayer->IsScoped())
			{
				setup->fov = 90 + Clientvariables->Misc.PlayerFOV;
			}
			else
			{
				if (Clientvariables->Misc.static_scope)
					setup->fov = 90 + Clientvariables->Misc.PlayerFOV;
				else {

				}
			}
		}
	}
	grenade_prediction::instance().View(setup);

	clientmodeVMT->GetOriginalMethod<OverrideViewFn>(18)(_this, setup);
}

float __stdcall GGetViewModelFOV()
{
	float fov = g_pClientModeHook->GetMethod<oGetViewModelFOV>(35)();

	if (g_pEngine->IsConnected() && g_pEngine->IsInGame())
	{
		if (G::LocalPlayer)
		{
				fov += Clientvariables->Misc.PlayerViewmodel;
		}
	}
	return fov;
}