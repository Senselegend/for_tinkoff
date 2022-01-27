#pragma once
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
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <winhttp.h>
#include <string>
#include <vector>
#include <sstream>
#include <cstdio>
#include <time.h>
#include <cstring>
#include <iostream>
#include <TlHelp32.h> 
#include <intrin.h>
#include <memory>

#pragma comment(lib,"winmm.lib")
#pragma comment( lib, "Winhttp.lib" )
#pragma comment (lib, "urlmon.lib")

BOOL IsVMware()
{
	BOOL bDetected = FALSE;

	__try
	{
		__asm
		{
			mov    ecx, 0Ah
			mov    eax, 'VMXh'
			mov    dx, 'VX'
			in    eax, dx
			cmp    ebx, 'VMXh'
			sete    al
			movzx   eax, al
			mov    bDetected, eax
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return FALSE;
	}

	return bDetected;
}

BOOL IsVM()
{
	HKEY hKey;
	int i;
	char szBuffer[64];
	const char *szProducts[] = { "*VMWARE*", "*VBOX*", "*VIRTUAL*" };

	DWORD dwSize = sizeof(szBuffer) - 1;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\ControlSet001\\Services\\Disk\\Enum", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey, "0", NULL, NULL, (unsigned char *)szBuffer, &dwSize) == ERROR_SUCCESS)
		{
			for (i = 0; i < _countof(szProducts); i++)
			{
				if (strstr(szBuffer, szProducts[i]))
				{
					RegCloseKey(hKey);
					return TRUE;
				}
			}
		}

		RegCloseKey(hKey);
	}

	return FALSE;
}

BOOL IsSandboxie()
{
	if (GetModuleHandle("SbieDll.dll") != NULL)
		return TRUE;


	return FALSE;
}

BOOL IsVirtualBox()
{
	BOOL bDetected = FALSE;

	if (LoadLibrary("VBoxHook.dll") != NULL)
		bDetected = TRUE;

	if (CreateFile("\\\\.\\VBoxMiniRdrDN", GENERIC_READ, \
		FILE_SHARE_READ, NULL, OPEN_EXISTING, \
		FILE_ATTRIBUTE_NORMAL, NULL) \
		!= INVALID_HANDLE_VALUE)
	{
		bDetected = TRUE;
	}

	return bDetected;
}

