#include "sdk.h"
#include "hooks.h"
#include "Menu.h"
#include "MaterialHelper.h"
#include "BacktrackingHelper.h"
#include "security.h"
#include "Math.h"
#include "global.h"
#include <dbghelp.h>
#include <tchar.h>
#include "Config.h"
#include "ScreenEffects.h"
#include "AW_hitmarker.h"

HINSTANCE hAppInstance;
CClient* g_pClient;
CClientEntityList* g_pEntitylist;
CEngine* g_pEngine;
CInput* g_pInput;
CModelInfo* g_pModelInfo;
CInputSystem* g_pInputSystem;
CPanel* g_pPanel;
IStudioRender* g_pStudioRender;
CSurface* g_pSurface;
CEngineTrace* g_pEngineTrace;
CDebugOverlay* g_pDebugOverlay;
IPhysicsSurfaceProps* g_pPhysics;
CRenderView* g_pRenderView;
CUtlVectorSimple *g_ClientSideAnimationList;
CClientModeShared* g_pClientMode;
CGlobalVarsBase* g_pGlobals;
CModelRender* g_pModelRender;
CGlowObjectManager* g_GlowObjManager;
CMaterialSystem* g_pMaterialSystem;
IMoveHelper* g_pMoveHelper;

CPrediction* g_pPrediction;
CGameMovement* g_pGameMovement;
IGameEventManager* g_pGameEventManager;
IEngineVGui* g_pEngineVGUI;
ICvar* g_pCvar;
CInterfaces Interfaces;
CCRC gCRC;

C_TEFireBullets* g_pFireBullet;
IViewRenderBeams* g_pViewRenderBeams;

VMT* panelVMT;
VMT* beginFrame;
VMT* clientmodeVMT;
VMT* enginevguiVMT;
VMT* modelrenderVMT;
VMT* clientVMT;
VMT* HNetchan;
VMT* firebulletVMT;
VMT* d3d9VMT;
VMT* netChan;
VMT* enginetraceVMT;
VMT* renderviewVMT;

namespace FakeLatency
{
	extern char *clientdllstr;
	extern char *enginedllstr;
	extern char *cam_tothirdperson_sig;
	extern char *cam_tofirstperson_sig;
	extern char *clientstatestr;
	extern char *updateclientsideanimfnsigstr;
	extern void DecStr(char *adr, const unsigned len);
	extern void EncStr(char *adr, const unsigned len);
	extern void DecStr(char *adr);
	extern void EncStr(char *adr);
	extern HANDLE FindHandle(std::string name);
	extern uintptr_t FindMemoryPattern(HANDLE ModuleHandle, char* strpattern, int length);
	extern HANDLE EngineHandle;
	extern HANDLE ClientHandle;
	extern DWORD ClientState;
}


CVMTHookManager* g_pD3D = nullptr;
CVMTHookManager* g_pClientModeHook = nullptr;

HMODULE h_ThisModule;

