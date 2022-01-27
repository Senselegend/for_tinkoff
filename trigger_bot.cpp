#include "legit_bot.hpp"
#include "helping.hpp"




auto trigger_bot::Run(CBaseEntity* pLocalEntity, CUserCmd *pCmd) -> void
{
	CreateMove(pLocalEntity, pCmd);
}




auto trigger_bot::CreateMove(CBaseEntity* pLocalEntity, CUserCmd *pCmd) -> void
{
	auto CrossId = pLocalEntity->GetCrossId();
	auto t = pLocalEntity->GetWeapon()->get_wpn_type();
	auto id = pLocalEntity->GetWeapon()->GetItemDefinitionIndex();



	if (!Clientvariables->Triggerbot.WeaponConfig)
	{
		if ((GetAsyncKeyState(Clientvariables->Triggerbot.Key) || Clientvariables->Triggerbot.AutoFire) && Clientvariables->Triggerbot.Enable)
		{
			if (CrossId > 0 && CrossId <= 64)
			{
				auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

				if (!helping::EntityIsValid(CrossId))
					return;


				if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
					return;

				if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.FriendlyFire))
					pCmd->buttons |= IN_ATTACK;
			}
		}
	}
	else
	{
		if (id == WEAPON_DEAGLE)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.deagle.Key) || Clientvariables->Triggerbot.deagle.AutoFire) && Clientvariables->Triggerbot.deagle.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.deagle.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}

		if (id == WEAPON_DUALS)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.duals.Key) || Clientvariables->Triggerbot.duals.AutoFire) && Clientvariables->Triggerbot.duals.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.duals.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}

		if (id == WEAPON_FIVE7)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.five7.Key) || Clientvariables->Triggerbot.five7.AutoFire) && Clientvariables->Triggerbot.five7.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.five7.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}

		if (id == WEAPON_GLOCK)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.glock.Key) || Clientvariables->Triggerbot.glock.AutoFire) && Clientvariables->Triggerbot.glock.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.glock.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}

		if (id == WEAPON_CZ75)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.cz75.Key) || Clientvariables->Triggerbot.cz75.AutoFire) && Clientvariables->Triggerbot.cz75.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.cz75.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}

		if (id == WEAPON_P250)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.p250.Key) || Clientvariables->Triggerbot.p250.AutoFire) && Clientvariables->Triggerbot.p250.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.p250.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}

		if (id == WEAPON_P2000)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.p2000.Key) || Clientvariables->Triggerbot.p2000.AutoFire) && Clientvariables->Triggerbot.p2000.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.duals.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}

	/*	if (id == WEAPON_REVOLVER)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.r8.Key) || Clientvariables->Triggerbot.r8.AutoFire) && Clientvariables->Triggerbot.r8.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.r8.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}*/
		if (id == WEAPON_TEC9)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.tec9.Key) || Clientvariables->Triggerbot.tec9.AutoFire) && Clientvariables->Triggerbot.tec9.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.tec9.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_USPS)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.usp.Key) || Clientvariables->Triggerbot.usp.AutoFire) && Clientvariables->Triggerbot.usp.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.usp.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_XM1014)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.xm1014.Key) || Clientvariables->Triggerbot.xm1014.AutoFire) && Clientvariables->Triggerbot.xm1014.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.xm1014.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_MAG7)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.mag7.Key) || Clientvariables->Triggerbot.mag7.AutoFire) && Clientvariables->Triggerbot.mag7.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.mag7.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_NOVA)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.nova.Key) || Clientvariables->Triggerbot.nova.AutoFire) && Clientvariables->Triggerbot.nova.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.nova.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_SAWEDOFF)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.sawed_off.Key) || Clientvariables->Triggerbot.sawed_off.AutoFire) && Clientvariables->Triggerbot.sawed_off.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.sawed_off.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_MAC10)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.mac10.Key) || Clientvariables->Triggerbot.mac10.AutoFire) && Clientvariables->Triggerbot.mac10.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.mac10.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_P90)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.p90.Key) || Clientvariables->Triggerbot.p90.AutoFire) && Clientvariables->Triggerbot.p90.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.p90.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_UMP45)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.umo45.Key) || Clientvariables->Triggerbot.umo45.AutoFire) && Clientvariables->Triggerbot.umo45.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.umo45.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_MP7)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.mp7.Key) || Clientvariables->Triggerbot.mp7.AutoFire) && Clientvariables->Triggerbot.mp7.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.mp7.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_MP9)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.mp9.Key) || Clientvariables->Triggerbot.mp9.AutoFire) && Clientvariables->Triggerbot.mp9.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.mp9.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_BIZON)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.bizon.Key) || Clientvariables->Triggerbot.bizon.AutoFire) && Clientvariables->Triggerbot.bizon.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.bizon.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_AK47)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.ak47.Key) || Clientvariables->Triggerbot.ak47.AutoFire) && Clientvariables->Triggerbot.ak47.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.ak47.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_AUG)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.aug.Key) || Clientvariables->Triggerbot.aug.AutoFire) && Clientvariables->Triggerbot.aug.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.aug.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_FAMAS)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.famas.Key) || Clientvariables->Triggerbot.famas.AutoFire) && Clientvariables->Triggerbot.famas.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.famas.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_GALIL)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.galil.Key) || Clientvariables->Triggerbot.galil.AutoFire) && Clientvariables->Triggerbot.galil.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.galil.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_M4A1)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.m4a1.Key) || Clientvariables->Triggerbot.m4a1.AutoFire) && Clientvariables->Triggerbot.m4a1.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.m4a1.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_M4A1S)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.m4a1s.Key) || Clientvariables->Triggerbot.m4a1s.AutoFire) && Clientvariables->Triggerbot.m4a1s.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.m4a1s.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_SG553)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.sg553.Key) || Clientvariables->Triggerbot.sg553.AutoFire) && Clientvariables->Triggerbot.sg553.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.sg553.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_AWP)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.awp.Key) || Clientvariables->Triggerbot.awp.AutoFire) && Clientvariables->Triggerbot.awp.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.awp.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_G3SG1)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.g3sg1.Key) || Clientvariables->Triggerbot.g3sg1.AutoFire) && Clientvariables->Triggerbot.g3sg1.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.g3sg1.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_SCAR20)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.scar20.Key) || Clientvariables->Triggerbot.scar20.AutoFire) && Clientvariables->Triggerbot.scar20.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.scar20.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == SSG08)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.ssg080.Key) || Clientvariables->Triggerbot.ssg080.AutoFire) && Clientvariables->Triggerbot.ssg080.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.ssg080.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_M249)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.m249.Key) || Clientvariables->Triggerbot.m249.AutoFire) && Clientvariables->Triggerbot.m249.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.m249.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (id == WEAPON_NEGEV)
		{
			if ((GetAsyncKeyState(Clientvariables->Triggerbot.negev.Key) || Clientvariables->Triggerbot.negev.AutoFire) && Clientvariables->Triggerbot.negev.Enable)
			{
				if (CrossId > 0 && CrossId <= 64)
				{
					auto ChoosedPlayer = g_pEntitylist->GetClientEntity(CrossId);

					if (!helping::EntityIsValid(CrossId))
						return;


					if (!pLocalEntity->IsValid() || t == CBaseCombatWeapon::WeaponType::Knife || t == CBaseCombatWeapon::WeaponType::Grenade || t == CBaseCombatWeapon::WeaponType::Bomb)
						return;

					if ((ChoosedPlayer->GetTeamNum() != pLocalEntity->GetTeamNum()) || (ChoosedPlayer->GetTeamNum() == pLocalEntity->GetTeamNum() && Clientvariables->Triggerbot.negev.FriendlyFire))
						pCmd->buttons |= IN_ATTACK;
				}
			}
		}
	}
}