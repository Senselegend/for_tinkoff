#include "sdk.h"
#include "Math.h"
#include "global.h"
#include "GameUtils.h"
#include "xor.h"

CBaseCombatWeapon* CBaseEntity::GetWeapon()
{
	ULONG WeaponUlong = *(PULONG)((DWORD)this + offys.m_hActiveWeapon); // hActiveWeapon
	return (CBaseCombatWeapon*)(g_pEntitylist->GetClientEntityFromHandle(WeaponUlong));
}

DWORD GetCSWpnDataAddr;

bool CBaseEntity::IsEnemy()
{
	return this->GetTeamNum() != (reinterpret_cast< CBaseEntity* >(g_pEntitylist->GetClientEntity(g_pEngine->GetLocalPlayer())))->GetTeamNum();
}

void CBaseEntity::CopyPoseParameters(float* dest)
{
	float* flPose = (float*)((DWORD)this + 0x2764);
	memcpy(dest, flPose, sizeof(float) * 24);

}

bool CBaseEntity::ComputeHitboxSurroundingBox(Vector *pVecWorldMins, Vector *pVecWorldMaxs)
{
	typedef bool(__thiscall* SComputeHitboxSurroundingBox)(void*, Vector *, Vector *);
	static auto oComputeHitboxSurroundingBox = (SComputeHitboxSurroundingBox)(offys.dwComputeHitboxSurroundingBox);

	return oComputeHitboxSurroundingBox(this, pVecWorldMins, pVecWorldMaxs);
}
void CBaseEntity::InvalidateBoneCache()
{
//	static DWORD m_dwOcclusionArray = *(uintptr_t*)(FindPattern("client.dll", (PBYTE) "\xA1\x00\x00\x00\x00\x8B\xB7\x00\x00\x00\x00\x89\x75\xF8", "x????xx????xxx") + 0x1); //"A1 ? ? ? ? 8B B7 ? ? ? ? 89 75 F8"
	unsigned long g_iModelBoneCounter = **(unsigned long**)(offys.InvalidateBoneCache + 10);
	*(unsigned int*)((DWORD)this + 0x2914) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
	*(unsigned int*)((DWORD)this + 0x2680) = (g_iModelBoneCounter - 1); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
}
void CBaseEntity::SetCurrentCommand(CUserCmd *cmd)
{
	static int offset = g_pNetVars->GetOffset("DT_BasePlayer", "m_hConstraintEntity");
	*Member<CUserCmd**>(this, (offset - 0xC)) = cmd;
}


std::array<float, 24> &CBaseEntity::m_flPoseParameter()
{
	static int _m_flPoseParameter = g_pNetVars->GetOffset("CBaseAnimating", "m_flPoseParameter");
	return *(std::array<float, 24>*)((uintptr_t)this + _m_flPoseParameter);
}

bool CBaseEntity::IsValidRenderable() {

	if (!this || this == nullptr || G::LocalPlayer == nullptr)
		return false;

	if (this == G::LocalPlayer)
		return false;

	if (this->GetTeamNum() == G::LocalPlayer->GetTeamNum())
		return false;

	if (this->IsDormant())
		return false;

	if (!this->isAlive())
		return false;

	return true;
}

void CBaseEntity::SetAbsOrigin(const Vector &origin)
{
	using SetAbsOriginFn = void(__thiscall*)(void*, const Vector &origin);
	static SetAbsOriginFn SetAbsOrigin = (SetAbsOriginFn)FindPatternIDA("client.dll", "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8");
	SetAbsOrigin(this, origin);
}

bool CBaseEntity::IsValidTarget() {

	if (!this || this == nullptr)
		return false;

	ClientClass* pClass = (ClientClass*)this->GetClientClass(); // Needed to check clientclass after nullptr check that was causing a crash

	if (this == G::LocalPlayer)
		return false;

	if (pClass->m_ClassID != 35)
		return false;

	if (this->GetTeamNum() == G::LocalPlayer->GetTeamNum())
		return false;

	if (this->IsDormant())
		return false;

	if (!this->isAlive())
		return false;

	if (this->IsProtected())
		return false;

	return true;
}

void CBaseEntity::SetAngle2(Vector wantedang)
{
	typedef void(__thiscall* SetAngleFn)(void*, const Vector &);
	static SetAngleFn SetAngle = (SetAngleFn)((DWORD)Utilities::Memory::FindPatternIDA("client.dll", "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1"));
	SetAngle(this, wantedang);
}