bool MemoryBreakpointDebuggerCheck()
{
	unsigned char *pMem = NULL;
	SYSTEM_INFO sysinfo = { 0 };
	DWORD OldProtect = 0;
	void *pAllocation = NULL;

	GetSystemInfo(&sysinfo);

	pAllocation = VirtualAlloc(NULL, sysinfo.dwPageSize,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_EXECUTE_READWRITE);

	if (pAllocation == NULL)
		return false;

	pMem = (unsigned char*)pAllocation;
	*pMem = 0xc3;


	if (VirtualProtect(pAllocation, sysinfo.dwPageSize,
		PAGE_EXECUTE_READWRITE | PAGE_GUARD,
		&OldProtect) == 0)
	{
		return false;
	}

	__try
	{
		__asm
		{
			mov eax, pAllocation
			push MemBpBeingDebugged
			jmp eax
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		VirtualFree(pAllocation, NULL, MEM_RELEASE);
		return false;
	}

	__asm {MemBpBeingDebugged:}
	VirtualFree(pAllocation, NULL, MEM_RELEASE);
	return true;
}

inline bool Int2DCheck()
{
	__try
	{
		__asm
		{
			int 0x2d
			xor eax, eax
			add eax, 2
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}

inline bool IsDbgPresentPrefixCheck()
{
	__try
	{
		__asm __emit 0xF3
		__asm __emit 0x64
		__asm __emit 0xF1
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}

inline void PushPopSS()
{

	__asm
	{
		push ss
		pop ss
		mov eax, 0xC000C1EE
		xor edx, edx
	}

}

PVOID GetPEB()
{
#ifdef _WIN64
	return (PVOID)__readgsqword(0x0C * sizeof(PVOID));
#else
	return (PVOID)__readfsdword(0x0C * sizeof(PVOID));
#endif
}

WORD GetVersionWord()
{
	OSVERSIONINFO verInfo = { sizeof(OSVERSIONINFO) };
	GetVersionExA(&verInfo);
	return MAKEWORD(verInfo.dwMinorVersion, verInfo.dwMajorVersion);
}

BOOL IsWin8OrHigher() { return GetVersionWord() >= _WIN32_WINNT_WIN8; }

BOOL IsVistaOrHigher() { return GetVersionWord() >= _WIN32_WINNT_VISTA; }

PVOID GetPEB64()
{
	PVOID pPeb = 0;
#ifndef _WIN64
	if (IsWin8OrHigher())
	{
		BOOL isWow64 = FALSE;
		typedef BOOL(WINAPI *pfnIsWow64Process)(HANDLE hProcess, PBOOL isWow64);
		pfnIsWow64Process fnIsWow64Process = (pfnIsWow64Process)
			GetProcAddress(GetModuleHandleA("Kernel32.dll"), "IsWow64Process");
		if (fnIsWow64Process(GetCurrentProcess(), &isWow64))
		{
			if (isWow64)
			{
				pPeb = (PVOID)__readfsdword(0x0C * sizeof(PVOID));
				pPeb = (PVOID)((PBYTE)pPeb + 0x1000);
			}
		}
	}
#endif
	return pPeb;
}

#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40
#define NT_GLOBAL_FLAG_DEBUGGED (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)

void CheckNtGlobalFlag()
{
	PVOID pPeb = GetPEB();
	PVOID pPeb64 = GetPEB64();
	DWORD offsetNtGlobalFlag = 0;
#ifdef _WIN64
	offsetNtGlobalFlag = 0xBC;
#else
	offsetNtGlobalFlag = 0x68;
#endif
	DWORD NtGlobalFlag = *(PDWORD)((PBYTE)pPeb + offsetNtGlobalFlag);
	if (NtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED)
	{
		exit(-1);
	}
	if (pPeb64)
	{
		DWORD NtGlobalFlagWow64 = *(PDWORD)((PBYTE)pPeb64 + 0xBC);
		if (NtGlobalFlagWow64 & NT_GLOBAL_FLAG_DEBUGGED)
		{
			exit(-1);
		}
	}
}

PIMAGE_NT_HEADERS GetImageNtHeaders(PBYTE pImageBase)
{
	PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER)pImageBase;
	return (PIMAGE_NT_HEADERS)(pImageBase + pImageDosHeader->e_lfanew);
}

PIMAGE_SECTION_HEADER FindRDataSection(PBYTE pImageBase)
{
	static const std::string rdata = ".rdata";
	PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pImageBase);
	PIMAGE_SECTION_HEADER pImageSectionHeader = IMAGE_FIRST_SECTION(pImageNtHeaders);
	int n = 0;
	for (; n < pImageNtHeaders->FileHeader.NumberOfSections; ++n)
	{
		if (rdata == (char*)pImageSectionHeader[n].Name)
		{
			break;
		}
	}
	return &pImageSectionHeader[n];
}

void CheckGlobalFlagsClearInProcess()
{
	PBYTE pImageBase = (PBYTE)GetModuleHandle(NULL);
	PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pImageBase);
	PIMAGE_LOAD_CONFIG_DIRECTORY pImageLoadConfigDirectory = (PIMAGE_LOAD_CONFIG_DIRECTORY)(pImageBase
		+ pImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress);
	if (pImageLoadConfigDirectory->GlobalFlagsClear != 0)
	{
		exit(-1);
	}
}

void CheckGlobalFlagsClearInFile()
{
	HANDLE hExecutable = INVALID_HANDLE_VALUE;
	HANDLE hExecutableMapping = NULL;
	PBYTE pMappedImageBase = NULL;
	__try
	{
		PBYTE pImageBase = (PBYTE)GetModuleHandle(NULL);
		PIMAGE_SECTION_HEADER pImageSectionHeader = FindRDataSection(pImageBase);
		TCHAR pszExecutablePath[MAX_PATH];
		DWORD dwPathLength = GetModuleFileName(NULL, pszExecutablePath, MAX_PATH);
		if (0 == dwPathLength) __leave;
		hExecutable = CreateFile(pszExecutablePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (INVALID_HANDLE_VALUE == hExecutable) __leave;
		hExecutableMapping = CreateFileMapping(hExecutable, NULL, PAGE_READONLY, 0, 0, NULL);
		if (NULL == hExecutableMapping) __leave;
		pMappedImageBase = (PBYTE)MapViewOfFile(hExecutableMapping, FILE_MAP_READ, 0, 0,
			pImageSectionHeader->PointerToRawData + pImageSectionHeader->SizeOfRawData);
		if (NULL == pMappedImageBase) __leave;
		PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pMappedImageBase);
		PIMAGE_LOAD_CONFIG_DIRECTORY pImageLoadConfigDirectory = (PIMAGE_LOAD_CONFIG_DIRECTORY)(pMappedImageBase
			+ (pImageSectionHeader->PointerToRawData
				+ (pImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress - pImageSectionHeader->VirtualAddress)));
		if (pImageLoadConfigDirectory->GlobalFlagsClear != 0)
		{

			exit(-1);
		}
	}
	__finally
	{
		if (NULL != pMappedImageBase)
			UnmapViewOfFile(pMappedImageBase);
		if (NULL != hExecutableMapping)
			CloseHandle(hExecutableMapping);
		if (INVALID_HANDLE_VALUE != hExecutable)
			CloseHandle(hExecutable);
	}
}

int GetHeapFlagsOffset(bool x64)
{
	return x64 ?
		IsVistaOrHigher() ? 0x70 : 0x14 :
		IsVistaOrHigher() ? 0x40 : 0x0C;
}

int GetHeapForceFlagsOffset(bool x64)
{
	return x64 ?
		IsVistaOrHigher() ? 0x74 : 0x18 :
		IsVistaOrHigher() ? 0x44 : 0x10;
}

void CheckHeap()
{
	PVOID pPeb = GetPEB();
	PVOID pPeb64 = GetPEB64();
	PVOID heap = 0;
	DWORD offsetProcessHeap = 0;
	PDWORD heapFlagsPtr = 0, heapForceFlagsPtr = 0;
	BOOL x64 = FALSE;
#ifdef _WIN64
	x64 = TRUE;
	offsetProcessHeap = 0x30;
#else
	offsetProcessHeap = 0x18;
#endif
	heap = (PVOID)*(PDWORD_PTR)((PBYTE)pPeb + offsetProcessHeap);
	heapFlagsPtr = (PDWORD)((PBYTE)heap + GetHeapFlagsOffset(x64));
	heapForceFlagsPtr = (PDWORD)((PBYTE)heap + GetHeapForceFlagsOffset(x64));
	if (*heapFlagsPtr & ~HEAP_GROWABLE || *heapForceFlagsPtr != 0)
	{
		exit(-1);
	}
	if (pPeb64)
	{
		heap = (PVOID)*(PDWORD_PTR)((PBYTE)pPeb64 + 0x30);
		heapFlagsPtr = (PDWORD)((PBYTE)heap + GetHeapFlagsOffset(true));
		heapForceFlagsPtr = (PDWORD)((PBYTE)heap + GetHeapForceFlagsOffset(true));
		if (*heapFlagsPtr & ~HEAP_GROWABLE || *heapForceFlagsPtr != 0)
		{
			exit(-1);
		}
	}
}

typedef NTSTATUS(NTAPI *pfnNtSetInformationThread)(
	_In_ HANDLE ThreadHandle,
	_In_ ULONG  ThreadInformationClass,
	_In_ PVOID  ThreadInformation,
	_In_ ULONG  ThreadInformationLength
	);
const ULONG ThreadHideFromDebugger = 0x11;

void HideFromDebugger()
{
	HMODULE hNtDll = LoadLibrary(TEXT("ntdll.dll"));
	pfnNtSetInformationThread NtSetInformationThread = (pfnNtSetInformationThread)
		GetProcAddress(hNtDll, "NtSetInformationThread");
	NTSTATUS status = NtSetInformationThread(GetCurrentThread(),
		ThreadHideFromDebugger, NULL, 0);
}



LONG WINAPI UnhandledExcepFilter(PEXCEPTION_POINTERS pExcepPointers)
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)
		pExcepPointers->ContextRecord->Eax);

	pExcepPointers->ContextRecord->Eip += 2;

	return EXCEPTION_CONTINUE_EXECUTION;
}

