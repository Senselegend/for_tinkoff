#include "sdk.h"
#include "Aimbot.h"
#include "global.h"
#include "Menu.h"
#include "Math.h"
#include "GameUtils.h"
#include "Autowall.h"
#include "BacktrackingHelper.h"
#include "NoSpread.h"
#define TICK_INTERVAL			( g_pGlobals->interval_per_tick )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )
template<class T, class U>
inline T clamp( T in, U low, U high )
{
    if( in <= low )
        return low;
    else if( in >= high )
        return high;
    else
        return in;
}

bool CAimbot::HitChance(CBaseEntity* pCSTarget, CBaseCombatWeapon* pCSWeapon, QAngle qAngle, int chance)
{
	if (!chance)
		return true;
	int iHit = 0;
	int iHitsNeed = (int)((float)256.f * ((float)chance / 100.f));
	bool bHitchance = false;
	Vector forward, right, up;
	Math::AngleVectors(qAngle, &forward, &right, &up);
	if ((pCSWeapon->WeaponID() != ItemDefinitionIndex::TASER))
	{
		float hitchance = 90;
		float inaccuracy = pCSWeapon->GetInaccuracy();
		if (inaccuracy == 0) inaccuracy = 0.0000001;
		inaccuracy = 1 / inaccuracy;
		hitchance = inaccuracy;
		return hitchance >= Clientvariables->Ragebot.Minhitchance;
	}
	else
	{
		pCSWeapon->UpdateAccuracyPenalty();
		for (auto i = 0; i < 256; ++i) {
			float RandomA = Math::RandomFloat2(0.0f, 1.0f);
			float RandomB = 1.0f - RandomA * RandomA;
			RandomA = Math::RandomFloat2(0.0f, M_PI_F * 2.0f);
			RandomB *= pCSWeapon->GetSpread() + pCSWeapon->GetInaccuracy();
			float SpreadX1 = (cos(RandomA) * RandomB);
			float SpreadY1 = (sin(RandomA) * RandomB);
			float RandomC = Math::RandomFloat2(0.0f, 1.0f);
			float RandomF = RandomF = 1.0f - RandomC * RandomC;
			RandomC = Math::RandomFloat2(0.0f, M_PI_F * 2.0f);
			RandomF *= pCSWeapon->GetSpread();
			float SpreadX2 = (cos(RandomC) * RandomF);
			float SpreadY2 = (sin(RandomC) * RandomF);
			float fSpreadX = SpreadX1 + SpreadX2;
			float fSpreadY = SpreadY1 + SpreadY2;
			Vector vSpreadForward;
			vSpreadForward[0] = forward[0] + (fSpreadX * right[0]) + (fSpreadY * up[0]);
			vSpreadForward[1] = forward[1] + (fSpreadX * right[1]) + (fSpreadY * up[1]);
			vSpreadForward[2] = forward[2] + (fSpreadX * right[2]) + (fSpreadY * up[2]);
			vSpreadForward.NormalizeInPlace();
			QAngle qaNewAngle;
			Math::VectorAngles(vSpreadForward, qaNewAngle);
			qaNewAngle.NormalizeVec();
			QAngle vEnd;
			Math::AngleVectors(qaNewAngle, &vEnd);
			vEnd = G::LocalPlayer->GetEyePosition() + (vEnd * 8192.f);
			if (G::LocalPlayer->canHit(vEnd, pCSTarget))
				iHit++;
			if ((int)(((float)iHit / 256.f) * 100.f) >= chance) {
				bHitchance = true;
				break;
			}
			if ((256.f - 1 - i + iHit) < iHitsNeed)
				break;
		}
		return bHitchance;
	}
}
std::vector<Vector> MainHitbox(CBaseEntity* pTarget, VMatrix BoneMatrix[128])
{
	bool lbyup;
	std::vector<Vector> Points;
	int Aimspot = Clientvariables->Ragebot.Hitbox;
	if (pTarget->GetVelocity().Length2D() > 40 && *pTarget->GetFlags() & FL_ONGROUND && Clientvariables->Ragebot.headaimwalking)
	{
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 0, BoneMatrix);
	}
	float PlayerVel = abs(pTarget->GetVelocity().Length2D());
	static float next_lby_update12[65];
	static float last_lby12[65];
	for (int j = g_BacktrackHelper->PlayerRecord[pTarget->Index()].records.size() - 1; j >= 0; j--)
	{
		tick_record rec = g_BacktrackHelper->PlayerRecord[pTarget->Index()].records.at(j);
		if (rec.bLowerBodyYawUpdated)
		{
			lbyup = true;
			next_lby_update12[pTarget->Index()] = pTarget->GetSimulationTime() + 1.1;
		}
		else if (pTarget->GetSimulationTime() >= next_lby_update12[pTarget->Index()])
		{
			lbyup = true;
			next_lby_update12[pTarget->Index()] = pTarget->GetSimulationTime() + 1.1;
		}
	}
	if (lbyup)
	{
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 0, BoneMatrix);
	}
	switch (Aimspot) {
	case 0:
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 0, BoneMatrix);
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 6, BoneMatrix);
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 3, BoneMatrix);//pelvis
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 4, BoneMatrix);//stomach
		break;
	case 1:
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 0, BoneMatrix);
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 6, BoneMatrix);
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 3, BoneMatrix);//pelvis
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 4, BoneMatrix);//stomach
		break;
	case 2:
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 0, BoneMatrix);
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 6, BoneMatrix);
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 3, BoneMatrix);//pelvis
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 4, BoneMatrix);//stomach
		break;
	case 3:
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 0, BoneMatrix);
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 6, BoneMatrix);
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 3, BoneMatrix);//pelvis
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 4, BoneMatrix);//stomach
		break;
	case 4:
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 0, BoneMatrix);
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 6, BoneMatrix);
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 3, BoneMatrix);//pelvis
		Points = GameUtils::GetMultiplePointsForHitbox(pTarget, 4, BoneMatrix);//stomach
		break;
	}
	return Points;
}