void Init()
{
	SetupOffsets();

	g_MaterialHelper = new CMaterialHelper();
	g_Event.RegisterSelf();
	//hitmarker_2->initialize();
	srand(time(0));

	GUI_Init();

	ConSys->CheckConfigs();
	ConSys->Handle();

	panelVMT = new VMT(g_pPanel);
	panelVMT->HookVM((void*)Hooks::PaintTraverse, 41);
	panelVMT->ApplyVMT();

	g_GlowObjManager = *(CGlowObjectManager**)(FindPatternIDA("client.dll", "0F 11 05 ? ? ? ? 83 C8 01 C7 05 ? ? ? ? 00 00 00 00") + 3);

	clientmodeVMT = new VMT(g_pClientMode);
	clientmodeVMT->HookVM((void*)Hooks::CreateMove, 24);
	clientmodeVMT->HookVM((void*)Hooks::OverrideView, 18);
	clientmodeVMT->ApplyVMT();

	g_pClientModeHook = new CVMTHookManager((PDWORD*)g_pClientMode);
	g_pClientModeHook->HookMethod((DWORD)hkDoPostScreenSpaceEffects, 44);
	g_pClientModeHook->HookMethod((DWORD)&GGetViewModelFOV, 35);
	g_pClientModeHook->ReHook();

	renderviewVMT = new VMT(g_pRenderView);
	renderviewVMT->HookVM((void*)Hooks::scene_end, 9);
	renderviewVMT->ApplyVMT();

	modelrenderVMT = new VMT(g_pModelRender);
	modelrenderVMT->HookVM((void*)Hooks::DrawModelExecute, 21);
	modelrenderVMT->ApplyVMT();

	clientVMT = new VMT(g_pClient);
	clientVMT->HookVM((void*)Hooks::FrameStageNotify, 36);
	clientVMT->ApplyVMT();

	beginFrame = new VMT(g_pStudioRender);
	beginFrame->HookVM((void*)Hooks::g_hkBeginFrame, 9);
	beginFrame->ApplyVMT();

	auto dwDevice = **(uint32_t**)(FindPatternIDA(XorStr("shaderapidx9.dll"), XorStr("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 1);

	d3d9VMT = new VMT((void*)dwDevice);
	d3d9VMT->HookVM((void*)Hooks::D3D9_EndScene, 42);
	d3d9VMT->ApplyVMT();

	g_pD3D = new CVMTHookManager(reinterpret_cast<DWORD**>(dwDevice));
	oResetScene = reinterpret_cast<tReset>(g_pD3D->HookMethod(reinterpret_cast<DWORD>(Hooks::hkdReset), 16));

	Hooks::g_pOldWindowProc = (WNDPROC)SetWindowLongPtr(G::Window, GWLP_WNDPROC, (LONG_PTR)Hooks::WndProc);




	FakeLatency::DecStr(FakeLatency::clientdllstr, 10);
	FakeLatency::ClientHandle = FakeLatency::FindHandle(FakeLatency::clientdllstr);
	FakeLatency::EncStr(FakeLatency::clientdllstr, 10);

	FakeLatency::DecStr(FakeLatency::enginedllstr, 10);
	FakeLatency::EngineHandle = FakeLatency::FindHandle(FakeLatency::enginedllstr); //engine.dll
	FakeLatency::EncStr(FakeLatency::enginedllstr, 10);



	FakeLatency::DecStr(FakeLatency::clientstatestr, 50);
	DWORD adr = FakeLatency::FindMemoryPattern(FakeLatency::EngineHandle, FakeLatency::clientstatestr, 50);
	FakeLatency::EncStr(FakeLatency::clientstatestr, 50);

	FakeLatency::ClientState = *(DWORD*)(adr + 2);




	FakeLatency::DecStr(FakeLatency::updateclientsideanimfnsigstr, 94);
	adr = FakeLatency::FindMemoryPattern(FakeLatency::ClientHandle, FakeLatency::updateclientsideanimfnsigstr, 94);
	FakeLatency::EncStr(FakeLatency::updateclientsideanimfnsigstr, 94);



	delete[] FakeLatency::clientstatestr;
	delete[] FakeLatency::enginedllstr;
	delete[] FakeLatency::updateclientsideanimfnsigstr;




}

void StartCheat()
{
	if (Interfaces.GetInterfaces() && g_pPanel && g_pClientMode)
	{
		Sleep(500);
		Init();
	}
}

BOOL WINAPI DllMain(HINSTANCE Instance, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(Instance);
		G::Window = FindWindowA(("Valve001"), 0);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)StartCheat, 0, 0, 0);
		break;
	}
	return true;
}

MsgFn oMsg;

void __cdecl Msg(char const* msg, ...)
{
	//DOES NOT CRASH
	if (!oMsg)
		oMsg = (MsgFn)GetProcAddress(GetModuleHandle(XorStr("tier0.dll")), XorStr("Msg"));

	char buffer[989];
	va_list list;
	va_start(list, msg);
	vsprintf_s(buffer, msg, list);
	va_end(list);
	oMsg(buffer, list);
}