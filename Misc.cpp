#include "sdk.h"
#include "Misc.h"
#include "global.h"
#include <chrono>
#include "GameUtils.h"
#include "Menu.h"
#include "Math.h"
#include "xor.h"
#include "Draw.h"
#include "Aimbot.h"
#include <chrono>
CMisc* g_Misc = new CMisc;
//using RevealAllFn = void(*)(int); Double V fix: Can't use this syntax with my VS version!
typedef void(*RevealAllFn)(int);
RevealAllFn fnReveal;
void CMisc::RankReveal()
{
	if (!Clientvariables->Visuals.Rank)
		return;

	if(!fnReveal)
		fnReveal = (RevealAllFn)Utilities::Memory::FindPattern(XorStr("client.dll"), (PBYTE)XorStr("\x55\x8B\xEC\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00"), XorStr("xxxxx????x????"));

	int iBuffer[1];

	if (G::UserCmd->buttons & IN_SCORE)
		fnReveal(iBuffer[1]);
}

void CMisc::Bunnyhop()
{
	if ((*G::LocalPlayer->GetFlags() & FL_ONGROUND) && G::walkbotenabled)
		G::UserCmd->buttons = IN_JUMP;

	if (Clientvariables->Misc.AutoJump/* && Cvar->FindVar("sv_enablebunnyhopping")->GetValue() != 1*/)
	{
		static auto bJumped = false;
		static auto bFake = false;
		if (!bJumped && bFake)
		{
			bFake = false;
			G::UserCmd->buttons |= IN_JUMP;
		}
		else if (G::UserCmd->buttons & IN_JUMP)
		{
			if (*G::LocalPlayer->GetFlags() & FL_ONGROUND)
			{
				bJumped = true;
				bFake = true;
			}
			else
			{
				G::UserCmd->buttons &= ~IN_JUMP;
				bJumped = false;
			}
		}
		else
		{
			bJumped = false;
			bFake = false;
		}
	}
}

float RightMovement;
bool IsActive;
float StrafeAngle;

static float GetTraceFractionWorldProps(Vector start, Vector end)
{
	Ray_t ray;
	trace_t tr;
	CTraceFilterWorldAndPropsOnly filter;

	ray.Init(start, end);
	g_pEngineTrace->TraceRay_NEW(ray, MASK_SOLID, &filter, &tr);

	return tr.fraction;
}

void Start(CUserCmd* pCmd)
{
	StrafeAngle = 0;
	IsActive = true;

	QAngle CurrentAngles;
	CurrentAngles.y = 0;
	CurrentAngles.x = 0;
	Vector Forward = CurrentAngles.Direction();
	Vector Right = Forward.Cross(Vector(0, 0, 1));
	Vector Left = Vector(-Right.x, -Right.y, Right.z);

	float LeftPath = GetTraceFractionWorldProps(G::LocalPlayer->GetAbsOrigin() + Vector(0, 0, 10), G::LocalPlayer->GetAbsOrigin() + Left * 450.f + Vector(0, 0, 10));
	float RightPath = GetTraceFractionWorldProps(G::LocalPlayer->GetAbsOrigin() + Vector(0, 0, 10), G::LocalPlayer->GetAbsOrigin() + Right * 450.f + Vector(0, 0, 10));

	RightMovement = 1;
}

void Strafe(CUserCmd* pCmd)
{
	Vector Velocity = G::LocalPlayer->GetVelocity();
	Velocity.z = 0;
	float Speed = Velocity.Length();
	if (Speed < 45) Speed = 45;
	if (Speed > 750) Speed = 750;

	float FinalPath = GetTraceFractionWorldProps(G::LocalPlayer->GetAbsOrigin() + Vector(0, 0, 10), G::LocalPlayer->GetAbsOrigin() + Vector(0, 0, 10) + Velocity / 2.0f);
	float DeltaAngle = RightMovement * fmax((275.0f / Speed) * (2.0f / FinalPath) * (128.0f / (1.7f / g_pGlobals->interval_per_tick)) * Clientvariables->Misc.Retrack, 2.0f);
	StrafeAngle += DeltaAngle;

	if (fabs(StrafeAngle) >= 360.0f)
	{
		StrafeAngle = 0.0f;
		IsActive = false;
	}
	else
	{
		pCmd->forwardmove = cos((StrafeAngle + 90 * RightMovement) * (M_PI / 180.0f)) * 450.f;
		pCmd->sidemove = sin((StrafeAngle + 90 * RightMovement) * (M_PI / 180.0f)) * 450.f;
	}
}