std::vector<int> GetHitboxesToScan(CBaseEntity* pTarget)
{
	std::vector<int> HitBoxesToScan;
	int HitScanMode = Clientvariables->Ragebot.Hitscan;
	int Aimspot = Clientvariables->Ragebot.AimbotSelection;
	float PlayerVel = abs(pTarget->GetVelocity().Length2D());
	static float next_lby_update[65];
	static float last_lby[65];
	bool lbyup;
	for (int j = g_BacktrackHelper->PlayerRecord[pTarget->Index()].records.size() - 1; j >= 0; j--)
	{
		tick_record rec = g_BacktrackHelper->PlayerRecord[pTarget->Index()].records.back();
		if (rec.bLowerBodyYawUpdated)
		{
			lbyup = true;
			next_lby_update[pTarget->Index()] = pTarget->GetSimulationTime() + 1.1f;
		}
		else if (pTarget->GetSimulationTime() >= next_lby_update[pTarget->Index()])
		{
			lbyup = true;
			next_lby_update[pTarget->Index()] = pTarget->GetSimulationTime() + 1.1f;
		}
	}
	if (lbyup)
	{
		HitBoxesToScan.push_back((int)CSGOHitboxID::HEAD);
	}
	if (pTarget->GetVelocity().Length2D() > 40 && *pTarget->GetFlags() & FL_ONGROUND && Clientvariables->Ragebot.headaimwalking)
	{
		HitBoxesToScan.push_back((int)CSGOHitboxID::HEAD);
		HitBoxesToScan.push_back((int)CSGOHitboxID::BELLY);
		HitBoxesToScan.push_back((int)CSGOHitboxID::THORAX);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LEFT_THIGH);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RIGHT_THIGH);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LEFT_FOOT);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RIGHT_FOOT);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LEFT_CALF);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RIGHT_CALF);
	}
	else
	{
		HitBoxesToScan.push_back((int)CSGOHitboxID::BELLY);
		HitBoxesToScan.push_back((int)CSGOHitboxID::THORAX);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LEFT_THIGH);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RIGHT_THIGH);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LEFT_FOOT);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RIGHT_FOOT);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LEFT_CALF);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RIGHT_CALF);
	}
	return HitBoxesToScan;
}
Vector RunAimScan(CBaseEntity* pTarget, float &simtime, Vector& origin)
{
	Vector vEyePos = G::LocalPlayer->GetEyePosition();
	auto wpn_data = G::WeaponData;
	static float minimum_damage = 1.f;
	if (Clientvariables->Ragebot.Mindamage == 0) // Enemy HP mode
		minimum_damage = pTarget->GetHealth();
	else
		minimum_damage = Clientvariables->Ragebot.Mindamage;

	VMatrix BoneMatrix[128];

	auto index = pTarget->GetIndex();
	if (Clientvariables->Ragebot.PositionAdjustment)
	{
		std::vector<tick_record> lby_records, trash_records;
		float latest_time = 0.0f;
		tick_record record;
		for (int j = g_BacktrackHelper->PlayerRecord[pTarget->Index()].records.size() - 1; j >= 0; j--) //works pretty good for nospread
		{
			record = g_BacktrackHelper->PlayerRecord[index].records.at(0);
		}
		if (g_BacktrackHelper->IsTickValid(record))
		{
			simtime = record.m_flSimulationTime;
			int hitgroup = -1;
			Vector original;
			original = pTarget->GetAbsOrigin();
			for (auto HitBox : MainHitbox(pTarget, record.boneMatrix)) {
				static float damage_given = 0.f;
				pTarget->SetAbsOrigin(record.m_vecOrigin);
				*reinterpret_cast<int*>(uintptr_t(pTarget) + 0xA30) = g_pGlobals->framecount;
				*reinterpret_cast<int*>(uintptr_t(pTarget) + 0xA28) = 0;
				pTarget->InvalidateBoneCache();
				if (Clientvariables->Ragebot.Autowall && g_Autowall->PenetrateWall(pTarget, HitBox) && !HitBox.IsZero()) {
					return HitBox;
				}
				pTarget->SetAbsOrigin(original);
				if (g_pEngineTrace->IsVisible(G::LocalPlayer, vEyePos, HitBox, pTarget, hitgroup)) {
					float modified_damage = wpn_data->GetDamage() * (float)pow(wpn_data->GetRangeModifier(), wpn_data->GetRange() * 0.002);
					ScaleDamage(hitgroup, pTarget, wpn_data->GetArmorRatio(), modified_damage);
					if (!HitBox.IsZero() && modified_damage >= minimum_damage)
						return HitBox;
				}
			}
			for (auto HitboxID : GetHitboxesToScan(pTarget)) {
				Vector vPoint;
				std::vector<Vector> Points = GameUtils::GetMultiplePointsForHitbox(pTarget, HitboxID, record.boneMatrix);
				for (int k = 0; k < Points.size(); k++) {
					vPoint = Points.at(k);
					pTarget->SetAbsOrigin(record.m_vecOrigin);
					*reinterpret_cast<int*>(uintptr_t(pTarget) + 0xA30) = g_pGlobals->framecount;
					*reinterpret_cast<int*>(uintptr_t(pTarget) + 0xA28) = 0;
					pTarget->InvalidateBoneCache();
					static float damage_given = 0.f;
					if (Clientvariables->Ragebot.Autowall && g_Autowall->CanHit(pTarget, vPoint, &damage_given)) {
						if (damage_given >= minimum_damage) {
							if (vPoint != Vector(0, 0, 0))
								return vPoint;
						}
					}
					pTarget->SetAbsOrigin(original);
					int hitgroup = -1;
					if (g_pEngineTrace->IsVisible(G::LocalPlayer, vEyePos, vPoint, pTarget, hitgroup)) {
						if (vPoint != Vector(0, 0, 0))
							return vPoint;
					}
				}
			}
		}
	}
	else
	{
		pTarget->SetupBones(BoneMatrix, 128, 0x100, g_pGlobals->curtime);
		simtime = pTarget->GetSimulationTime();
		origin = pTarget->GetOrigin();
		for (auto HitBox : MainHitbox(pTarget, BoneMatrix)) {
			if (g_pEngineTrace->IsVisible(G::LocalPlayer, vEyePos, HitBox, pTarget) && !HitBox.IsZero())
				return HitBox;
			if (Clientvariables->Ragebot.Autowall && g_Autowall->PenetrateWall(pTarget, HitBox) && !HitBox.IsZero())
				return HitBox;
		}
		for (auto HitboxID : GetHitboxesToScan(pTarget)) {
			Vector vPoint;
			std::vector<Vector> Points = GameUtils::GetMultiplePointsForHitbox(pTarget, HitboxID, BoneMatrix);
			for (int k = 0; k < Points.size(); k++) {
				vPoint = Points.at(k);
				float damage = 0.f;
				int hitgroup = -1;
				if (g_pEngineTrace->IsVisible(G::LocalPlayer, vEyePos, vPoint, pTarget, hitgroup)) {
					if (vPoint != Vector(0, 0, 0))
						return vPoint;
				}
				static float damage_given = 0.f;
				if (Clientvariables->Ragebot.Autowall && g_Autowall->CanHit(pTarget, vPoint, &damage_given)) {
					if (damage_given >= minimum_damage) {
						if (vPoint != Vector(0, 0, 0))
							return vPoint;
					}
				}
			}
		}
	}
	return Vector(0, 0, 0);
}
CAimbot* g_Aimbot = new CAimbot;
void CAimbot::DropTarget()
{
    target_index = -1;
    best_distance = 99999.f;
    aimbotted_in_current_tick = false;
    fired_in_that_tick = false;
    current_aim_position = Vector();
    pTarget = nullptr;
}
float get_curtime1() {
	if (!G::LocalPlayer)
		return 0;

	int g_tick = 0;
	CUserCmd* g_pLastCmd = nullptr;
	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = G::LocalPlayer->GetTickBase();
	}
	else {
		++g_tick;
	}
	g_pLastCmd = G::UserCmd;
	float curtime = g_tick * g_pGlobals->interval_per_tick;
	return curtime;
}
void CAimbot::Run()
{
    if( !Clientvariables->Ragebot.EnableAimbot)
        return;
    QAngle view; g_pEngine->GetViewAngles( view );
    CBaseCombatWeapon* pWeapon = G::LocalPlayer->GetWeapon();
	if (Clientvariables->Ragebot.AutomaticRevolver)
	{
		if (G::MainWeapon->WeaponID() == ItemDefinitionIndex::REVOLVER) {
			{
				G::UserCmd->buttons |= IN_ATTACK;
				float flPostponeFireReady = G::MainWeapon->GetPostponeFireReadyTime();
				if (flPostponeFireReady > 0 && flPostponeFireReady < get_curtime1()) {
					G::UserCmd->buttons &= ~IN_ATTACK;
				}
			}
		}
	}
    if( !pWeapon || pWeapon->Clip1() == 0 || pWeapon->IsMiscWeapon() || !GameUtils::IsAbleToShoot() )
        return;
    for( int i = 1; i < 64; ++i ) {
        if( i == g_pEngine->GetLocalPlayer() )
            continue;
        CBaseEntity* target = g_pEntitylist->GetClientEntity( i );
        if( !target->IsValidTarget() )
            continue;
		G::Target = target;
        float fov = GameUtils::GetFoV( view, G::LocalPlayer->GetEyePosition(), target->GetEyePosition(), false );
        if( fov > 180.f)
            continue;
        float simtime = 0;
        Vector minus_origin = Vector( 0, 0, 0 );
        Vector aim_position = RunAimScan( target, simtime, minus_origin );
        if( aim_position == Vector( 0, 0, 0 ) )
            continue;
        float selection_value = 0;
        switch( Clientvariables->Ragebot.AimbotSelection ) {
        case 0:
            selection_value = fov;
            break;
        case 2:
            selection_value = Math::RandomFloat( 0, 100 );
            break;
        case 3:
            selection_value = target->GetVelocity().Length();
            break;
        case 4:
            selection_value = target->GetHealth();
            break;
        default:
            break;
        }
        if( best_distance >= selection_value && aim_position != Vector( 0, 0, 0 ) ) {
            best_distance = selection_value;
            target_index = i;
            current_aim_position = aim_position;
            pTarget = target;
            current_aim_simulationtime = simtime;
            current_aim_player_origin = minus_origin;
        }
    }
    if( target_index != -1 && current_aim_position != Vector( 0, 0, 0 ) && pTarget ) {
        aimbotted_in_current_tick = true;
        QAngle aim = GameUtils::CalculateAngle( G::LocalPlayer->GetEyePosition(), current_aim_position );
        aim.y = Math::NormalizeYaw( aim.y );
        G::UserCmd->viewangles = aim;
        if(!Clientvariables->Ragebot.SilentAimbot)
            g_pEngine->SetViewAngles( G::UserCmd->viewangles );
		if (Clientvariables->Ragebot.AutomaticFire)
		{
			if (Clientvariables->Ragebot.AutomaticScope && pWeapon->IsScopeable() && !G::LocalPlayer->IsScoped())
				G::UserCmd->buttons |= IN_ATTACK2;
			else
			{
				if (Clientvariables->Ragebot.Minhitchance == 0 || HitChance(pTarget, pWeapon, aim, Clientvariables->Ragebot.Minhitchance))
				{
					G::UserCmd->buttons |= IN_ATTACK;
					this->fired_in_that_tick = true;
				}
			}
		}
        if(Clientvariables->Ragebot.PositionAdjustment)
            G::UserCmd->tick_count = TIME_TO_TICKS( current_aim_simulationtime ) + TIME_TO_TICKS( g_BacktrackHelper->GetLerpTime() );// for backtracking
    }
}
void CAimbot::CompensateInaccuracies()
{
	if (G::UserCmd->buttons & IN_ATTACK)
	{
		if (Clientvariables->Ragebot.NoRecoil)
		{
			if (Clientvariables->Misc.AntiUT)
			{
				G::UserCmd->viewangles -= G::LocalPlayer->GetPunchAngle() * 2; // на нсе какого то ху€ в ноги ебашит xD
			}
			else
			{
				ConVar* recoilscale = g_pCvar->FindVar("weapon_recoil_scale");
				if (recoilscale) {
					QAngle qPunchAngles = G::LocalPlayer->GetPunchAngle();
					QAngle qAimAngles = G::UserCmd->viewangles;
					qAimAngles -= qPunchAngles * recoilscale->GetFloat();
					G::UserCmd->viewangles = qAimAngles;
				}
			}
		}
		if (Clientvariables->Ragebot.NoSpread && !Clientvariables->Misc.AntiUT)
			G::UserCmd->viewangles = g_NoSpread->SpreadFactor(G::UserCmd->random_seed);
	}
}