bool CBaseEntity::SetupBones2(matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	static DWORD m_nWritableBones = g_pNetVars->GetOffset("DT_BaseAnimating", "m_nForceBone");
	static DWORD m_dwOcclusionArray = *(uintptr_t*)(FindPattern("client.dll", (PBYTE) "\xA1\x00\x00\x00\x00\x8B\xB7\x00\x00\x00\x00\x89\x75\xF8", "x????xx????xxx") + 0x1); //"A1 ? ? ? ? 8B B7 ? ? ? ? 89 75 F8"
	static DWORD m_bDidCheckForOcclusion = *(uintptr_t*)(FindPattern("client.dll", (PBYTE) "\xA1\x00\x00\x00\x00\x8B\xB7\x00\x00\x00\x00\x89\x75\xF8", "x????xx????xxx") + 0x7); //"A1 ? ? ? ? 8B B7 ? ? ? ? 89 75 F8"

	*(int*)((uintptr_t)this + m_nWritableBones) = 0;
	*(int*)((uintptr_t)this + m_bDidCheckForOcclusion) = reinterpret_cast< int* >(m_dwOcclusionArray)[1];

	__asm
	{
		mov edi, this
		lea ecx, dword ptr ds : [edi + 0x4]
		mov edx, dword ptr ds : [ecx]
		push currentTime
		push boneMask
		push nMaxBones
		push pBoneToWorldOut
		call dword ptr ds : [edx + 0x34]
	}

	//typedef bool(__thiscall* oSetupBones)(PVOID, matrix3x4*, int, int, float);
	//return getvfunc< oSetupBones>(this, 13)(this, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
}

bool CBaseEntity::canHit(Vector end, CBaseEntity* ent) {
	Ray_t ray;
	trace_t tr;
	CTraceFilter traceFilter;
	traceFilter.pSkip = this;
	ray.Init(this->GetEyePosition(), end);
	g_pEngineTrace->ClipRayToCBaseEntity(ray, MASK_SHOT, ent, &tr); // ignore grate

	if (!tr.m_pEnt)
		return false;

	CBaseEntity *pEnt = (CBaseEntity*)tr.m_pEnt;

	if (pEnt->GetTeamNum() != this->GetTeamNum())
		return true;

	return false;
}

Vector CBaseEntity::GetBonePos(int i)
{
	VMatrix boneMatrix[128];
	if (this->SetupBones(boneMatrix, 128, BONE_USED_BY_HITBOX, g_pGlobals->curtime))
	{
		return Vector(boneMatrix[i][0][3], boneMatrix[i][1][3], boneMatrix[i][2][3]);
	}
	return Vector(0, 0, 0);
}

Vector CBaseEntity::GetHedPos()
{
	return this->GetBonePos(6);

}
CSWeaponInfo* CBaseCombatWeapon::GetCSWpnData()
{

	typedef CSWeaponInfo*(__thiscall* OriginalFn)(void*);
	return  CallVFunction<OriginalFn>(this, 445)(this);
}
#define TIME_TO_TICKS( dt )	( ( int )( 0.5f + ( float )( dt ) / g_pGlobals->interval_per_tick ) )
int CBaseEntity::GetChockedPackets()
{
	if (GetSimulationTime() > GetOldSimulationTime())
		return TIME_TO_TICKS(fabs(GetSimulationTime() - GetOldSimulationTime()));
	return 0;
}
Vector& CBaseEntity::m_vecNetworkOrigin() 
{
	
	static int offset = g_pNetVars->GetOffset("DT_CSPlayer", "m_flFriction") - sizeof(Vector);
	return *(Vector*)((DWORD)this + offset);
}

float CBaseEntity::GetOldSimulationTime()
{

		static uintptr_t offset = g_pNetVars->GetOffset("DT_CSPlayer", "m_flSimulationTimen") +0x4;
	return *(float*)((DWORD)this + offset);
}
bool CBaseEntity::SetupBones(VMatrix *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	//void *pRenderable = (void*)(this + 0x4);
	//typedef bool(__thiscall* OriginalFn)(PVOID, VMatrix*, int, int, float);
	//return CallVFunction<OriginalFn>(pRenderable, 13)(pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

	__asm
	{
		mov edi, this
		lea ecx, dword ptr ds : [edi + 0x4]
		mov edx, dword ptr ds : [ecx]
		push currentTime
		push boneMask
		push nMaxBones
		push pBoneToWorldOut
		call dword ptr ds : [edx + 0x34]
	}
}