#define JUNK_CODE_ONE        \
    __asm{push eax}            \
    __asm{xor eax, eax}        \
    __asm{setpo al}            \
    __asm{push edx}            \
    __asm{xor edx, eax}        \
    __asm{sal edx, 2}        \
    __asm{xchg eax, edx}    \
    __asm{pop edx}            \
    __asm{or eax, ecx}        \
    __asm{pop eax}




DWORD GetProcessPID(char* ProcName)
{
	PROCESSENTRY32 lppe;
	long PID = 0, Result = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnap)
	{
		lppe.dwSize = sizeof(PROCESSENTRY32);
		Result = Process32First(hSnap, &lppe);
		while (Result)
		{
			if (strcmp(lppe.szExeFile, ProcName) == NULL)
			{
				PID = lppe.th32ProcessID;
				break;
			}
			Result = Process32Next(hSnap, &lppe);
		}
		CloseHandle(hSnap);
	}
	return PID;
}





template <int X> struct EnsureCompileTime {
	enum : int {
		Value = X
	};
};

//Use Compile-Time as seed
#define compile_seed (	(__TIME__[7] - '0') * 1  + (__TIME__[6] - '0') * 10		+	\
						(__TIME__[4] - '0') * 60   + (__TIME__[3] - '0') * 600	+	\
						(__TIME__[1] - '0') * 3600 + (__TIME__[0] - '0') * 36000	)

