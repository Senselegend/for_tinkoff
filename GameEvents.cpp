#include "hooks.h"
#include <time.h>
#include "Mmsystem.h"
#include <thread>
#include "Hitmarker.h"
#include "global.h"
#include "Menu.h"
#include "controls.h"
#include "ESP.h"
#include "Math.h"
#include "bullet_log.h"
#include "event_log.h"
#include "AW_hitmarker.h"
#include "sounds.h"
#pragma comment(lib, "winmm.lib") 

string prefix = "";

std::vector<impact_info> impacts;
std::vector<hitmarker_info> hitmarkers;

#define EVENT_HOOK( x )
#define TICK_INTERVAL			(g_pGlobals->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

cGameEvent g_Event;

std::vector <CMessage> Eventlog::messages;

std::vector<cbullet_tracer_info> logs;

std::vector<trace_info> trace_logs;

CBaseEntity* get_player(int userid) {
	int i = g_pEngine->GetPlayerForUserID(userid);
	return g_pEntitylist->GetClientEntity(i);
}

int showtime = 3;

namespace Eventlog
{
	void Msg(string str, bool red)
	{
		Color clr = red ? Color(165, 165, 165, 255) : Color(255, 255, 255, 255);
		messages.push_back(CMessage(prefix + str, G::LocalPlayer->GetTickBase() * g_pGlobals->interval_per_tick, clr));
		g_pEngine->ClientCmd_Unrestricted(("echo " /*+ prefix*/ + str).c_str());
	}

	void Draw()
	{
		for (int i = 0; i < messages.size(); i++)
		{
			CMessage cur = messages.at(i);
			float curtime = g_pGlobals->curtime;
			if (fabs(curtime - cur.time) > 5) {
				messages.erase(messages.begin() + i);
			}
			g_Draw.StringA(g_Draw.event_font, false, 15, 10 + (15 * i), cur.clr.r(), cur.clr.g(), cur.clr.b(), cur.alpha, (char*)cur.str.c_str());
		}
	}
}


char* HitgroupToName(int hitgroup)
{
	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		return "head";
	case HITGROUP_LEFTLEG:
		return "left leg";
	case HITGROUP_RIGHTLEG:
		return "right leg";
	case HITGROUP_STOMACH:
		return "stomach";
	default:
		return "body";
	}
}

player_info_t GetInfo(int Index)
{
	player_info_t Info;
	g_pEngine->GetPlayerInfo(Index, &Info);
	return Info;
}

void Hurt(IGameEvent* Event)
{
	int attackerid = Event->GetInt("attacker");
	int entityid = g_pEngine->GetPlayerForUserID(attackerid);
	if (entityid == g_pEngine->GetLocalPlayer())
	{
		int nUserID = Event->GetInt("attacker");
		int nDead = Event->GetInt("userid");
		if (nUserID || nDead)
		{
			player_info_t killed_info = GetInfo(g_pEngine->GetPlayerForUserID(nDead));
			player_info_t killer_info = GetInfo(g_pEngine->GetPlayerForUserID(nUserID));

			std::string _1 = "Hit ";
			std::string _2 = killed_info.m_szPlayerName;
			std::string _3 = " at ";
			std::string _4 = HitgroupToName(Event->GetInt("hitgroup"));
			std::string _5 = " for ";
			std::string _6 = Event->GetString("dmg_health");
			std::string _7 = " damage";
			std::string _8 = " (";
			std::string _9 = Event->GetString("health");
			std::string _10 = " health remaining)";
			string out = _1 + _2 + _3 + _4 + _5 + _6 + _7 + _8 + _9 + _10;

			if (Event->GetInt("health") > 0)
				Eventlog::Msg(out, false);
			else
			{
				std::string _1 = "Killed ";
				std::string _2 = killed_info.m_szPlayerName;
				std::string _3 = " at ";
				std::string _4 = HitgroupToName(Event->GetInt("hitgroup"));
				string out = _1 + _2 + _3 + _4;
				Eventlog::Msg(out, true);
			}
		}
	}
}

