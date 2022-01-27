#include "sdk.h"
#include "BacktrackingHelper.h"
#include "global.h"
#include "xor.h"
#include "Math.h"



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
#define TICK_INTERVAL			( g_pGlobals->interval_per_tick )
#define TIME_TO_TICKS( dt )		( floorf(( 0.5f + (float)(dt) / TICK_INTERVAL ) ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )
CBacktrackHelper* g_BacktrackHelper = new CBacktrackHelper;


void CBacktrackHelper::AnimFix(CBaseEntity* entity)
{


	struct clientanimating_t
	{
		CBaseAnimating *pAnimating;
		unsigned int	flags;
		clientanimating_t(CBaseAnimating *_pAnim, unsigned int _flags) : pAnimating(_pAnim), flags(_flags) {}
	};


	clientanimating_t *animating = nullptr;
	int animflags;

	const unsigned int FCLIENTANIM_SEQUENCE_CYCLE = 0x00000001;

	AnimationLayer AnimLayer[15];
 

	int cnt = 15;
	std::memcpy(AnimLayer, entity->GetAnimOverlays(), (sizeof(AnimationLayer) * entity->GetNumAnimOverlays()));

	float flPoseParameter[24];
	float* pose = (float*)((uintptr_t)entity + 0x2764);
	memcpy(&flPoseParameter, pose, sizeof(float) * 24);

	Vector TargetEyeAngles = *entity->GetEyeAnglesPtr();

	bool bForceAnimationUpdate = entity->GetEyeAnglesPtr()->x != TargetEyeAngles.x || entity->GetEyeAnglesPtr()->y != TargetEyeAngles.y;

	if (bForceAnimationUpdate)
	{
		//Update animations and pose parameters
		clientanimating_t *animating = nullptr;
		int animflags;

		//Make sure game is allowed to client side animate. Probably unnecessary
		for (unsigned int i = 0; i < g_ClientSideAnimationList->count; i++)
		{
			clientanimating_t *tanimating = (clientanimating_t*) g_ClientSideAnimationList->Retrieve(i, sizeof(clientanimating_t));
		     CBaseEntity *pAnimEntity = (CBaseEntity*)tanimating->pAnimating;
			if (pAnimEntity == entity)
			{
				animating = tanimating;
				animflags = tanimating->flags;
				tanimating->flags |= FCLIENTANIM_SEQUENCE_CYCLE;
				break;
			}
		}

		//Update animations/poses
		entity->UpdateClientSideAnimation();

		//Restore anim flags
		if (animating)
			animating->flags = animflags;
	}
	for (unsigned int i = 0; i < g_ClientSideAnimationList->count; i++)
	{
		clientanimating_t *animating = (clientanimating_t*) g_ClientSideAnimationList->Retrieve(i, sizeof(clientanimating_t));
		CBaseEntity *Entity = (CBaseEntity*)animating->pAnimating;
		if (!Entity->IsDormant() && Entity->GetHealth() > 0)
		{

			int TickReceivedNetUpdate[65];

			TickReceivedNetUpdate[entity->GetIndex()] = g_pGlobals->tickcount;

			bool HadClientAnimSequenceCycle[65];

			int ClientSideAnimationFlags[65];
			bool IsBreakingLagCompensation[65];
			IsBreakingLagCompensation[entity->GetIndex()] = !Entity->IsDormant() && entity->GetOrigin().LengthSqr() > (64.0f * 64.0f);

			unsigned int flags = animating->flags;
			ClientSideAnimationFlags[entity->GetIndex()] = flags;
			HadClientAnimSequenceCycle[entity->GetIndex()] = (flags & FCLIENTANIM_SEQUENCE_CYCLE);
			if (HadClientAnimSequenceCycle[entity->GetIndex()])
			{
				if (IsBreakingLagCompensation[entity->GetIndex()] && g_pGlobals->tickcount != TickReceivedNetUpdate[entity->GetIndex()])
				{
					Entity->UpdateClientSideAnimation();
					//Store the new animations
					Entity->CopyPoseParameters(flPoseParameter);
					std::memcpy(entity->GetAnimOverlays(), AnimLayer, (sizeof(AnimationLayer) * entity->GetNumAnimOverlays()));
				}
			}
		}
	}
	if (G::LocalPlayer) {

		for (unsigned int i = 0; i < g_ClientSideAnimationList->count; i++)
		{
			clientanimating_t *animating = (clientanimating_t*)g_ClientSideAnimationList->Retrieve(i, sizeof(clientanimating_t));
			CBaseEntity *Entity = (CBaseEntity*)animating->pAnimating;
			if (!Entity->IsDormant() && Entity->GetHealth() > 0)
			{
				bool HadClientAnimSequenceCycle[65];

				int ClientSideAnimationFlags[65];

				unsigned int flags = animating->flags;
				ClientSideAnimationFlags[entity->GetIndex()] = flags;
				HadClientAnimSequenceCycle[entity->GetIndex()] = (flags & FCLIENTANIM_SEQUENCE_CYCLE);

				if (HadClientAnimSequenceCycle[entity->GetIndex()])
				{
					animating->flags |= FCLIENTANIM_SEQUENCE_CYCLE;
				}
			}
		}
	}
}
void CBacktrackHelper::meme(Vector &velocity, Vector &origin, CBaseEntity* pPlayer, int &flags, bool was_in_air)
{
	
		int index = pPlayer->GetIndex();
		simulation_record latest_record = this->SimRecord[index][0];
		simulation_record pre_latest_record = this->SimRecord[index][1];
		int latency_ticks = GetLatencyTicks();

		Vector vel = velocity;
		INetChannelInfo* nci = g_pEngine->GetNetChannelInfo();
		if (!(flags & FL_ONGROUND))
			velocity.z -= (g_pGlobals->frametime * g_pCvar->FindVar("sv_gravity")->GetFloat());
		else if (was_in_air)
			velocity.z = g_pCvar->FindVar("sv_jump_impulse")->GetFloat();

		const Vector mins = pPlayer->GetCollision()->VecMins();
		const Vector max = pPlayer->GetCollision()->VecMaxs();

		const Vector src = origin;
		Vector end = src + (velocity * g_pGlobals->frametime);

		Ray_t ray;
		ray.Init(src, end, mins, max);

		trace_t trace;
		CTraceFilter filter;
		filter.pSkip = (void*)(pPlayer);

		g_pEngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

		if (trace.fraction != 1.f)
		{
			for (int i = 0; i < 2; i++)
			{
				velocity -= trace.plane.normal * velocity.Dot(trace.plane.normal);

				const float dot = velocity.Dot(trace.plane.normal);
				if (dot < 0.f)
				{
					velocity.x -= dot * trace.plane.normal.x;
					velocity.y -= dot * trace.plane.normal.y;
					velocity.z -= dot * trace.plane.normal.z;
				}

				end = trace.endpos + (velocity * (g_pGlobals->interval_per_tick * (1.f - trace.fraction)));

				ray.Init(trace.endpos, end, mins, max);
				g_pEngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

				if (trace.fraction == 1.f)
					break;
			}
		}

		origin = trace.endpos;
		end = trace.endpos;
		end.z -= 2.f;

		ray.Init(origin, end, mins, max);
		g_pEngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

		flags &= ~(1 << 0);

		if (trace.DidHit() && trace.plane.normal.z > 0.7f)
			flags |= (1 << 0);

}


