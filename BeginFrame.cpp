#include "hooks.h"
#include "sdk.h"
#include "bullet_log.h"
#include "global.h"


void __fastcall Hooks::g_hkBeginFrame(void* thisptr)
{
	//if (!G::LocalPlayer) {
	//	beginFrame->GetOriginalMethod<BeginFrameFn>(9)(thisptr);
	//	return;
	//}

	beginFrame->GetOriginalMethod<BeginFrameFn>(9)(thisptr);

	if (Clientvariables->Visuals.BulletTracers)
	{
			for (size_t i = 0; i < logs.size(); i++)
			{
				auto current = logs.at(i);
				g_pDebugOverlay->AddLineOverlay(current.src, current.dst, current.color.r(), current.color.g(), current.color.b(), true, -1.f);
				g_pDebugOverlay->AddBoxOverlay(current.dst, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 255, 0, 0, 127, -1.f);
				if (fabs(g_pGlobals->curtime - current.time) > 3.f)
					logs.erase(logs.begin() + i);
			}

		}
}