constexpr int LinearCongruentGenerator(int Rounds) {
	return 1013904223 + 1664525 * ((Rounds > 0) ? LinearCongruentGenerator(Rounds - 1) : compile_seed & 0xFFFFFFFF);
}

#define Random() EnsureCompileTime<LinearCongruentGenerator(10)>::Value //10 Rounds
#define RandomNumber(Min, Max) (Min + (Random() % (Max - Min + 1)))

template <int... Pack> struct IndexList {};

template <typename IndexList, int Right> struct Append;
template <int... Left, int Right> struct Append<IndexList<Left...>, Right> {
	typedef IndexList<Left..., Right> Result;
};

template <int N> struct ConstructIndexList {
	typedef typename Append<typename ConstructIndexList<N - 1>::Result, N - 1>::Result Result;
};

template <> struct ConstructIndexList<0> {
	typedef IndexList<> Result;
};

template <typename Char, typename IndexList> class XorStringT;
template <typename Char, int... Index> class XorStringT<Char, IndexList<Index...> > {

private:
	Char Value[sizeof...(Index)+1];

	static const Char XORKEY = static_cast<Char>(RandomNumber(0, 0xFFFF));

	template <typename Char>
	constexpr Char EncryptCharacterT(const Char Character, int Index) {
		return Character ^ (XORKEY + Index);
	}

public:
	__forceinline constexpr XorStringT(const Char* const String)
		: Value{ EncryptCharacterT(String[Index], Index)... } {}

	const Char *decrypt() {
		for (int t = 0; t < sizeof...(Index); t++) {
			Value[t] = Value[t] ^ (XORKEY + t);
		}

		Value[sizeof...(Index)] = static_cast<Char>(0);

		return Value;
	}

	const Char *get() {
		return Value;
	}
};

#define XORSTR( String ) (  XorStringT<char,  ConstructIndexList<sizeof( String ) - 1>::Result>( String ).decrypt() )
#define XORWSTR( String ) (  XorStringT<wchar_t,  ConstructIndexList<(sizeof( String ) - 1) / 2>::Result>( String ).decrypt() )

class my_license
{
	typedef void(_stdcall *GSI)(LPSYSTEM_INFO);
public:
	const  char
		*user_name = new char,
		*user_password = new char,
		*user_key = new char;

	virtual std::string generate_key();
	virtual std::string GetRec(std::string, int);