bool CBacktrackHelper::IsTickValid(tick_record record)
{
	float correct = 0;

	correct += g_pEngine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
	correct += g_pEngine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING);
	correct += g_BacktrackHelper->GetLerpTime();

	static ConVar* sv_maxunlag = g_pCvar->FindVar("sv_maxunlag");
	correct = clamp<float>(correct, 0.0f, sv_maxunlag->GetFloat());

	float deltaTime = correct - (g_pGlobals->curtime - record.m_flSimulationTime);

	float latecy = Clientvariables->Ragebot.FakeLatencyAmount;
	float ping = Clientvariables->Ragebot.FakeLatency ? latecy : 0.2f;
	if (fabsf(deltaTime) < ping /*&& fabsf(deltaTime) > ping - 0.2f*/)
	{
		return true;
	}

	return false;
}
void CBacktrackHelper::ebala()
{
	auto player = G::LocalPlayer;
	if (player)
	{
	   auto& lag_records = this->PlayerRecord[player->Index()].records;

	   Vector position = lag_records.back().m_vecOrigin;

       ExtrapolatePosition(player, position, lag_records.back().m_flSimulationTime, lag_records.back().m_vecVelocity);

	}
}
/*void CBacktrackHelper::AntimFIX(ClientFrameStage_t stage)
{
	if (!G::LocalPlayer->isAlive())
		return;

	for (int i = 1; i < g_pGlobals->maxClients; i++)
	{
		CBaseEntity* player = g_pEntitylist->GetClientEntity(i);
		if (player)
		{
			auto& record = this->PlayerRecord[player->Index()].records;

			auto leet = [](CBaseEntity *player) -> void
			{
				static ConVar *sv_pvsskipanimation = g_pCvar->FindVar("sv_pvsskipanimation");

				int32_t backup_sv_pvsskipanimation = sv_pvsskipanimation->GetInt();
				sv_pvsskipanimation->SetValue(0);

				*(int32_t*)((uintptr_t)player + 0xA30) = 0;
				*(int32_t*)((uintptr_t)player + 0x269C) = 0;

				int32_t backup_effects = *(int32_t*)((uintptr_t)player + 0xEC);
				*(int32_t*)((uintptr_t)player + 0xEC) |= 8;

				player->SetupBones(NULL, -1, 0x7FF00, g_pGlobals->curtime);

				*(int32_t*)((uintptr_t)player + 0xEC) = backup_effects;
				sv_pvsskipanimation->SetValue(backup_sv_pvsskipanimation);
			};

			const float curtime = g_pGlobals->curtime;
			const float frametime = g_pGlobals->frametime;
			AnimationLayer backup_layers[15];
			std::memcpy(backup_layers, player->GetAnimOverlays(), (sizeof(AnimationLayer) * player->GetNumAnimOverlays()));
			float flPoseParameter[24];
			float* pose = (float*)((uintptr_t)player + 0x2764);
			std::memcpy(&flPoseParameter, pose, sizeof(float) * 24);
			auto old_ragpos = player->get_ragdoll_pos();
			static auto host_timescale = g_pCvar->FindVar(("host_timescale"));
			g_pGlobals->frametime = g_pGlobals->interval_per_tick * host_timescale->GetFloat();
			g_pGlobals->curtime = player->GetOldSimulationTime() + g_pGlobals->interval_per_tick;
			Vector backup_origin = player->GetOrigin();
			Vector backup_absorigin = player->GetAbsOrigin();
			Vector backup_velocity = player->GetVelocity();
			int backup_flags = *player->GetFlags();
			if (record.size() > 2)
			{
             	bool ChokedPacket = TIME_TO_TICKS(player->GetSimulationTime() - record.back().m_flSimulationTime) > 1;
				bool Air = false;

				if (!(*player->GetFlags() & FL_ONGROUND) || !(record.back().m_nFlags & FL_ONGROUND))
					Air = true;

				if (ChokedPacket)
				{
					player->GetOrigin() = record.back().m_vecOrigin;
					player->SetAbsOrigin(record.back().m_vecAbsOrigin);
					player->GetVelocity() = record.back().m_vecVelocity;
					*player->GetFlags() = record.back().m_nFlags;
				}

				Vector data_origin = player->GetOrigin();
				Vector data_velocity = player->GetVelocity();
				int data_flags = *player->GetFlags();
				Vector position = record.back().m_vecOrigin;
				if (ChokedPacket)
				{
					meme(data_velocity, data_origin, player, data_flags, Air);
					player->GetOrigin() = data_origin;
					player->SetAbsOrigin(data_origin);
					player->GetVelocity() = data_velocity;

					*player->GetFlags() &= 0xFFFFFFFE;
					auto penultimate_record = *std::prev(record.end(), 2);
					if ((record.back().m_nFlags & FL_ONGROUND) && (penultimate_record.m_nFlags & FL_ONGROUND))
						*player->GetFlags() |= 1;
					if (*(float*)((uintptr_t)player->GetAnimOverlay(0) + 0x138) > 0.f)
						*player->GetFlags() |= 1;
				}
				auto GetClientSideAnimation = player->GetClientSideAnimation();
				AnimationLayer backup_layers[15];
				std::memcpy(backup_layers, player->GetAnimOverlays(), (sizeof(AnimationLayer) * player->GetNumAnimOverlays()));

				// invalidates prior animations so the entity gets animated on our client 100% via UpdateClientSideAnimation
				CBasePlayerAnimState *state = player->GetBasePlayerAnimState();
				if (state)
					state->m_iLastClientSideAnimationUpdateFramecount = g_pGlobals->framecount - 1;


				GetClientSideAnimation = true;

				// updates local animations + poses + calculates new abs angle based on eyeangles and other stuff
				player->UpdateClientSideAnimation();

				GetClientSideAnimation = false;



				// restore
				std::memcpy(player->GetAnimOverlays(), backup_layers, (sizeof(AnimationLayer) * player->GetNumAnimOverlays()));
				player->GetOrigin() = backup_origin;
				player->SetAbsOrigin(backup_absorigin);
				player->GetVelocity() = backup_velocity;
				*player->GetFlags() = backup_flags;
				player->CopyPoseParameters(flPoseParameter);
				player->get_ragdoll_pos() = old_ragpos;
				g_pGlobals->curtime = curtime;
				g_pGlobals->frametime = frametime;

				leet(player);
			}

		}
	}
}*/
void CBacktrackHelper::AnimationFix(ClientFrameStage_t stage)
{
	if (!G::LocalPlayer->isAlive())
		return;

	static int userId[64];
	static AnimationLayer
		backupLayersUpdate[64][15],
		backupLayersInterp[64][15];

	for (int i = 1; i < g_pGlobals->maxClients; i++)
	{
		CBaseEntity* player = g_pEntitylist->GetClientEntity(i);
		if (player)
		{
			auto& lag_records = this->PlayerRecord[player->Index()].records;

			auto leet = [](CBaseEntity *player) -> void
			{
				static ConVar *sv_pvsskipanimation = g_pCvar->FindVar("sv_pvsskipanimation");

				int32_t backup_sv_pvsskipanimation = sv_pvsskipanimation->GetInt();
				sv_pvsskipanimation->SetValue(0);

				*(int32_t*)((uintptr_t)player + 0xA30) = 0;
				*(int32_t*)((uintptr_t)player + 0x269C) = 0;

				int32_t backup_effects = *(int32_t*)((uintptr_t)player + 0xEC);
				*(int32_t*)((uintptr_t)player + 0xEC) |= 8;

				player->SetupBones(NULL, -1, 0x7FF00, g_pGlobals->curtime);

				*(int32_t*)((uintptr_t)player + 0xEC) = backup_effects;
				sv_pvsskipanimation->SetValue(backup_sv_pvsskipanimation);
			};

			// backup
			const float curtime = g_pGlobals->curtime;
			const float frametime = g_pGlobals->frametime;
			float flPoseParameter[24];
			float* pose = (float*)((uintptr_t)player + 0x2764);
			memcpy(&flPoseParameter, pose, sizeof(float) * 24);
			static auto host_timescale = g_pCvar->FindVar(("host_timescale"));
			auto old_ragpos = player->get_ragdoll_pos();
			g_pGlobals->frametime = g_pGlobals->interval_per_tick * host_timescale->GetFloat();
			g_pGlobals->curtime = player->GetOldSimulationTime() + g_pGlobals->interval_per_tick;

			Vector backup_origin = player->GetOrigin();
			Vector backup_absorigin = player->GetAbsOrigin();
			Vector backup_velocity = player->GetVelocity();
			int backup_flags = *player->GetFlags();

			if (lag_records.size() > 2)
			{
				bool bChocked = TIME_TO_TICKS(player->GetSimulationTime() - lag_records.back().m_flSimulationTime) > 1;
				bool bInAir = false;

				if (!(*player->GetFlags() & FL_ONGROUND) || !(lag_records.back().m_nFlags & FL_ONGROUND))
					bInAir = true;

				if (bChocked)
				{
					player->GetOrigin() = lag_records.back().m_vecOrigin;
					player->SetAbsOrigin(lag_records.back().m_vecAbsOrigin);
					player->GetVelocity() = lag_records.back().m_vecVelocity;
					*player->GetFlags() = lag_records.back().m_nFlags;
				}

				Vector data_origin = player->GetOrigin();
				Vector data_velocity = player->GetVelocity();
				int data_flags = *player->GetFlags();
				auto ragpos = lag_records.back().ragpos;
				Vector position = lag_records.back().m_vecOrigin;
				if (bChocked)
				{
				
					meme(data_velocity, data_origin, player, data_flags, bInAir);
					player->GetOrigin() = data_origin;
					player->SetAbsOrigin(data_origin);
					player->GetVelocity() = data_velocity;
					player->get_ragdoll_pos() = ragpos;

					*player->GetFlags() &= 0xFFFFFFFE;
					auto penultimate_record = *std::prev(lag_records.end(), 2);
					if ((lag_records.back().m_nFlags & FL_ONGROUND) && (penultimate_record.m_nFlags & FL_ONGROUND))
						*player->GetFlags() |= 1;
					if (*(float*)((uintptr_t)player->GetAnimOverlay(0) + 0x138) > 0.f)
						*player->GetFlags() |= 1;
				}
			}
			auto GetClientSideAnimation = player->GetClientSideAnimation();
			AnimationLayer backup_layers[15];
			std::memcpy(backup_layers, player->GetAnimOverlays(), (sizeof(AnimationLayer) * player->GetNumAnimOverlays()));

			// invalidates prior animations so the entity gets animated on our client 100% via UpdateClientSideAnimation
			CBasePlayerAnimState *state = player->GetBasePlayerAnimState();
			if (state)
				state->m_iLastClientSideAnimationUpdateFramecount = g_pGlobals->framecount - 1;
		

			GetClientSideAnimation = true;

			// updates local animations + poses + calculates new abs angle based on eyeangles and other stuff
			player->UpdateClientSideAnimation();

			GetClientSideAnimation = false;



			// restore
			std::memcpy(player->GetAnimOverlays(), backup_layers, (sizeof(AnimationLayer) * player->GetNumAnimOverlays()));
			player->GetOrigin() = backup_origin;
			player->SetAbsOrigin(backup_absorigin);
			player->GetVelocity() = backup_velocity;
			*player->GetFlags() = backup_flags;
		//	player->CopyPoseParameters(flPoseParameter);
		//	player->get_ragdoll_pos() = old_ragpos;
			g_pGlobals->curtime = curtime;
			g_pGlobals->frametime = frametime;

			leet(player);
		}
	}
}