void CMisc::WalkBotCM(Vector& oldang)
{
	static bool active = false;
	static bool firstrun = true;
	bool walkbotting = false;
	float wbdistance = 0;

	if (GetAsyncKeyState(Clientvariables->Misc.WalkbotStart) & 0x1)
		active = !active;

	Vector localpos = G::LocalPlayer->GetAbsOrigin();

	if (GetAsyncKeyState(Clientvariables->Misc.WalkbotSet) & 0x1)
	{
		G::walkpoints.push_back(localpos);
		G::wbpoints++;
	}
	else if (GetAsyncKeyState(Clientvariables->Misc.WalkbotDelete) & 0x1)
	{
		if (G::walkpoints.size() > 0)
			G::walkpoints.pop_back();

		if (G::wbpoints > -1)
			G::wbpoints--;
	}

	if (G::NewRound)
		firstrun = true;

	if (!active)
	{
		G::wbcurpoint = 0;
		firstrun = true;
		G::walkbotenabled = false;
		return;
	}

	G::walkbotenabled = true;
	walkbotting = true;

	if (G::wbcurpoint > G::wbpoints)
		G::wbcurpoint = 0;

	if (G::wbpoints == -1)
		return;

	Vector point = G::walkpoints[G::wbcurpoint];
	wbdistance = fabs(Vector(localpos - point).Length2D());

	if (wbdistance < 25.f)
		G::wbcurpoint++;

	if (G::wbcurpoint > G::wbpoints)
		G::wbcurpoint = 0;

	if (G::wbpoints == -1)
		return;

	point = G::walkpoints[G::wbcurpoint];
	wbdistance = fabs(Vector(localpos - point).Length2D());

	if (G::wbcurpoint == 0 && firstrun == true)
	{
		float lowdist = wbdistance;

		for (int i = 0; i < G::wbpoints; i++)
		{
			Vector pt = G::walkpoints[i];
			float dist = fabs(Vector(localpos - pt).Length2D());

			if (dist < lowdist)
			{
				lowdist = dist;
				G::wbcurpoint = i;
				point = G::walkpoints[G::wbcurpoint];
				wbdistance = dist;
			}
		}

		firstrun = false;
	}

	static Vector lastang;

	Vector curang = GameUtils::CalculateAngle(G::LocalPlayer->GetEyePosition(), point);
	curang.x = 0.f;

	Math::NormalizeVector(curang);
	Math::ClampAngles(curang);
	lastang = curang;

	G::StrafeAngle = curang;
}

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
#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)
void CMisc::AutoStrafe()
{
	bool CircleStrafe = GetAsyncKeyState(Clientvariables->Misc.PrespeedKey);

	int MoveType = G::LocalPlayer->GetMoveType();

	if (!IsActive && CircleStrafe && !(MoveType == 8 || MoveType == 9))
		Start(G::UserCmd);

	if (CircleStrafe && IsActive)
	{
		Strafe(G::UserCmd);
		return;
	}
	else if (IsActive)
		IsActive = false;

	if (!Clientvariables->Misc.AutoStrafe)
		return;

	if (GetAsyncKeyState(VK_SPACE) || G::walkbotenabled)
	{
		/* AW*/
		static int old_yaw;

		auto get_velocity_degree = [](float length_2d)
		{
			auto tmp = RAD2DEG(atan(30.f / length_2d));

			if (CheckIfNonValidNumber(tmp) || tmp > 90.f)
				return 90.f;

			else if (tmp < 0.f)
				return 0.f;

			else
				return tmp;
		};

		//if (pLocal->GetMoveType() != MOVETYPE_WALK)
		//return;

		auto velocity = G::LocalPlayer->GetVelocity();
		velocity.z = 0;

		static auto flip = false;
		auto turn_direction_modifier = (flip) ? 1.f : -1.f;
		flip = !flip;

		if (*G::LocalPlayer->GetFlags() & FL_ONGROUND || G::LocalPlayer->GetMoveType() == MOVETYPE_LADDER)
			return;

		if (G::UserCmd->forwardmove > 0.f)
			G::UserCmd->forwardmove = 0.f;

		auto velocity_length_2d = velocity.Length2D();

		auto strafe_angle = RAD2DEG(atan(15.f / velocity_length_2d));

		if (strafe_angle > 90.f)
			strafe_angle = 90.f;

		else if (strafe_angle < 0.f)
			strafe_angle = 0.f;

		Vector Buffer(0, G::StrafeAngle.y - old_yaw, 0);
		Buffer.y = Math::NormalizeYaw(Buffer.y);

		int yaw_delta = Buffer.y;
		old_yaw = G::StrafeAngle.y;

		if (yaw_delta > 0.f)
			G::UserCmd->sidemove = -450.f;

		else if (yaw_delta < 0.f)
			G::UserCmd->sidemove = 450.f;

		auto abs_yaw_delta = abs(yaw_delta);

		if (abs_yaw_delta <= strafe_angle || abs_yaw_delta >= 30.f)
		{
			Vector velocity_angles;
			Math::VectorAngles(velocity, velocity_angles);

			Buffer = Vector(0, G::StrafeAngle.y - velocity_angles.y, 0);
			Buffer.y = Math::NormalizeYaw(Buffer.y);
			int velocityangle_yawdelta = Buffer.y;

			auto velocity_degree = get_velocity_degree(velocity_length_2d) * Clientvariables->Misc.Retrack; // retrack value, for teleporters

			if (velocityangle_yawdelta <= velocity_degree || velocity_length_2d <= 15.f)
			{
				if (-(velocity_degree) <= velocityangle_yawdelta || velocity_length_2d <= 15.f)
				{
					G::StrafeAngle.y += (strafe_angle * turn_direction_modifier);
					G::UserCmd->sidemove = 450.f * turn_direction_modifier;
				}

				else
				{
					G::StrafeAngle.y = velocity_angles.y - velocity_degree;
					G::UserCmd->sidemove = 450.f;
				}
			}

			else
			{
				G::StrafeAngle.y = velocity_angles.y + velocity_degree;
				G::UserCmd->sidemove = -450.f;
			}


		}




		G::UserCmd->buttons &= ~(IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK);

		if (G::UserCmd->sidemove <= 0.0)
			G::UserCmd->buttons |= IN_MOVELEFT;
		else
			G::UserCmd->buttons |= IN_MOVERIGHT;

		if (G::UserCmd->forwardmove <= 0.0)
			G::UserCmd->buttons |= IN_BACK;
		else
			G::UserCmd->buttons |= IN_FORWARD;


	}
}

