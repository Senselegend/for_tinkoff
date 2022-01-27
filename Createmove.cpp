#include "hooks.h"
#include "global.h"
#include "Misc.h"
#include "Menu.h"
#include "BacktrackingHelper.h"
#include "Math.h"
#include "GameUtils.h"
#include "Aimbot.h"
#include "PredictionSystem.h"
#include "Antiaim.h"
#include "GrenadePrediction.h"
#include "legit_bot.hpp"
#include <Psapi.h>
static CPredictionSystem* Prediction = new CPredictionSystem();


void __declspec(naked) LagFix()
{
	__asm
	{
		push edi
		xchg dword ptr[esp], edi
		push eax
		mov eax, 77
		mov eax, dword ptr[esp]
		add esp, 4
		pop edi

		cmp esp, 0
		jne fixentity

		_emit 0x88
		_emit 0xFF

		invlpg dword ptr[eax]

		int 2

		fixentity:
		sub esp, 4
			mov dword ptr[esp], ebp

			call cleanup

			pop ebp

			ret

			cleanup :

		ret
	}
}

static int Ticks = 0;
static int LastReserve = 0;
namespace FakeLatency
{
	char *clientdllstr = new char[11]{ 25, 22, 19, 31, 20, 14, 84, 30, 22, 22, 0 }; /*client.dll*/
	char *enginedllstr = new char[11]{ 31, 20, 29, 19, 20, 31, 84, 30, 22, 22, 0 }; /*engine.dll*/
	char *cam_tofirstperson_sig = new char[51]{ 59, 75, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 56, 67, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 60, 60, 90, 90, 67, 74, 90, 90, 67, 74, 0 }; /*A1  ??  ??  ??  ??  B9  ??  ??  ??  ??  FF  90  90*/
	char *cam_tothirdperson_sig = new char[51]{ 59, 75, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 56, 67, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 60, 60, 90, 90, 67, 74, 90, 90, 66, 57, 0 }; /*A1  ??  ??  ??  ??  B9  ??  ??  ??  ??  FF  90  8C*/
	char *clientstatestr = new char[51]{ 66, 56, 90, 90, 73, 62, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 66, 59, 90, 90, 60, 67, 90, 90, 60, 73, 90, 90, 74, 60, 90, 90, 75, 75, 90, 90, 78, 79, 90, 90, 60, 57, 0 }; /*8B  3D  ??  ??  ??  ??  8A  F9  F3  0F  11  45  FC*/
	char *updateclientsideanimfnsigstr = new char[95]{ 79, 79, 90, 90, 66, 56, 90, 90, 63, 57, 90, 90, 79, 75, 90, 90, 79, 76, 90, 90, 66, 56, 90, 90, 60, 75, 90, 90, 66, 74, 90, 90, 56, 63, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 74, 74, 90, 90, 74, 74, 90, 90, 74, 74, 90, 90, 77, 78, 90, 90, 73, 76, 90, 90, 66, 56, 90, 90, 74, 76, 90, 90, 60, 60, 90, 90, 67, 74, 90, 90, 69, 69, 90, 90, 69, 69, 90, 90, 74, 74, 90, 90, 74, 74, 0 }; /*55  8B  EC  51  56  8B  F1  80  BE  ??  ??  00  00  00  74  36  8B  06  FF  90  ??  ??  00  00*/
	void DecStr(char *adr, const unsigned len)
	{
		for (unsigned i = 0; i < len; i++)
		{
			adr[i] ^= 50;
			adr[i] ^= 72;
		}
	}

	void EncStr(char *adr, const unsigned len)
	{
		for (unsigned i = 0; i < len; i++)
		{
			adr[i] ^= 72;
			adr[i] ^= 50;
		}
	}

	void DecStr(char *adr)
	{
		int len = strlen(adr);
		for (unsigned i = 0; i < len; i++)
		{
			adr[i] ^= 50;
			adr[i] ^= 72;
		}
	}