int CBacktrackHelper::GetLatencyTicks()
{
	double v0; // st7@0
	INetChannelInfo* v1; // esi@1
	INetChannelInfo* v2; // eax@1
	float v3; // ST08_4@1
	float v4; // ST0C_4@1

	v1 = (INetChannelInfo*)g_pEngine->GetNetChannelInfo();
	v2 = (INetChannelInfo*)g_pEngine->GetNetChannelInfo();

	v3 = v1->GetAvgLatency(Typetype_t::TYPE_LOCALPLAYER);
	v4 = v2->GetAvgLatency(Typetype_t::TYPE_GENERIC);

	float interval_per_tick = 1.0f / g_pGlobals->interval_per_tick;

	return floorf(((v3 + v4) * interval_per_tick) + 0.5f);
}

float CBacktrackHelper::GetEstimateServerTime()
{
	double v0; // st7@0
	INetChannelInfo* v1; // esi@1
	INetChannelInfo* v2; // eax@1
	float v3; // ST08_4@1
	float v4; // ST0C_4@1

	v1 = (INetChannelInfo*)g_pEngine->GetNetChannelInfo();
	v2 = (INetChannelInfo*)g_pEngine->GetNetChannelInfo();

	v3 = v1->GetAvgLatency(Typetype_t::TYPE_LOCALPLAYER);
	v4 = v2->GetAvgLatency(Typetype_t::TYPE_GENERIC);

	//return floorf(((v3 + v4) / g_pGlobals->interval_per_tick) + 0.5f) + 1 + G::UserCmdForBacktracking->tick_count;*/

	return v3 + v4 + TICKS_TO_TIME(1) + TICKS_TO_TIME(G::UserCmdForBacktracking->tick_count);

}
float CBacktrackHelper::GetNetworkLatency()
{
	// Get true latency
	INetChannelInfo *nci = g_pEngine->GetNetChannelInfo();
	if (nci)
	{
		//float IncomingLatency = nci->GetAvgLatency(FLOW_INCOMING); //ppl say use only this one, but meh
		float OutgoingLatency = nci->GetLatency(0);
		return OutgoingLatency;
	}
	return 0.0f;
}
ConVar* minupdate;
ConVar* maxupdate;
ConVar * updaterate;
ConVar * interprate;
ConVar* cmin;
ConVar* cmax;
ConVar* interp;