bool CBaseEntity::IsTargettingLocal()
{

	Vector src, dst, forward;
	trace_t tr;

	if (!this || !G::LocalPlayer || G::LocalPlayer->GetHealth() < 0)
		return false;

	Vector viewangle = this->GetEyeAngles();

	Math::AngleVectors(viewangle, &forward);
	forward *= 8142.f;
	src = this->GetEyePosition();
	dst = src + forward;

	Ray_t ray;
	ray.Init(src, dst);
	CTraceCBaseEntity filter;
	filter.pHit = G::LocalPlayer;

	g_pEngineTrace->TraceRay_NEW(ray, MASK_SHOT, &filter, &tr);
	

	if (tr.m_pEnt && tr.m_pEnt->GetTeamNum() != this->GetTeamNum()/*== G::LocalPlayer*/)
		return true;

	return false;
}
bool CBaseEntity::IsPlayer()
{
	ClientClass* pClass = (ClientClass*)this->GetClientClass();
	return pClass->m_ClassID == 35;
}

bool CBaseCombatWeapon::IsReloadingVisually() {
	static int m_bReloadVisuallyComplete = g_pNetVars->GetOffset(XorStr("DT_WeaponCSBase"), XorStr("m_bReloadVisuallyComplete"));
	return !GetFieldValue<bool>(m_bReloadVisuallyComplete);
}

float_t &CBaseEntity::m_flMaxspeed()
{
	static unsigned int _m_flMaxspeed = g_pData->Find(GetPredDescMap(), "m_flMaxspeed");
	return *(float_t*)((uintptr_t)this + _m_flMaxspeed);
}

float_t &CBaseEntity::m_surfaceFriction()
{
	static unsigned int _m_surfaceFriction = g_pData->Find(GetPredDescMap(), "m_surfaceFriction");
	return *(float_t*)((uintptr_t)this + _m_surfaceFriction);
}
AnimationLayer *CBaseEntity::GetAnimOverlays()
{
	// to find offset: use 9/12/17 dll
	// sig: 55 8B EC 51 53 8B 5D 08 33 C0
	return *(AnimationLayer**)((DWORD)this + 0x2970);
}

AnimationLayer *CBaseEntity::GetAnimOverlay(int i)
{
	if (i < 15)
		return &GetAnimOverlays()[i];
}

int CBaseEntity::GetSequenceActivity(int sequence)
{
	auto hdr = g_pModelInfo->GetStudioModel(this->GetModel());

	if (!hdr)
		return -1;

	// sig for stuidohdr_t version: 53 56 8B F1 8B DA 85 F6 74 55
	// sig for C_BaseAnimating version: 55 8B EC 83 7D 08 FF 56 8B F1 74 3D
	// c_csplayer vfunc 242, follow calls to find the function.

	static auto get_sequence_activity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(offys.getSequenceActivity);

	return get_sequence_activity(this, hdr, sequence);
}


CBasePlayerAnimState* CBaseEntity::GetBasePlayerAnimState()
{
	static int basePlayerAnimStateOffset = 0x3874;
	return *(CBasePlayerAnimState**)((DWORD)this + basePlayerAnimStateOffset);
}

CCSPlayerAnimState *CBaseEntity::GetPlayerAnimState()
{
	return *(CCSPlayerAnimState**)((DWORD)this + 0x3870);
}

void CBaseEntity::UpdateAnimationState(CCSGOPlayerAnimState *state, QAngle angle)
{
	static auto UpdateAnimState = FindPatternIDA(("client.dll"), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");
	if (!UpdateAnimState)
		return;

	__asm
	{
		mov ecx, state

		movss xmm1, dword ptr[angle + 4]
		movss xmm2, dword ptr[angle]

		call UpdateAnimState
	}
}

void CBaseEntity::ResetAnimationState(CCSGOPlayerAnimState *state)
{
	using ResetAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*);
	static auto ResetAnimState = (ResetAnimState_t)FindPatternIDA(("client.dll"), "56 6A 01 68 ? ? ? ? 8B F1");
	if (!ResetAnimState)
		return;

	ResetAnimState(state);
}

void CBaseEntity::CreateAnimationState(CCSGOPlayerAnimState *state)
{
	using CreateAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*, CBaseEntity*);
	static auto CreateAnimState = (CreateAnimState_t)FindPatternIDA("client.dll", "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46");
	if (!CreateAnimState)
		return;

	CreateAnimState(state, this);
}