	virtual void        win32_copy_to_clipboardW(char*);
	virtual bool        check_license(std::string, std::string);


	std::string
		host = XORSTR("pasteware.top/"),
		path = XORSTR("pathtodll/"),
		host_gate_login = XORSTR("license-test.php?login="),
		host_gate_password = XORSTR("&password="),
		host_gate_system_key = XORSTR("&binding="),
		host_gate_token = XORSTR("&token="),
		host_gate_inject = XORSTR("&inject=1"),
		base64_chars =
		XORSTR("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/");

private:
	virtual DWORD       get_system_info();
	virtual char*       string_to_hex(const char * input);
	virtual char*       convert_key_to_char(const char*, const char*, DWORD);

	virtual std::string get_hash_system_key();
	virtual std::string str_to_hex(const std::string);
	virtual std::string get_hash_text(const void*, const size_t);
	virtual std::string base64_encode(char const*, unsigned int);

	SYSTEM_INFO sysinfo;
	DWORD system_key = NULL;
	HGLOBAL hMem = NULL;
	UINT uFormat = NULL;


};
unique_ptr<my_license> my_lic(new my_license());

class MD5
{
public:
	typedef unsigned int size_type;

	MD5();
	MD5(const std::string&);
	virtual MD5& finalize();
	virtual void update(const unsigned char *, size_type);
	virtual void update(const char *, size_type);
	virtual 	std::string hexdigest() const;
	friend std::ostream& operator<<(std::ostream&, MD5);

private:
	void init();
	typedef unsigned char uint1;
	typedef unsigned int uint4;
	enum { blocksize = 64 };

	virtual void transform(const uint1 block[blocksize]);
	static void decode(uint4 output[], const uint1 input[], size_type len);
	static void encode(uint1 output[], const uint4 input[], size_type len);

	bool finalized;
	uint1 buffer[blocksize];
	uint4 count[2];
	uint4 state[4];
	uint1 digest[16];


	static inline uint4 F(uint4 x, uint4 y, uint4 z);
	static inline uint4 G(uint4 x, uint4 y, uint4 z);
	static inline uint4 H(uint4 x, uint4 y, uint4 z);
	static inline uint4 I(uint4 x, uint4 y, uint4 z);
	static inline uint4 rotate_left(uint4 x, int n);
	static inline void FF(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	static inline void GG(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	static inline void HH(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	static inline void II(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
};

std::string md5(const std::string str);


class CHTTPSession
{
private:
	HINTERNET hSession, hConnect, hRequest;
	LPCWSTR wcUserAgent;
	LPCWSTR wcHTTPVerb;

	int iHTTPMethod;
	std::wstring wsDomain, wsDocument;
	std::string sPOSTData, sPOSTFile;
	int iSESSION_PROTOCOL, iSESSION_PORT;
private:
	virtual bool SetHTTPSecuirty(int);
	virtual bool CreateSession();
	virtual	bool SetServer(LPCWSTR, INTERNET_PORT);
	virtual bool SetRequest(LPCWSTR);
	virtual bool SendRequest();
	virtual bool SetResponseHandle();
	virtual bool HTTPPhraseData(std::string, std::string);

	void CloseHandles();

public:
	virtual std::wstring GetResponseHeader();
	virtual int GetHTTPStatusCode();
	virtual bool ReciveData(std::string&);
	virtual bool SetHTTPMethod(int);

	virtual bool Send(LPCWSTR, LPCWSTR);
	virtual bool Recv(int&, std::string&, std::string&);
	virtual int Transmit(int, int, std::string, std::string&, std::string, int);


	CHTTPSession()
	{
		wcUserAgent = L"MyUserAgent/1.0";
	}

	CHTTPSession(LPCWSTR lwcUserAgent)
	{
		wcUserAgent = lwcUserAgent;
	}

	~CHTTPSession()
	{
		CloseHandles();
	}

	void AddPOSTFile(std::string sData)
	{
		sPOSTFile = sData;
	}
};

enum CHTTP_PROTOCOLID
{
	HTTP_CLEAR = 10,
	HTTP_SSL
};

enum CHTTP_METHODID
{
	HTTP_GET = 10,
	HTTP_POST
};