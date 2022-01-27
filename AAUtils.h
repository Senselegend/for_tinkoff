#pragma once
#include "sdk.h"
#include "global.h"
#include "math.h"
#include "GameUtils.h"

/*namespace BameWareFreestand
{
	inline float NormalizeYaw(float yaw)
	{
		if (yaw > 180)
			yaw -= (round(yaw / 360) * 360.f);
		else if (yaw < -180)
			yaw += (round(yaw / 360) * -360.f);

		return yaw;
	}

auto DoFreestanding = [is_moving, this](float& yaw, float& lby) -> bool
 {
	Vector view; g_pEngine->GetViewAngles(view);
	auto local_player = G::LocalPlayer;
	static constexpr int damage_tolerance = 10;

	std::vector<CBaseEntity*> enemies;

	/// Find the lowest fov enemy
	CBaseEntity* closest_enemy = nullptr;
	float lowest_fov = 360.f;
	for (int i = 0; i < 64; i++)
	{
		auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
		if (!entity || entity->GetIsDormant() || entity->GetTeam() == local_player->GetTeam() || entity->GetHealth() <= 0)
			continue;

		const float current_fov = fabs(NormalizeYaw(GameUtils::CalculateAngle(local_player->GetOrigin(), entity->GetOrigin()).y - viewangles.y));
		if (current_fov < lowest_fov)
		{
			lowest_fov = current_fov;
			closest_enemy = entity;
		}

		enemies.push_back(entity);
	}

	/// No enemies
	if (closest_enemy == nullptr)
		return false;

	const float at_target_yaw = GameUtils::CalculateAngle(local_player->GetOrigin(), closest_enemy->GetOrigin()).y;
	const float right_yaw = at_target_yaw - 90.f;
	const float left_yaw = at_target_yaw + 90.f;

	/// Misc functions
	auto CalcDamage = [local_player, enemies](Vector point) -> int
	{
		int damage = 0;
		for (auto& enemy : enemies)
		{
			damage += UTILS::Max<int>(FEATURES::RAGEBOT::autowall.CalculateDamage(enemy->GetVecOrigin() + Vector(0, 0, 64.f), point, enemy, local_player, 1).damage,
				FEATURES::RAGEBOT::autowall.CalculateDamage(enemy->GetVecOrigin() + Vector(0, 0, 49.f), point, enemy, local_player, 1).damage);
		}

		return damage;
	};
	auto RotateAndExtendPosition = [](Vector position, float yaw, float distance) -> Vector
	{
		Vector direction;
		Math::AngleVectors(Vector(0, yaw, 0), &direction);

		return position + (direction * distance);
	};
	auto RotateLBYAndYaw = [right_yaw, left_yaw, local_player](int right_damage, int left_damage, float lby_delta, float& yaw, float& lby, bool prefect_angle = false, bool symetrical = true) -> bool
	{
		const bool prefer_right = right_damage < left_damage;

		yaw = prefer_right ? right_yaw : left_yaw;
		if (symetrical)
			lby_delta = lby_delta * (prefer_right ? -1.f : 1.f);

		/// If not moving
		if (prefect_angle)
		{
			if (local_player->GetVelocity().Length2D() < 0.1f)
				yaw = UTILS::GetLBYRotatedYaw(yaw + lby_delta, yaw);
			else
				yaw = UTILS::GetLBYRotatedYaw(local_player->LowerBodyYaw(), yaw);
		}

		lby = yaw + lby_delta;

		if (fabs(NormalizeYaw(local_player->LowerBodyYaw() - lby)) < 35.f)
		{
			yaw = local_player->LowerBodyYaw() - lby_delta;
			lby = yaw + lby_delta;
		}

		return true;
	};
	auto DoBackFreestanding = [at_target_yaw, local_player](float& yaw, float& lby) -> bool
	{
		yaw = at_target_yaw + 180.f;
		lby = yaw;

		return true;
	};

	const auto head_position = local_player->GetOrigin() + Vector(0, 0, 74.f);
	const auto back_head_position = RotateAndExtendPosition(head_position, at_target_yaw + 180.f, 17.f);
	auto right_head_position = RotateAndExtendPosition(head_position, right_yaw, 17.f);
	auto left_head_position = RotateAndExtendPosition(head_position, left_yaw, 17.f);

	int right_damage = CalcDamage(right_head_position), left_damage = CalcDamage(left_head_position), back_damage = CalcDamage(back_head_position);

	/// too much damage to both sides
	if (right_damage > damage_tolerance && left_damage > damage_tolerance)
	{
		/// do backwards if valid
		if (back_damage < damage_tolerance)
			return DoBackFreestanding(yaw, lby);

		return false;
	}

	/// keep searching for a better angle
	if (right_damage == left_damage)
	{
		/// if on top of them, prioritise backwards antiaim
		if (MATH::NormalizePitch(GameUtils::CalculateAngle(local_player->GetOrigin(), closest_enemy->GetOrigin()).x) > 15.f && back_damage < damage_tolerance)
			return DoBackFreestanding(yaw, lby);

		/// do some traces a bit further out
		right_head_position = RotateAndExtendPosition(head_position, right_yaw, 50.f);
		left_head_position = RotateAndExtendPosition(head_position, left_yaw, 50.f);

		right_damage = CalcDamage(right_head_position), left_damage = CalcDamage(left_head_position);
		if (right_damage == left_damage)
		{
			/// just return the side closest to a wall
			right_head_position = UTILS::TraceToEnd(head_position, RotateAndExtendPosition(head_position, right_yaw, 17.f));
			left_head_position = UTILS::TraceToEnd(head_position, RotateAndExtendPosition(head_position, left_yaw, 17.f));

			float distance_1, distance_2;
			SDK::trace_t trace;
			SDK::Ray_t ray;
			SDK::CTraceWorldOnly filter;
			const auto end_pos = closest_enemy->GetVecOrigin() + Vector(0, 0, 64.f);

			/// right position
			ray.Init(right_head_position, end_pos);
			INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);
			distance_1 = (right_head_position - trace.end).Length();

			/// left position
			ray.Init(left_head_position, end_pos);
			INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);
			distance_2 = (left_head_position - trace.end).Length();

			if (fabs(distance_1 - distance_2) > 15.f)
			{
				RotateLBYAndYaw(distance_1, distance_2, SETTINGS::ragebot_configs.antiaim_edging_lby_delta, yaw, lby);

				if ((is_moving && !UTILS::IsPressingMovementKeys(reinterpret_cast<SDK::CUserCmd*>(GLOBAL::last_cmd))) ||
					(UTILS::GetCurtime() - m_last_move_time < 0.22 && !is_moving))
					yaw = lby + 60.f;

				return true;
			}

			return DoBackFreestanding(yaw, lby);
		}
		else
		{
			RotateLBYAndYaw(right_damage, left_damage, SETTINGS::ragebot_configs.antiaim_edging_lby_delta, yaw, lby);

			if ((is_moving && !UTILS::IsPressingMovementKeys(reinterpret_cast<CUserCmd*>(GLOBAL::last_cmd))) ||
				(UTILS::GetCurtime() - m_last_move_time < 0.22 && !is_moving))
				yaw = lby + 60.f;

			return true;
		}
	}
}*/
namespace OldFreestanding
{
	Vector get_hitbox_location(CBaseEntity* obj, int hitbox_id) {
		matrix3x4 bone_matrix[128];

		if (obj->SetupBones(bone_matrix, 128, 0x00000100, 0.0f)) {
			if (obj->GetModel()) {
				auto studio_model = g_pModelInfo->GetStudioModel(obj->GetModel());
				if (studio_model) {
					auto hitbox = studio_model->pHitboxSet(0)->pHitbox(hitbox_id);
					if (hitbox) {
						auto min = Vector{}, max = Vector{};

						Math::vector_transform(hitbox->bbmin, bone_matrix[hitbox->bone], min);
						Math::vector_transform(hitbox->bbmax, bone_matrix[hitbox->bone], max);

						return (min + max) / 2.0f;
					}
				}
			}
		}
		return Vector{};
	}


