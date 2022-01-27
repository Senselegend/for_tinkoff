#pragma once
#include "sdk.h"
#include <windows.h>

//namespace fov_get
//{
//	extern float GetFov(Vector& viewAngle, Vector& aimAngle);
//}
//class legit_bot
//{
//public:
//	auto Run(CBaseEntity* LocalPlayer) -> void;
//private:
//	auto FindTarget(CBaseEntity * pLocalPlayer, CUserCmd * pCmd) -> int;
//
//};

class trigger_bot
{
public:
	auto Run(CBaseEntity* pLocalEntity, CUserCmd *pCmd) -> void;
private:
	auto trigger_bot::CreateMove(CBaseEntity* pLocalEntity, CUserCmd *pCmd) -> void;
};