void CBacktrackHelper::SetOverwriteTick(CBaseEntity *player, QAngle angles, float_t correct_time, uint32_t priority)
{
	int idx = player->Index();
	tick_record overwrite_record;
	auto& m_LagRecords = this->PlayerRecord[player->Index()].records;


	// g_CVar->ConsoleColorPrintf(Color(255, 0, 0, 255), "Dev Error: failed to overwrite tick, delta too big. Priority: %d\n", priority);

	UpdateBacktrackRecords(player);
	overwrite_record.m_angEyeAngles = angles;
	//overwrite_record.m_iPriority = priority;
	overwrite_record.m_flSimulationTime = correct_time;
	m_LagRecords.emplace_back(overwrite_record);
}



float CBacktrackHelper::GetLerpTime()
{
	if (!minupdate)
		minupdate = g_pCvar->FindVar(XorStr("sv_minupdaterate"));
	if (!maxupdate)
		maxupdate = g_pCvar->FindVar(XorStr("sv_maxupdaterate"));
	if (!updaterate)
		updaterate = g_pCvar->FindVar(XorStr("cl_updaterate"));
	if (!interprate)
		interprate = g_pCvar->FindVar(XorStr("cl_interp_ratio"));
	if (!cmin)
		cmin = g_pCvar->FindVar(XorStr("sv_client_min_interp_ratio"));
	if (!cmax)
		cmax = g_pCvar->FindVar(XorStr("sv_client_max_interp_ratio"));
	if (!interp)
		interp = g_pCvar->FindVar(XorStr("cl_interp"));

	float UpdateRate = updaterate->GetValue();
	float LerpRatio = interprate->GetValue();

	return max(LerpRatio / UpdateRate, interp->GetValue());
}
void CBacktrackHelper::ExtrapolatePosition(CBaseEntity* pPlayer, Vector& position, float &simtime, Vector velocity)
{
	int index = pPlayer->GetIndex();
	simulation_record latest_record = this->SimRecord[index][0];
	simulation_record pre_latest_record = this->SimRecord[index][1];
	int latency_ticks = GetLatencyTicks();
	Vector vel = velocity;
	float s_time = simtime;
	INetChannelInfo* nci = g_pEngine->GetNetChannelInfo();
	float latency = nci->GetAvgLatency(0) + nci->GetAvgLatency(1);
	float ticks_per_second = 1.0f / g_pGlobals->interval_per_tick;
	float server_time = ((floorf(((latency)*ticks_per_second) + 0.5) + G::UserCmdForBacktracking->tick_count + 1) * g_pGlobals->interval_per_tick);
	float server_time_difference = server_time - s_time;

	if (server_time_difference > 1.0f)
		server_time_difference = 1.0f;

	float sim_time_difference = g_pGlobals->curtime - latest_record.update_time;
	//proper method
	int ticks_choked = /*TIME_TO_TICKS(latest_record.simulation_time - pre_latest_record.simulation_time_increasment_per_tick);*/TIME_TO_TICKS(sim_time_difference);

	if (ticks_choked <= 15)
	{
		latency_ticks = 1;
		if (ticks_choked < 1)
			ticks_choked = 1;
	}
	else
		ticks_choked = 15;

	int total_difference = floorf((server_time_difference * ticks_per_second) + 0.5) - ticks_choked;

	vel.z -= g_pCvar->FindVar("sv_gravity")->GetValue() * g_pGlobals->interval_per_tick;

	Ray_t ray;
	trace_t tr;
	CTraceFilterWorldAndPropsOnly filter;

	if (total_difference < 0)
	{
		return;
	}
	else
	{
		do
		{
			int chokes = ticks_choked;

			do
			{

				ray.Init(position, position + (vel* g_pGlobals->interval_per_tick), G::LocalPlayer->GetCollision()->VecMins(), G::LocalPlayer->GetCollision()->VecMaxs());

				g_pEngineTrace->TraceRay_NEW(ray, MASK_SOLID, &filter, &tr);

				if (tr.fraction > 0)
				{
					position += vel * (g_pGlobals->interval_per_tick);
					simtime += g_pGlobals->interval_per_tick;
				}
				else
				{

					return;
				}

				chokes--;

			} while (chokes);

			total_difference -= ticks_choked;
		} while (total_difference >= 0);
	}
}