typedef void(__fastcall* ClanTagFn)(const char*, const char*);
ClanTagFn dw_ClanTag;
void SetClanTag(const char* tag, const char* name)
{
	//static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15\x00\x00\x00\x00\x6A\x24\x8B\xC8\x8B\x30", "xxxxxxxxx????xxxxxx")); // 0x9AF10
	if (!dw_ClanTag)
		dw_ClanTag = reinterpret_cast<ClanTagFn>(FindPatternIDA("engine.dll", "53 56 57 8B DA 8B F9 FF 15"));

	if (dw_ClanTag)
		dw_ClanTag(tag, name);
}

void SetScrollClanTag(std::string Tag)
{
	//int TagLength = Tag.length();
	//std::string Whitespace;
	//for (int i = 0; i < TagLength; i++)
	//	Whitespace += " ";
	//std::string WhitespaceTag = Whitespace + Tag + Whitespace;
	//if (TickCounter / 1000.0f > TagLength * 2) 
	//clock_begin = std::chrono::high_resolution_clock::now();
	//std::string CurrentSubStr = WhitespaceTag.substr((int)(TickCounter / 1000.0f), TagLength);

	//SetClanTag(CurrentSubStr.c_str(), CurrentSubStr.c_str());
	SetClanTag(Tag.c_str(), Tag.c_str());
	//TickCounter = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - clock_begin).count();
}


