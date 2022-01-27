#include "legit_bot.hpp"
#include "sdk.h"
#include "global.h"
#include <windows.h>
#include "Math.h"
#include "GameUtils.h"

namespace Global = G;
float GetLastTimeStamp = 0.0f;



namespace helping
{

	Vector get_hitbox_location(CBaseEntity* obj, int hitbox_id)
	{
		matrix3x4 bone_matrix[128];

		if (obj->SetupBones(bone_matrix, 128, 0x00000100, 0))
		{
			if (obj->GetModel())
			{
				auto studio_model = g_pModelInfo->GetStudioModel(obj->GetModel());
				if (studio_model)
				{
					auto hitbox = studio_model->pHitboxSet(0)->pHitbox(hitbox_id);
					if (hitbox)
					{
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

	void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
	{
		out.x = in1.x * in2[0][0] + in1.y * in2[0][1] + in1.z * in2[0][2] + in2[0][3];
		out.y = in1.x * in2[1][0] + in1.y* in2[1][1] + in1.z * in2[1][2] + in2[1][3];
		out.z = in1.x * in2[2][0] + in1.y * in2[2][1] + in1.z * in2[2][2] + in2[2][3];
	}
	/*	Vector get_hitbox_location(CBaseEntity* pEntity, int Hitbox)
	{
	matrix3x4 matrix[128];

	if (!pEntity->SetupBones(matrix, 128, BONE_USED_BY_HITBOX, GetLastTimeStamp))
	return Vector(0, 0, 0);

	auto pModel = pEntity->GetModel();

	if (!pModel)
	return Vector(0, 0, 0);

	auto pStudioHdr = g_pModelInfo->GetStudioModel(pModel);

	if (!pStudioHdr)
	return Vector(0, 0, 0);

	auto pHitboxSet = pStudioHdr->pHitboxSet(pEntity->GetHitboxSet());

	auto hitbox = pHitboxSet->pHitbox(Hitbox);

	if (!hitbox)
	return Vector(0, 0, 0);

	Vector vMin = { 0,0,0 }, vMax = { 0,0,0 }, vCenter = { 0,0,0 }, sCenter = { 0,0,0 };
	VectorTransform(hitbox->bbmin, matrix[hitbox->bone], vMin);
	VectorTransform(hitbox->bbmax, matrix[hitbox->bone], vMax);
	vCenter = (vMin + vMax) *0.5f;
	return vCenter;
	}*/
	inline float FASTSQRT(float x)
	{
		unsigned int i = *(unsigned int*)&x;

		i += 127 << 23;
		i >>= 1;
		return *(float*)&i;
	}

	float Distance(Vector VecA, Vector VecB)
	{

		return FASTSQRT(pow(VecA.x - VecB.x, 2) + pow(VecA.y - VecB.y, 2) + pow(VecA.y - VecB.y, 2));
	}
	bool IsVisible_enemy(CBaseEntity* pEnemy)
	{
		CBaseEntity* pLocalEntity = g_pEntitylist->GetClientEntity(g_pEngine->GetLocalPlayer());
		if (!pLocalEntity)
			return false;


		Ray_t ray;
		trace_t tr;
		CTraceFilter traceFilter;

		traceFilter.pSkip = pEnemy;

		ray.Init(pLocalEntity->GetEyePosition(), pEnemy->GetEyePosition());

		g_pEngineTrace->TraceRay(ray, 0x46004003, &traceFilter, &tr);

		if (tr.m_pEnt == pEnemy || tr.fraction > 0.97f)
			return true;
		else
			return false;

	}

	float getHypotenuseFromCenter(Vector Point)
	{
		float PositionX = 0;
		float PositionY = 0;
		float ScreenCenterX, ScreenCenterY;
		Vector Screen = { 0,0,0 };

		static int width = 0, height = 0;
		g_pEngine->GetScreenSize(width, height);
		ScreenCenterX = width / 2;
		ScreenCenterY = height / 2;

		if (GameUtils::WorldToScreen(Point, Screen) == FALSE)
			return 9999.0f;


		PositionX = Screen.x > ScreenCenterX ? Screen.x - ScreenCenterX : ScreenCenterX - Screen.x;
		PositionY = Screen.y > ScreenCenterY ? Screen.y - ScreenCenterY : ScreenCenterY - Screen.y;

		return sqrt(PositionX * PositionX + PositionY * PositionY);
	}


	void VectorAngles(const Vector& forward, Vector &angles)
	{

		if (forward.y == 0.0f && forward.x == 0.0f)
		{
			angles[0] = (forward.z > 0.0f) ? 270.0f : 90.0f; // Pitch (up/down)
			angles.y = 0.0f;  //yaw left/right
		}
		else
		{
			angles[0] = atan2(-forward.z, forward.Length2D()) * -180 / M_PI;
			angles.y = atan2(forward.y, forward[0]) * 180 / M_PI;

			if (angles.y> 90) angles.y -= 180;
			else if (angles.y < 90) angles.y += 180;
			else if (angles.y == 90) angles.y = 0;
		}

		angles[2] = 0.0f;
	}

	void CA(Vector src, Vector dst, Vector &angles)
	{
		Vector delta = src - dst;
		double hyp = delta.Length2D(); //delta.Length
		angles.y = (atan(delta.y / delta.x) * 57.295779513082f);
		angles.x = (atan(delta.z / hyp) * 57.295779513082f);
		angles.z = 0.00;

		if (delta.x >= 0.0)
			angles.y += 180.0f;
	}

	Vector CalcAngle(Vector src, Vector dst)
	{
		Vector angles = { 0, 0, 0, };
		Vector delta = src - dst;
		VectorAngles(delta, angles);
		delta.Normalize();
		return angles;
	}



	auto GetFovs(Vector& viewAngle, Vector& aimAngle) -> float
	{
		Vector delta = aimAngle - viewAngle;
		delta.Normalized();
		return sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));
	}


	/*int ClosestBone(CBaseEntity* Entity, CUserCmd* cmd)
	{

	float BestDist = 360.f;
	int aimbone;

	VMatrix matrix[128];

	if (!Entity->SetupBones(matrix, 128, BONE_USED_BY_HITBOX, GetLastTimeStamp))
	return -1;

	auto pStudioModel = g_pModelInfo->GetStudioModel(Entity->GetModel());
	if (!pStudioModel)
	return -1;

	auto set = pStudioModel->pHitboxSet(Entity->GetHitboxSet());
	if (!set)
	return -1;

	for (int i = 0; i < set->numhitboxes; i++)
	{
	if (i == (int)CSGOHitboxID::RightThigh || i == (int)CSGOHitboxID::LeftThigh || i == (int)CSGOHitboxID::LeftFoot
	|| i == (int)CSGOHitboxID::RightShin || i == (int)CSGOHitboxID::LeftShin || i == (int)CSGOHitboxID::RightFoot
	|| i == (int)CSGOHitboxID::RightHand || i == (int)CSGOHitboxID::LeftHand || i == (int)CSGOHitboxID::RightUpperArm
	|| i == (int)CSGOHitboxID::RightLowerArm || i == (int)CSGOHitboxID::LeftUpperArm || i == (int)CSGOHitboxID::LeftLowerArm)
	continue;

	auto hitbox = set->pHitbox(i);

	if (!hitbox)
	continue;



	auto thisdist = fov_get::GetFov(cmd->viewangles, CalcAngle(Global::LocalPlayer->GetEyePosition(), get_hitbox_location(Entity, hitbox->bone)));
	if (BestDist > thisdist)
	{
	BestDist = thisdist;
	aimbone = hitbox->bone;
	continue;
	}
	}
	return aimbone;
	//}



	}*/
	bool EntityIsValid(int i)
	{
		auto* pEntity = g_pEntitylist->GetClientEntity(i);

		if (!pEntity)
			return false;

		if (pEntity->IsDormant())
			return false;

		if (pEntity->GetHealth() <= 0)
			return false;

		if (!pEntity->isAlive())
			return false;

		//if (pEntity->HasGunGameImmunity())
		//return false;

		if (pEntity == G::LocalPlayer)
			return false;


		return true;
	}
}