	void EncStr(char *adr)
	{
		int len = strlen(adr);
		for (unsigned i = 0; i < len; i++)
		{
			adr[i] ^= 72;
			adr[i] ^= 50;
		}
	}
	HANDLE FindHandle(std::string name)
	{
		return GetModuleHandle(name.c_str());
	}
	uintptr_t FindMemoryPattern(HANDLE ModuleHandle, char* strpattern, int length)
	{
		unsigned char *signature = new unsigned char[length + 1];
		bool *skippable = new bool[length + 1];
		int signaturelength = 0;
		for (int byteoffset = 0; byteoffset < length - 1; byteoffset += 2)
		{
			char charhex[4]; //4 to keep sscanf happy
			*(short*)charhex = *(short*)&strpattern[byteoffset];
			if (charhex[0] != ' ')
			{
				if (charhex[0] == '?')
				{
					signature[signaturelength] = '?';
					skippable[signaturelength] = true;
				}
				else
				{
					charhex[2] = NULL; //add null terminator
					signature[signaturelength] = (unsigned char)std::stoul(charhex, nullptr, 16);
					skippable[signaturelength] = false;
				}
				signaturelength++;
			}
		}
		int searchoffset = 0;
		int maxoffset = signaturelength - 1;


		MODULEINFO dllinfo;
		GetModuleInformation(GetCurrentProcess(), (HMODULE)ModuleHandle, &dllinfo, sizeof(MODULEINFO));
		DWORD endadr = (DWORD)ModuleHandle + dllinfo.SizeOfImage;
		DWORD adrafterfirstmatch = NULL;
		for (DWORD adr = (DWORD)ModuleHandle; adr < endadr; adr++)
		{
			if (skippable[searchoffset] || *(char*)adr == signature[searchoffset] || *(unsigned char*)adr == signature[searchoffset])
			{
				if (searchoffset == 0)
				{
					adrafterfirstmatch = adr + 1;
				}
				searchoffset++;
				if (searchoffset > maxoffset)
				{
					delete[] signature;
					delete[] skippable;
					//timetakentosearch = QPCTime() - startsearch;
					return adr - maxoffset; //FOUND OFFSET!
				}
			}
			else if (adrafterfirstmatch)
			{
				adr = adrafterfirstmatch;
				searchoffset = 0;
				adrafterfirstmatch = NULL;
			}
		}

		delete[] signature;
		delete[] skippable;
		return NULL; //NOT FOUND!
	}
	HANDLE EngineHandle = NULL;
	HANDLE ClientHandle = NULL;
	DWORD ClientState = NULL;
}