void CMisc::HandleClantag()
{
	if (!Clientvariables->Misc.Clantag)
		return;
	static int iLastTime;

	float latency = g_pEngine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING) + g_pEngine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);

	if (int(g_pGlobals->curtime * 2 + latency) != iLastTime)
	{
		std::string Name = "Sourcecode $";
		SetScrollClanTag(Name.substr(0, int(g_pGlobals->curtime * 2 + latency) % (Name.length() + 1)));
		iLastTime = int(g_pGlobals->curtime * 2 + latency);
	}
}


void CMisc::FixCmd()
{
	if (Clientvariables->Misc.AntiUT)
	{
		G::UserCmd->viewangles.y = Math::NormalizeYaw(G::UserCmd->viewangles.y);
		Math::ClampAngles(G::UserCmd->viewangles);

		if (G::UserCmd->forwardmove > 450)
			G::UserCmd->forwardmove = 450;
		if (G::UserCmd->forwardmove < -450)
			G::UserCmd->forwardmove = -450;

		if (G::UserCmd->sidemove > 450)
			G::UserCmd->sidemove = 450;
		if (G::UserCmd->sidemove < -450)
			G::UserCmd->sidemove = -450;
	}
}



int LagCompBreak() {
	
	Vector velocity = G::LocalPlayer->GetVelocity();
	velocity.z = 0;
	float speed = velocity.Length();
	if (speed > 0.f) {
		auto distance_per_tick = speed *
			g_pGlobals->interval_per_tick;
		int choked_ticks = std::ceilf(65.f / distance_per_tick);
		return std::min<int>(choked_ticks, 14);
	}
	return 1;
}

int lagticks = 0;
int lagticksMax = 15;
bool CMisc::FakeLag()
{
	static int ticks = 0;
	const int max_choke = 15;
	const int min_choke = 1;
	static int tick_choke = 1;
	bool m = true;
	if (*G::LocalPlayer->GetFlags() & FL_ONGROUND)
	{
		if (!Clientvariables->Misc.FakelagOnground)
		{
			G::SendPacket = true;
			return false;
		}

/*		if (G::LocalPlayer->GetVelocity().Length() < 0.1f)
		{
			G::SendPacket = true;
			return false;
		}*/

		if (G::UserCmd->buttons & IN_ATTACK && g_Aimbot->fired_in_that_tick)
		{
			G::SendPacket = true;
			return false;
		}
	}

	switch (Clientvariables->Misc.FakelagMode)
	{
	case 0:
		G::chokedticks = Clientvariables->Misc.FakelagAmount;
		break;

	case 1:
		G::chokedticks = std::min<int>(static_cast<int>(std::ceilf(64 / (G::LocalPlayer->GetVelocity().Length() * g_pGlobals->interval_per_tick))), Clientvariables->Misc.FakelagAmount);
		break;

	case 2:
		G::chokedticks = LagCompBreak();
		break;

	case 3: 
	{
		static int time = 0;
		time ++;
		if (time >= 15)
			time = 0;
		G::chokedticks = time;
	}
		break;
	case 4:
	{
		static int Time = 0;
		static int swap;
		swap++;
		static int swap2;
		swap2++;
		static int swap3;
		swap3++;
		if (swap3 >= 100)
		{
			swap3 = 0;
			Time = Clientvariables->Misc.FakelagAmount;
		}
		if (swap2 >= 50)
		{
			swap2 = 0;
			Time = Clientvariables->Misc.FakelagAmount;
		}
		if (swap >= 10)
		{
			swap = 0;
			Time = Clientvariables->Misc.FakelagAmount;
		}
		else

			Time = 2;
		G::chokedticks = Time;
	}
	break;

	default:
		break;
	}
	if (ticks >= G::chokedticks)
	{
		ticks = 0;
		G::SendPacket = true;
	}
	else
	{
		G::SendPacket = false;
		ticks++;
	}
}