void cGameEvent::FireGameEvent(IGameEvent *event)
{
	/*short   m_nUserID		user ID who was hurt
	short	attacker	user ID who attacked
	byte	health		remaining health points
	byte	armor		remaining armor points
	string	weapon		weapon name attacker used, if not the world
	short	dmg_health	damage done to health
	byte	dmg_armor	damage done to armor
	byte	hitgroup	hitgroup that was damaged*/
	const char* szEventName = event->GetName();
	if (!szEventName)
		return;

	if (!strcmp(szEventName, "round_start"))
		G::NewRound = true;
	else
		G::NewRound = false;

	if (strcmp(szEventName, "player_connect") == 0)
	{
		memset(&G::Shots, 0, sizeof(G::Shots)); // reset shotsfired	
	}


	if (Clientvariables->Visuals.DamageIndicators)
	{
		if (strcmp(szEventName, "player_hurt") == 0)
		{
			auto attacker = event->GetInt("attacker");
			auto dmgdone = event->GetInt("dmg_health");
			auto Hurt = event->GetInt("userid");
			CBaseEntity* pEnt = g_pEntitylist->GetClientEntity(g_pEngine->GetPlayerForUserID(Hurt));
			if (g_pEngine->GetPlayerForUserID(attacker) == g_pEngine->GetLocalPlayer()) {
				G::DamageDealt = dmgdone;
				G::DamageHit.push_back(FloatingText(pEnt, 1.f, dmgdone));


			}
		}
	}

	if (strcmp(szEventName, "player_hurt") == 0)
	{
		Hurt(event);
	}



	if (Clientvariables->Visuals.Hitmarker)
	{
		if (strcmp(szEventName, "player_hurt") == 0)
		{
			auto attacker = get_player(event->GetInt("attacker"));
			auto victim = get_player(event->GetInt("userid"));

			if (!attacker || !victim || attacker != G::LocalPlayer)
				return;

			Vector enemypos = victim->GetOrigin();
			impact_info best_impact;
			float best_impact_distance = -1;
			float time = g_pGlobals->curtime;

			for (int i = 0; i < impacts.size(); i++)
			{
				auto iter = impacts[i];
				if (time > iter.time + 1.f)
				{
					impacts.erase(impacts.begin() + i);
					continue;
				}
				Vector position = Vector(iter.x, iter.y, iter.z);
				float distance = position.DistTo(enemypos);
				if (distance < best_impact_distance || best_impact_distance == -1)
				{
					best_impact_distance = distance;
					best_impact = iter;
				}
			}
			if (best_impact_distance == -1)
				return;

			hitmarker_info info;
			info.impact = best_impact;
			info.alpha = 255;

			hitmarkers.push_back(info);

			int Attacker = event->GetInt("attacker");
			int UserID = event->GetInt("userid");

			CBaseEntity* Entity = g_pEntitylist->GetClientEntity(g_pEngine->GetPlayerForUserID(UserID));

			if (g_pEngine->GetPlayerForUserID(Attacker) == g_pEngine->GetLocalPlayer())
			{
				G::playerhurtcalled = true;
				PlaySoundA(hitmarker_sound2, nullptr, SND_MEMORY | SND_ASYNC | SND_NOSTOP);
			}
		}
	}

	if (Clientvariables->Misc.buybot1 == 1)
	{
		if (strcmp(event->GetName(), "round_start") == 0)
			g_pEngine->ClientCmd_Unrestricted("buy scar20; buy g3sg1");
	}
	else if (Clientvariables->Misc.buybot1 == 2)
	{
		if (strcmp(event->GetName(), "round_start") == 0)
			g_pEngine->ClientCmd_Unrestricted("buy awp");
	}
	else if (Clientvariables->Misc.buybot1 == 3)
	{
		if (strcmp(event->GetName(), "round_start") == 0)
			g_pEngine->ClientCmd_Unrestricted("buy ssg08");
	}
	else if (Clientvariables->Misc.buybot1 == 4)
	{
		if (strcmp(event->GetName(), "round_start") == 0)
			g_pEngine->ClientCmd_Unrestricted("buy negev");
	}

	if (Clientvariables->Misc.buybot2 == 1)
	{
		if (strcmp(event->GetName(), "round_start") == 0)
			g_pEngine->ClientCmd_Unrestricted("buy elite");
	}
	else if (Clientvariables->Misc.buybot2 == 2)
	{
		if (strcmp(event->GetName(), "round_start") == 0)
			g_pEngine->ClientCmd_Unrestricted("buy deagle");
	}
	else if (Clientvariables->Misc.buybot2 == 3)
	{
		if (strcmp(event->GetName(), "round_start") == 0)
			g_pEngine->ClientCmd_Unrestricted("buy p250");
	}



	if (Clientvariables->Misc.buybot3 == 1)
	{
		if (strcmp(event->GetName(), "round_start") == 0)
			g_pEngine->ClientCmd_Unrestricted("buy smokegrenade; buy hegrenade; buy molotov; buy incgrenade; buy vest; buy vesthelm; buy defuser");
	}




	if (strstr(event->GetName(), "bullet_impact"))
	{
		CBaseEntity* ent = get_player(event->GetInt("userid"));
		if (!ent || ent != G::LocalPlayer)
			return;

		impact_info info;
		info.x = event->GetFloat("x");
		info.y = event->GetFloat("y");
		info.z = event->GetFloat("z");

		info.time = g_pGlobals->curtime;

		impacts.push_back(info);
	}

	//if we receive bullet_impact event
	if (strcmp(szEventName, "bullet_impact") == 0)
	{
		auto* index = g_pEntitylist->GetClientEntity(g_pEngine->GetPlayerForUserID(event->GetInt("userid")));

		if (G::LocalPlayer)
		{
			Vector position(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));
			G::weaponfirecalled = true;
			if (index)

				trace_logs.push_back(trace_info(index->GetEyePosition(), position, g_pGlobals->curtime, event->GetInt("userid")));
		}
	}
}
int cGameEvent::GetEventDebugID()
{
	return 42;
}

void cGameEvent::RegisterSelf()
{
	g_pGameEventManager->AddListener(this, "player_connect", false);
	g_pGameEventManager->AddListener(this, "player_hurt", false);
	g_pGameEventManager->AddListener(this, "round_start", false);
	g_pGameEventManager->AddListener(this, "round_end", false);
	g_pGameEventManager->AddListener(this, "player_death", false);
	g_pGameEventManager->AddListener(this, "weapon_fire", false);
	g_pGameEventManager->AddListener(this, "bullet_impact", false);
}

void cGameEvent::Register()
{
	EVENT_HOOK(FireEvent);
}