	bool is_viable_target(CBaseEntity* pEntity)
	{
		//ClientClass* pClass = (ClientClass*)pEntity->GetClientClass();
		if (!pEntity) return false;
		//if (pClass->m_ClassID != (int)CSGOClassID::CCSPlayer) return false;
		if (pEntity == G::LocalPlayer) return false;
		if (pEntity->GetTeamNum() == G::LocalPlayer->GetTeamNum()) return false;
		if (pEntity->IsProtected()) return false;
		if (!pEntity->isAlive() || pEntity->IsDormant()) return false;
		return true;
	}


	void normalize(Vector &vIn, Vector &vOut)
	{
		float flLen = vIn.Length();
		if (flLen == 0) {
			vOut.Init(0, 0, 1);
			return;
		}
		flLen = 1 / flLen;
		vOut.Init(vIn.x * flLen, vIn.y * flLen, vIn.z * flLen);
	}
	int aa_indicator = 0;

	void freeStand()
	{

		static float last_autodirect_angle;
		if (G::LocalPlayer->GetVelocity().Length() < 300) {

			auto fov_to_player = [](Vector view_offset, Vector view, CBaseEntity* m_entity, int hitbox)
			{
				CONST FLOAT MaxDegrees = 180.0f;
				Vector Angles = view;
				Vector Origin = view_offset;
				Vector Delta(0, 0, 0);
				Vector Forward(0, 0, 0);
				Math::AngleVectors(Angles, Forward);
				Vector AimPos = get_hitbox_location(m_entity, hitbox);
				VectorSubtract(AimPos, Origin, Delta);
				normalize(Delta, Delta);
				FLOAT DotProduct = Forward.Dot(Delta);
				return (acos(DotProduct) * (MaxDegrees / PI));
			};

			int target = -1;
			float mfov = 50;

			Vector viewoffset = G::LocalPlayer->GetOrigin() + G::LocalPlayer->GetViewOffset();
			Vector view; g_pEngine->GetViewAngles(view);

			for (int i = 0; i < g_pGlobals->maxClients; i++) {
				CBaseEntity* m_entity = g_pEntitylist->GetClientEntity(i);

				if (is_viable_target(m_entity)) {

					float fov = fov_to_player(viewoffset, view, m_entity, 0);
					if (fov < mfov) {
						mfov = fov;
						target = i;
					}
				}
			}

			Vector at_target_angle;

			if (target)
			{
				auto m_entity = g_pEntitylist->GetClientEntity(target);

				if (is_viable_target(m_entity))
				{
					Vector head_pos_screen;

					if (GameUtils::WorldToScreen(m_entity->GetHedPos(), head_pos_screen))
					{

						GameUtils::calculate_angle(G::LocalPlayer->GetOrigin(), m_entity->GetOrigin(), at_target_angle);
						at_target_angle.x = 0;

						Vector src3D, dst3D, forward, right, up, src, dst;
						float back_two, right_two, left_two;

						trace_t tr;
						Ray_t ray, ray2, ray3, ray4, ray5;
						CTraceFilter filter;

						const Vector to_convert = at_target_angle;
						Math::AngleVectors(to_convert, &forward, &right, &up);

						filter.pSkip = G::LocalPlayer;
						src3D = G::LocalPlayer->GetEyePosition();
						dst3D = src3D + (forward * 384); //Might want to experiment with other numbers, incase you don't know what the number does, its how far the trace will go. Lower = shorter.

						ray.Init(src3D, dst3D);
						g_pEngineTrace->TraceRay_NEW(ray, MASK_SHOT, &filter, &tr);
						back_two = (tr.endpos - tr.start).Length();

						ray2.Init(src3D + right * 35, dst3D + right * 35);
						g_pEngineTrace->TraceRay_NEW(ray2, MASK_SHOT, &filter, &tr);
						right_two = (tr.endpos - tr.start).Length();

						ray3.Init(src3D - right * 35, dst3D - right * 35);
						g_pEngineTrace->TraceRay_NEW(ray3, MASK_SHOT, &filter, &tr);
						left_two = (tr.endpos - tr.start).Length();
						static int timer;
						static int timer1;
						if (left_two > right_two)
						{
							timer1 = 0;

							timer++;
							if (timer >= 15)
							{
								/*if (next_lby_update())
								G::UserCmd->viewangles.y += 90 + g_pVars->Antiaim.FreestandingDelta;
								else*/
								G::UserCmd->viewangles.y += 90;
								last_autodirect_angle = +90;
								aa_indicator = 1;
							}
							else
								aa_indicator = 0;


						}
						else if (right_two > left_two)
						{
							timer = 0;
							timer1++;
							if (timer1 >= 15)
							{
								/*if (next_lby_update())
								G::UserCmd->viewangles.y -= 90 + g_pVars->Antiaim.FreestandingDelta;
								else*/
								G::UserCmd->viewangles.y -= 90;
								last_autodirect_angle = -90;
								aa_indicator = 2;
							}
							else
								aa_indicator = 0;
							//Body should be left

						}
						else
						{
							aa_indicator = 0;
						}

					}
				}
				else
					aa_indicator = 0;


			}
			else
				aa_indicator = 0;
		}
	}
}