void CMisc::FixMovement()
{
	Vector vMove =Vector(G::UserCmd->forwardmove, G::UserCmd->sidemove, 0.0f);
	float flSpeed = vMove.Length();
	Vector qMove;
	Math::VectorAngles(vMove, qMove);
	float normalized = fmod(G::UserCmd->viewangles.y + 180.f, 360.f) - 180.f;
	float normalizedx = fmod(G::UserCmd->viewangles.x + 180.f, 360.f) - 180.f;
	float flYaw = DEG2RAD((normalized - G::StrafeAngle.y) + qMove.y);

	if (normalizedx >= 90.0f || normalizedx <= -90.0f || (G::UserCmd->viewangles.x >= 90.f && G::UserCmd->viewangles.x <= 200) || G::UserCmd->viewangles.x <= -90)
		G::UserCmd->forwardmove = -cos(flYaw) * flSpeed;
	else 
		G::UserCmd->forwardmove = cos(flYaw) * flSpeed;

	G::UserCmd->sidemove = sin(flYaw) * flSpeed;
}
void LoadPathing()
{

	char path[MAX_PATH];
	GetModuleFileNameA(GetModuleHandle(NULL), path, 255);
	for (int i = strlen(path); i > 0; i--)
	{
		if (path[i] == '\\')
		{
			path[i + 1] = 0;
			break;
		}
	}

	char size[8];
	char vecstr[64];
	char itostr[8];
	CBaseEntity *pLocal = G::LocalPlayer;
	if (!pLocal)
		return;
	if (g_Misc->path.size() > 1)
	{

		g_Misc->path.erase(g_Misc->path.begin(), g_Misc->path.end());
		sprintf_s(path, "\\%s_%s_%d.cfg", "pathes", "test"/*g_pEngine->GetLevelName()*/, pLocal->GetTeamNum());
		GetPrivateProfileStringA("Points", "Size", "0", size, 8, path);
		int numPoints = atoi(size);
		for (int i = 0; i < numPoints - 1; i++)
		{
			char vecstr[64];
			char itostr[8];
			sprintf_s(itostr, "%d", i);
			GetPrivateProfileStringA("Pathing", itostr, "0.0 0.0 0.0", vecstr, 64, path);
			std::string PosStr = vecstr;
			string buffer;
			stringstream ss(PosStr);
			vector<string> floats;
			while (ss >> buffer)
				floats.push_back(buffer);

			g_Misc->path.push_back(Vector(stof(floats[0]), stof(floats[1]), stof(floats[2])));
		}
	}
}

void SavePathing()
{

	char path[MAX_PATH];
	GetModuleFileNameA(GetModuleHandle(NULL), path, 255);
	for (int i = strlen(path); i > 0; i--)
	{
		if (path[i] == '\\')
		{
			path[i + 1] = 0;
			break;
		}
	}
	char size[8];
	char vecstr[64];
	char itostr[8];

	if (g_Misc->path.size() > 1)
	{
		sprintf_s(path, "\\%s_%s_%d.cfg", "pathes", "test"/*g_pEngine->GetLevelName()*/, G::LocalPlayer->GetTeamNum());
		printf("Path %s\n", path);
		sprintf_s(size, "%d", g_Misc->path.size() + 1);
		WritePrivateProfileStringA("Points", "Size", size, path);
		for (int i = 0; i < g_Misc->path.size(); i++)
		{
			sprintf_s(itostr, "%d", i);
			sprintf_s(vecstr, "%f %f %f", g_Misc->path.at(i).x, g_Misc->path.at(i).y, g_Misc->path.at(i).z);
			WritePrivateProfileStringA("Pathing", itostr, vecstr, path);
		}
	}
}
bool MarksIsVisible(CBaseEntity* local, Vector& vTo) 
{
	Ray_t ray;
	trace_t trace;
	CTraceFilterNoPlayer filter;
	filter.pSkip = local;

	ray.Init(local->GetEyePosition(), vTo);
	g_pEngineTrace->TraceRay(ray, 0x4600400B, &filter, &trace);
	return (trace.fraction > 0.99f);
}