int __fastcall Hooked_SendDatagram(void* netchan, void*, void *datagram)
{

	INetChannel* chan = (INetChannel*)netchan;
	bf_write* data = (bf_write*)datagram;


	int instate = chan->m_nInReliableState;
	int insequencenr = chan->m_nInSequenceNr;

	if (Clientvariables->Ragebot.FakeLatency)
		g_BacktrackHelper->AddLatencyToNetchan(chan, Clientvariables->Ragebot.FakeLatencyAmount);

	int ret = HNetchan->GetOriginalMethod<SendDatagramFn>(48)(chan, data);

	chan->m_nInReliableState = instate;
	chan->m_nInSequenceNr = insequencenr;


	return ret;

}
bool __fastcall Hooks::CreateMove(void* thisptr, void*, float flInputSampleTime, CUserCmd* cmd)
{
	if (cmd)
	{

		if (!cmd->command_number)
			return true;


		g_pEngine->SetViewAngles(cmd->viewangles);
		QAngle org_view = cmd->viewangles;

		CBaseEntity* pLocalPlayer = g_pEntitylist->GetClientEntity(g_pEngine->GetLocalPlayer());
		if (pLocalPlayer)
		{
			g_BacktrackHelper->UpdateIncomingSequences();
			G::LocalPlayer = pLocalPlayer;
			G::UserCmd = cmd;
			G::UserCmdForBacktracking = cmd;
			CBasePlayerAnimState* Animations = G::LocalPlayer->GetBasePlayerAnimState();

			/*if (Animations->m_iLastClientSideAnimationUpdateFramecount)
				Animations->m_iLastClientSideAnimationUpdateFramecount--;

			if (Animations->m_flLastClientSideAnimationUpdateTime)
				Animations->m_flLastClientSideAnimationUpdateTime -= g_pGlobals->interval_per_tick;*/


			if (pLocalPlayer->GetHealth() > 0)
			{
				CBaseCombatWeapon* pWeapon = pLocalPlayer->GetWeapon();
				if (pWeapon)
				{
					PVOID pebp;
					__asm mov pebp, ebp;
					bool* pbSendPacket = (bool*)(*(PDWORD)pebp - 0x1C);
					bool& bSendPacket = *pbSendPacket;

					G::vecUnpredictedVel = G::LocalPlayer->GetVelocity();

					G::MainWeapon = pWeapon;
					G::WeaponData = pWeapon->GetCSWpnData();

					bool IsLadder = pLocalPlayer->GetMoveType() == MOVETYPE_LADDER;
					G::StrafeAngle = G::UserCmd->viewangles;

					g_Aimbot->DropTarget();



					if (Clientvariables->Misc.Walkbot)
						g_Misc->WalkBotCM(org_view);

					trigger_bot* g_pTriggerbot = new trigger_bot();
					g_pTriggerbot->Run(G::LocalPlayer, cmd);
					delete g_pTriggerbot;

					g_Misc->HandleClantag();
					g_Misc->Bunnyhop();
					g_Misc->AutoStrafe();

					if ((G::UserCmd->buttons & IN_ATTACK ||
						(G::UserCmd->buttons & IN_ATTACK2 && (G::MainWeapon->WeaponID() == REVOLVER || G::MainWeapon->IsKnife()))) && GameUtils::IsAbleToShoot())
						g_Aimbot->fired_in_that_tick = true;

					Prediction->EnginePrediction(cmd);

					g_Aimbot->Run();
					g_Aimbot->CompensateInaccuracies();
				
				
					if (Clientvariables->Misc.FakelagEnable)
						g_Misc->FakeLag();

					G::ForceRealAA = false;
					if (G::ChokedPackets >= 14)// breaks fakewalk
					{
						G::SendPacket = true;
						G::ChokedPackets = 0;
						G::ForceRealAA = true;
					}

					if (!IsLadder)
						g_Antiaim->Run(org_view);

					if ((G::UserCmd->buttons & IN_ATTACK || G::UserCmd->buttons & IN_ATTACK2 && G::MainWeapon->WeaponID() == REVOLVER) && (G::MainWeapon->IsPistol() || G::MainWeapon->WeaponID() == AWP || G::MainWeapon->WeaponID() == SSG08))
					{
						static bool bFlip = false;
						if (bFlip)
						{
							if (G::MainWeapon->WeaponID() == REVOLVER)
							{
							}
							else
								G::UserCmd->buttons &= ~IN_ATTACK;
						}
						bFlip = !bFlip;
					}



					if (G::SendPacket)
						G::FakeAngle = cmd->viewangles;
					else
						G::RealAngle = cmd->viewangles;

					if (G::SendPacket)
						G::ChokedPackets = 0;
					else
						G::ChokedPackets++;



				/*	if (Animations->m_iLastClientSideAnimationUpdateFramecount)
						Animations->m_iLastClientSideAnimationUpdateFramecount--;

					if (Animations->m_flLastClientSideAnimationUpdateTime)
						Animations->m_flLastClientSideAnimationUpdateTime -= g_pGlobals->interval_per_tick;*/


				

					if (Clientvariables->Ragebot.FakeLatency)
					{
						if (!HNetchan)
						{

							DWORD ClientState = *(DWORD*)FakeLatency::ClientState;

							if (ClientState)
							{
								DWORD NetChannel = *(DWORD*)(*(DWORD*)FakeLatency::ClientState + 0x9C);
								if (NetChannel)
								{
									HNetchan = new VMT((DWORD**)NetChannel);
									HNetchan->HookVM((void*)Hooked_SendDatagram, 48);
									HNetchan->ApplyVMT();
								}
							}

						}
					}


					G::LocalPlayer->GetClientSideAnimation();
					g_Misc->FixMovement();
					g_Misc->FixCmd(); //normalizes viewangles and clamps the movement values like upmove if a restriction is set

					cmd = G::UserCmd;
					bSendPacket = G::SendPacket;
					grenade_prediction::instance().Tick(G::UserCmd->buttons);
				}
			}
		}
	}
	return false;
}