void CBacktrackHelper::UpdateBacktrackRecords(CBaseEntity* pPlayer)
{
	int i = pPlayer->Index();
	for (int j = g_BacktrackHelper->PlayerRecord[i].records.size() - 1; j >= 0; j--)
	{
		tick_record rec = g_BacktrackHelper->PlayerRecord[i].records.at(j);
		float amount = Clientvariables->Ragebot.FakeLatencyAmount;
		float ping = Clientvariables->Ragebot.FakeLatency ? amount : 0.2f;
		if (rec.m_flSimulationTime < g_pGlobals->curtime - ping)
			g_BacktrackHelper->PlayerRecord[i].records.erase(g_BacktrackHelper->PlayerRecord[i].records.begin() + j);

	}
	static Vector old_origin[64];
	if (PlayerRecord[i].records.size() > 0 && pPlayer->GetSimulationTime() == PlayerRecord[i].records.back().m_flSimulationTime) //already got such a record
		return;

	for (int j = g_BacktrackHelper->PlayerRecord[i].records.size() - 1; j >= 0; j--)
	{
		tick_record rec = g_BacktrackHelper->PlayerRecord[i].records.at(j);
		if (PlayerRecord[i].records.size() > 0 && PlayerRecord[i].records.at(j).m_flSimulationTime > pPlayer->GetSimulationTime())//Invalid lag record, maybe from diffrent game?
		{
			PlayerRecord[i].records.clear();
			return;
		}

	}

	Vector cur_origin = pPlayer->GetOrigin();
	Vector v = cur_origin - old_origin[i];
	bool breaks_lagcomp = v.LengthSqr() > 4096.f;
	old_origin[i] = cur_origin;
	tick_record new_record;

	new_record.needs_extrapolation = breaks_lagcomp;
	static float OldLower[64];

	PlayerRecord[i].LowerBodyYawTarget = pPlayer->LowerBodyYaw();
	new_record.m_angEyeAngles = pPlayer->GetEyeAngles();
	new_record.m_flCycle = pPlayer->GetCycle();

	new_record.m_nFlags = *pPlayer->GetFlags();

	new_record.m_flSimulationTime = pPlayer->GetSimulationTime();
	new_record.m_flAnimTime = pPlayer->GetAnimationTime();
	new_record.m_vecAbsOrigin = pPlayer->GetAbsOrigin();
	new_record.bLowerBodyYawUpdated = false;
	new_record.m_nSequence = pPlayer->GetSequence();
	new_record.m_vecOrigin = pPlayer->GetOrigin();
	new_record.m_vecVelocity = pPlayer->GetVelocity();
	new_record.m_flUpdateTime = g_pGlobals->curtime;
	new_record.backtrack_time = new_record.m_flSimulationTime + GetLerpTime();
	new_record.m_vecMins = pPlayer->GetCollision()->VecMins();
	new_record.m_vecMax = pPlayer->GetCollision()->VecMaxs();
	new_record.ragpos = pPlayer->get_ragdoll_pos();

	if (PlayerRecord[i].LowerBodyYawTarget != OldLower[i] || (*pPlayer->GetFlags() & FL_ONGROUND && pPlayer->GetVelocity().Length() > 29.f))
		new_record.bLowerBodyYawUpdated = true;

	for (int i = 0; i < 24; i++)
		new_record.m_flPoseParameter[i] = *(float*)((DWORD)pPlayer + offys.m_flPoseParameter + sizeof(float) * i);

	pPlayer->SetupBones(new_record.boneMatrix, 128, 256, g_pGlobals->curtime);

	OldLower[i] = PlayerRecord[i].LowerBodyYawTarget;
	PlayerRecord[i].records.push_back(new_record);
}
void CBacktrackHelper::UpdateExtrapolationRecords(CBaseEntity* pPlayer)
{
	int index = pPlayer->Index();
	if (pPlayer->GetSimulationTime() == this->SimRecord[index][0].simulation_time)
		return;

	for (int i = 7; i > 0; i--)
	{
		this->SimRecord[index][i].acceleration = this->SimRecord[index][i - 1].acceleration;
		this->SimRecord[index][i].origin = this->SimRecord[index][i - 1].origin;
		this->SimRecord[index][i].simulation_time = this->SimRecord[index][i - 1].simulation_time;
		this->SimRecord[index][i].update_time = this->SimRecord[index][i - 1].update_time;
		this->SimRecord[index][i].velocity = this->SimRecord[index][i - 1].velocity;
	}

	this->SimRecord[index][0].simulation_time = pPlayer->GetSimulationTime();
	this->SimRecord[index][0].update_time = g_pGlobals->curtime;
	this->SimRecord[index][0].origin = pPlayer->GetOrigin();

	int lost_ticks = TIME_TO_TICKS(this->SimRecord[index][0].simulation_time) - TIME_TO_TICKS(this->SimRecord[index][1].simulation_time);

	this->SimRecord[index][0].simulation_time_increasment_per_tick = (this->SimRecord[index][0].simulation_time - this->SimRecord[index][1].simulation_time) / lost_ticks;

	/*calculate velocity by ourselves*/
	Vector velocity = this->SimRecord[index][0].origin - this->SimRecord[index][1].origin;
	/*divide through lost ticks to get the velocity per tick*/
	velocity /= lost_ticks;

	this->SimRecord[index][0].velocity = pPlayer->GetVelocity();//velocity;
}


