#include "sdk.h"
#include "Antiaim.h"
#include "global.h"
#include "GameUtils.h"
#include "Math.h"
#include "Aimbot.h"
#include "AAUtils.h"
#include <time.h>

CAntiaim* g_Antiaim = new CAntiaim;
static bool fakewalk;
float get_curtime() {
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
bool next_lby_update() {
	if (G::LocalPlayer) {
		static float next_lby_update_time;
		const float current_time = get_curtime();
		static bool in_air;
		G::localtime = next_lby_update_time;
		static bool stop;
		static bool stop1;
		stop = G::LocalPlayer->GetVelocity().Length2D() < 40 && *G::LocalPlayer->GetFlags() & FL_ONGROUND;
		stop1 = GetAsyncKeyState(VK_SHIFT) && *G::LocalPlayer->GetFlags() & FL_ONGROUND;
		static bool stop2;
		stop2 = !(*G::LocalPlayer->GetFlags() & FL_ONGROUND) && !Clientvariables->Misc.AntiUT;
		if (stop) {
			if ((next_lby_update_time < current_time)) {
				next_lby_update_time = current_time + 1.1f;
				return true;
			}
		}
		else if (stop1)
		{
			if ((next_lby_update_time < current_time)) {
				next_lby_update_time = current_time + 1.1f;
				return true;
			}
		}
	}
	return false;
}
void CAntiaim::Pitch_AA()
{
	float temp = 0;
	switch (Clientvariables->Antiaim.Pitch)
	{
	case 1:
	{
		if (Clientvariables->Misc.AntiUT)
		{
			temp = 89;
		}
		else {
			if (G::SendPacket)
			{
				temp = 120;
			}
			else
			{
				static bool side = false;
				if ((g_pGlobals->tickcount % 100) > 1 && (g_pGlobals->tickcount % 100) < 50)
					side = true;
				else
					side = false;
				static int swap3;
				swap3++;
				static bool flip;
				if (swap3 >= 50)
				{
					swap3 = 0;
					flip = !flip;
					temp = flip ? -180 : 180;
				}
				else
					temp = side ? 1080 : 147;
			}

		}
	}
	break;
	case 2:
	{
		bool UT = Clientvariables->Misc.AntiUT;
		temp = UT ? 0 : 1080;
	}
	break;
	case 3:
	{
		bool UT = Clientvariables->Misc.AntiUT;
		temp = UT ? -70 : 271;
	}
	break;
	}
	G::UserCmd->viewangles.x = temp;
}
static inline bool IsNearEqual(float v1, float v2, float Tolerance)
{
	return std::abs(v1 - v2) <= std::abs(Tolerance);
}
void OnBind()
{
	static bool side = false;
	static bool side2 = false;
	static bool back = false;
	static bool flip = false; flip = !flip;
	if (GetAsyncKeyState(VK_LEFT))
	{
		side = true;
		side2 = false;
		back = false;
	}
	else if (GetAsyncKeyState(VK_RIGHT))
	{
		side = false;
		side2 = true;
		back = false;
	}
	else if (GetAsyncKeyState(VK_DOWN))
	{
		side = false;
		side2 = false;
		back = true;
	}
	if (G::SendPacket)
	{
		if (side)
			G::UserCmd->viewangles.y += 90;
		else if (side2)
			G::UserCmd->viewangles.y -= 90;
		else if (back)
		{

		}
	}
	else
	{
		if (side)
			G::UserCmd->viewangles.y -= 90;
		else if (side2)
			G::UserCmd->viewangles.y += 90;
		else if (back)
		{

		}
	}
}
void YawAntiAim(int index)
{
	if (Clientvariables->Misc.AntiUT)
	{
		if (Clientvariables->Antiaim.breakaa == 1)
		{
			OnBind();

		}
		else if (Clientvariables->Antiaim.breakaa == 2)
		{
			OldFreestanding::freeStand();
		}
		switch (index)
		{
		case 1:
			G::UserCmd->viewangles.y += 180;
			break;
		case 2:
		{
			G::UserCmd->viewangles.y = G::LocalPlayer->LowerBodyYaw() + 90;
		}
		break;
		case 3:
		{
			int value = rand() % 361;
			float clamped = G::LocalPlayer->LowerBodyYaw() + value;
			if (fabsf(G::LocalPlayer->LowerBodyYaw() - clamped) < 45)
				value += 50;
			else if (fabsf(G::LocalPlayer->LowerBodyYaw() - clamped) > 310)
				value -= 50;
			G::UserCmd->viewangles.y = G::LocalPlayer->LowerBodyYaw() + value - 180;
		}
		break;
		case 4:
		{
			static float Ticks;
			Ticks += 5;
			if (Ticks > 270)
				Ticks = 135;
			G::UserCmd->viewangles.y += Ticks;
		}
		break;
		case 5:
		{
			G::UserCmd->viewangles.y -= Math::RandomFloat2(-180.f, 180.f);
		}
		break;
		case 6: {
			G::UserCmd->viewangles.y += 180 - Math::RandomFloat(-35, 35);
		}
				break;
		case 7:
		{
			static float T;
			T += 8;
			G::UserCmd->viewangles.y += T;
		}
		break;
		}
	}
	else {
		if (!(*G::LocalPlayer->GetFlags() & FL_ONGROUND))
		{
			static bool side = false;
			if ((g_pGlobals->tickcount % 100) > 1 && (g_pGlobals->tickcount % 100) < 50)
				side = true;
			else
				side = false;
			if (G::SendPacket) {
				G::UserCmd->viewangles.y = side ? 90 : -90;
			}
			else
			{
				G::UserCmd->viewangles.y = G::LocalPlayer->LowerBodyYaw() + 90;
			}
		}
		else
			G::UserCmd->viewangles.y = G::LocalPlayer->LowerBodyYaw() + 90;
	}
}

bool GetAlive(CBaseEntity* pLocal)
{
	for (int i = 1; i < 64; ++i)
	{
		if (i == g_pEngine->GetLocalPlayer())
			continue;
		CBaseEntity* target = g_pEntitylist->GetClientEntity(i);
		player_info_t info;
		//Calls from left->right so we wont get an access violation error
		Vector pos;
		if (!target || target->GetHealth() < 1)
			continue;
		if (pLocal->GetTeamNum() != target->GetTeamNum())
		{
			return true;
		}
	}
	return false;
}
void CAntiaim::Run(QAngle org_view)
{
	if (Clientvariables->Antiaim.AntiaimEnable)
	{
		static int iChokedPackets = -1;
		if ((g_Aimbot->fired_in_that_tick && iChokedPackets < 4 && GameUtils::IsAbleToShoot()) && !G::ForceRealAA)
		{
			G::SendPacket = false;
			iChokedPackets++;
		}
		else
		{
			iChokedPackets = 0;
			CGrenade* pCSGrenade = (CGrenade*)G::LocalPlayer->GetWeapon();
			if (G::UserCmd->buttons & IN_USE
				/*|| !GetAlive(G::LocalPlayer) && !Clientvariables->Misc.AntiUT*/
				|| G::LocalPlayer->GetMoveType() == MOVETYPE_LADDER && G::LocalPlayer->GetVelocity().Length() > 0
				|| G::LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP
				|| *G::LocalPlayer->GetFlags() & FL_FROZEN
				|| pCSGrenade && pCSGrenade->GetThrowTime() > 0.f)
				return;
			choke = !choke;
			if (!Clientvariables->Misc.FakelagEnable || (*G::LocalPlayer->GetFlags() & FL_ONGROUND && !Clientvariables->Misc.FakelagOnground || *G::LocalPlayer->GetFlags() & FL_ONGROUND && G::LocalPlayer->GetVelocity().Length() < 3))
			G::SendPacket = choke;
			G::UserCmd->viewangles = org_view; //fixes aimbot angles
			Fakewalk(G::UserCmd);
			if (G::bShouldChoke)
				G::SendPacket = G::bShouldChoke = false;
			if (!G::SendPacket)
				G::nChokedTicks++;
			else
				G::nChokedTicks = 0;
			Pitch_AA();
			if (G::SendPacket)
				YawAntiAim(Clientvariables->Antiaim.FakeYaw);
			else {
				if (next_lby_update()) {
					G::UserCmd->viewangles.y += Clientvariables->Antiaim.FreestandingDelta;
				}
				YawAntiAim(Clientvariables->Antiaim.Yaw);
			}
		}
	}
}

void Accelerate(CBaseEntity *player, Vector &wishdir, float wishspeed, float accel, Vector &outVel)
{
	float currentspeed = outVel.Dot(wishdir);
	float addspeed = wishspeed - currentspeed;
	if (addspeed <= 0)
		return;
	float accelspeed = accel * g_pGlobals->frametime * wishspeed * player->m_surfaceFriction();
	if (accelspeed > addspeed)
		accelspeed = addspeed;
	for (int i = 0; i < 3; i++)
		outVel[i] += accelspeed * wishdir[i];
}
void WalkMove(CBaseEntity *player, Vector &outVel)
{
	Vector forward, right, up, wishvel, wishdir, dest;
	float_t fmove, smove, wishspeed;
	Math::AngleVectors(player->GetEyeAngles(), forward, right, up);  // Determine movement angles
	g_pMoveHelper->SetHost(player);
	fmove = g_pMoveHelper->m_flForwardMove;
	smove = g_pMoveHelper->m_flSideMove;
	g_pMoveHelper->SetHost((CBaseEntity*)nullptr);
	if (forward[2] != 0)
	{
		forward[2] = 0;
		Math::NormalizeVector(forward);
	}
	if (right[2] != 0)
	{
		right[2] = 0;
		Math::NormalizeVector(right);
	}
	for (int i = 0; i < 2; i++)	// Determine x and y parts of velocity
		wishvel[i] = forward[i] * fmove + right[i] * smove;
	wishvel[2] = 0;	// Zero out z part of velocity
	wishdir = wishvel; // Determine maginitude of speed of move
	wishspeed = wishdir.Normalize();
	// Clamp to server defined max speed
	g_pMoveHelper->SetHost(player);
	if ((wishspeed != 0.0f) && (wishspeed > g_pMoveHelper->m_flMaxSpeed))
	{
		VectorMultiply(wishvel, player->m_flMaxspeed() / wishspeed, wishvel);
		wishspeed = player->m_flMaxspeed();
	}
	g_pMoveHelper->SetHost(nullptr);
	// Set pmove velocity
	outVel[2] = 0;
	Accelerate(player, wishdir, wishspeed, g_pCvar->FindVar("sv_accelerate")->GetFloat(), outVel); // Always have to have the biggest dynamic variable searching ever.
	outVel[2] = 0;
	// Add in any base velocity to the current velocity.
	VectorAdd(outVel, player->GetBaseVelocity(), outVel);
	float spd = outVel.Length();
	if (spd < 1.0f)
	{
		outVel.Init();
		// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(outVel, player->GetBaseVelocity(), outVel);
		return;
	}
	g_pMoveHelper->SetHost(player);
	g_pMoveHelper->m_outWishVel += wishdir * wishspeed;
	g_pMoveHelper->SetHost(nullptr);
	// Don't walk up stairs if not on ground.
	if (!(*player->GetFlags() & FL_ONGROUND))
	{
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(outVel, player->GetBaseVelocity(), outVel);
		return;
	}
	// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(outVel, player->GetBaseVelocity(), outVel);
}
void CAntiaim::Fakewalk(CUserCmd *userCMD)
{
	if (GetAsyncKeyState(VK_SHIFT))
	{
		Vector velocity = G::vecUnpredictedVel;
		int Iterations = 0;
		for (; Iterations < 15; ++Iterations) {
			if (velocity.Length() < 0.1)
			{
				//g_pCvar->ConsolePrintf("Ticks till stop %d\n", Iterations);
				//Msg("Ticks till stop %d\n", Iterations);
				break;
			}
			Friction(velocity);
			WalkMove(G::LocalPlayer, velocity);
		}
	    int choked_ticks = G::nChokedTicks;
		if (Iterations > 7 - choked_ticks || !choked_ticks)
		{
			float_t speed = velocity.Length();

			QAngle direction;
			Math::VectorAngles(velocity, direction);

			direction.y = userCMD->viewangles.y - direction.y;

			Vector forward;
			Math::AngleVectors(direction, forward);
			Vector negated_direction = forward * -speed;

			userCMD->forwardmove = negated_direction.x;
			userCMD->sidemove = negated_direction.y;
		}

		if (next_lby_update())
		{
			G::bShouldChoke = false;
			//	G::UserCmd->viewangles.y += Clientvariables->Antiaim.FreestandingDelta;
		}

		if (G::nChokedTicks < 7)
			G::bShouldChoke = true;
	}
}
inline float GetYawDelta(float yaw1, float yaw2)
{
	return fabs(Math::NormalizeYaw(yaw1 - yaw2));
}
float GetCurtime()
{
	auto local_player = G::LocalPlayer;
	if (!local_player)
		return 0.f;

	return local_player->GetTickBase() * g_pGlobals->interval_per_tick;
}

float m_next_lby_update_time = 0.f, m_last_move_time = 0.f, m_last_attempted_lby = 0.f;
bool m_will_lby_update = false;


void CAntiaim::UpdateLBY()
{
	auto local_player = G::LocalPlayer;
	if (!local_player || !(*local_player->GetFlags() & FL_ONGROUND))
		return;

	auto animstate = local_player->GetBasePlayerAnimState();
	if (!animstate)
		return;

	auto net_channel = g_pEngine->GetNetChannel();
	if (!net_channel || net_channel->m_nChokedPackets)
		return;

	const float curtime = GetCurtime();
	if (animstate->m_velocity > 0.1 )
	{
		m_next_lby_update_time = curtime + 0.22;
		m_last_move_time = curtime;
	}
	static float last_lby;
	if (
		last_lby != local_player->LowerBodyYaw()
		)
	{
		last_lby = local_player->LowerBodyYaw();
		if (fabs(curtime - m_last_move_time) > 0.3 && fabs(Math::NormalizeYaw(local_player->LowerBodyYaw() - m_last_attempted_lby)) > 35.f)
			m_next_lby_update_time = curtime + 1.1 + g_pGlobals->interval_per_tick;
	}

	if (m_next_lby_update_time < curtime)
	{
		m_next_lby_update_time = curtime + 1.1;
		m_will_lby_update = true;
	}
}

float CAntiaim::BreakLBY(float real_yaw, float desired_lby, bool double_flick)
{
	auto local_player = G::LocalPlayer;
	if (!local_player)
		return real_yaw;

	desired_lby = GetYawDelta(desired_lby, local_player->LowerBodyYaw()) < 35.f ? local_player->LowerBodyYaw() : desired_lby;
	if (m_will_lby_update)
	{
		const float lby_delta = Math::NormalizeYaw(desired_lby - real_yaw);
		static bool switch_bool = false;
		switch_bool = !switch_bool;
		if (
			fabs(lby_delta) < 100.f
			)
			desired_lby = real_yaw + (switch_bool ? lby_delta : -lby_delta);

		m_will_lby_update = false;
		m_last_attempted_lby = desired_lby;
		return desired_lby;
	}

	if (fabs(Math::NormalizeYaw(real_yaw - desired_lby)) > 130.f && fabs(Math::NormalizeYaw(desired_lby - local_player->LowerBodyYaw())) < 35.f)
	{
		if (fabs(GetCurtime() - m_next_lby_update_time) < g_pGlobals->interval_per_tick)
			return desired_lby - 115.f;
		if (fabs(GetCurtime() - m_next_lby_update_time) > 1.1 - (g_pGlobals->interval_per_tick * 5))
			return desired_lby + 115.f;
	}

	return real_yaw;
}
void CAntiaim::Friction(Vector &outVel)
{
	float speed, newspeed, control;
	float friction;
	float drop;

	speed = outVel.Length();

	if (speed <= 0.1f)
		return;

	drop = 0;

	// apply ground friction
	if (*G::LocalPlayer->GetFlags() & FL_ONGROUND)
	{
		friction = g_pCvar->FindVar("sv_friction")->GetFloat() * G::LocalPlayer->m_surfaceFriction();

		// Bleed off some speed, but if we have less than the bleed
		// threshold, bleed the threshold amount.
		control = (speed < g_pCvar->FindVar("sv_stopspeed")->GetFloat()) ? g_pCvar->FindVar("sv_stopspeed")->GetFloat() : speed;

		// Add the amount to the drop amount.
		drop += control * friction * g_pGlobals->frametime;
	}

	newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;

	if (newspeed != speed)
	{
		// Determine proportion of old speed we are using.
		newspeed /= speed;
		// Adjust velocity according to proportion.
		VectorMultiply(outVel, newspeed, outVel);
	}
}