static std::deque<CIncomingSequence>sequences;
static int lastincomingsequencenumber;
int Real_m_nInSequencenumber;

void CBacktrackHelper::UpdateIncomingSequences()
{
	if (!FakeLatency::ClientState || FakeLatency::ClientState == 0)
		return;
	DWORD ClientState = (DWORD)*(DWORD*)FakeLatency::ClientState;
	if (ClientState)
	{
		INetChannel *netchan = (INetChannel*)*(DWORD*)(ClientState + 0x9C);
		if (netchan)
		{
			if (netchan->m_nInSequenceNr > lastincomingsequencenumber)
			{
				lastincomingsequencenumber = netchan->m_nInSequenceNr;
				sequences.push_front(CIncomingSequence(netchan->m_nInReliableState, netchan->m_nOutReliableState, netchan->m_nInSequenceNr, g_pGlobals->realtime));
			}

			if (sequences.size() > 2048)
				sequences.pop_back();
		}
	}
}

void CBacktrackHelper::ClearIncomingSequences()
{
	sequences.clear();
}

void CBacktrackHelper::AddLatencyToNetchan(INetChannel *netchan, float Latency)
{
	for (auto& seq : sequences)
	{
		if (g_pGlobals->realtime - seq.curtime >= Latency || g_pGlobals->realtime - seq.curtime > 1)
		{
			netchan->m_nInReliableState = seq.inreliablestate;
			netchan->m_nInSequenceNr = seq.sequencenr;
			break;
		}
	}
}