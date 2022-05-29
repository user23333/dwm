#include "includes.hpp"
#include "ImGui\imgui.h"
#include "ImGui\imgui_internal.h"
#include "ImGui\imgui_impl_dx11.h"
#include "IMGUI/MyImGui.h"
#include "ck.h"
#include "CIMGuiDraw.h"
CIMGuiDraw g_ImGuiDraw;
HANDLE hEvent = NULL;

BOOL bDataCompare( const BYTE* pData, const BYTE* bMask, const char* szMask )
{
	for ( ; *szMask; ++szMask, ++pData, ++bMask )
	{
		if ( *szMask == 'x' && *pData != *bMask )
			return FALSE;
	}
	return ( *szMask ) == NULL;
}

DWORD64 FindPattern( const char* szModule, BYTE* bMask, const char* szMask )
{
	MODULEINFO mi{ };
	GetModuleInformation( GetCurrentProcess(), GetModuleHandleA( szModule ), &mi, sizeof( mi ) );

	DWORD64 dwBaseAddress = DWORD64( mi.lpBaseOfDll );
	const auto dwModuleSize = mi.SizeOfImage;

	for ( auto i = 0ul; i < dwModuleSize; i++ )
	{
		if ( bDataCompare( PBYTE( dwBaseAddress + i ), bMask, szMask ) )
			return DWORD64( dwBaseAddress + i );
	}
	return NULL;
}

void DrawEverything( IDXGISwapChain* pDxgiSwapChain )
{
	static bool b = true;
	if ( b )
	{
		ID3D11Device* pDevice = NULL;
		pDxgiSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice);
		//初始化migui
		if (g_ImGuiDraw.InitMiGuiDx11(pDxgiSwapChain, pDevice))
		{
			CK_TRACE_INFO("ImGui初始化成功!\n");
		}

		if (g_ImGuiDraw.InitMessage())
		{
			CK_TRACE_INFO("通讯初始化成功!\n");
		}
		b = false;
	
		hEvent = CreateEvent(NULL, FALSE, TRUE, L"dwm_event");
		CK_TRACE_INFO("hzw:事件%x\n", hEvent);
	}
	else
	{
		//单线程跑 加不加都无所谓
		//EnterSpinLock();
		g_ImGuiDraw.ImGuiDx11Draw();
		//LeaveSpinLock();
	}
}

using PresentMPO_ = __int64( __fastcall* )( void*, IDXGISwapChain*, unsigned int, unsigned int, int, __int64, __int64, int );
PresentMPO_ oPresentMPO = NULL;

__int64 __fastcall hkPresentMPO( void* thisptr, IDXGISwapChain* pDxgiSwapChain, unsigned int a3, unsigned int a4, int a5, __int64 a6, __int64 a7, int a8 )
{
	DrawEverything(pDxgiSwapChain);
	return oPresentMPO( thisptr, pDxgiSwapChain, a3, a4, a5, a6, a7, a8 );
}

using PresentDWM_ = __int64( __fastcall* )( void*, IDXGISwapChain*, unsigned int, unsigned int, const struct tagRECT*, unsigned int, const struct DXGI_SCROLL_RECT*, unsigned int, struct IDXGIResource*, unsigned int );
PresentDWM_ oPresentDWM = NULL;

__int64 __fastcall hkPresentDWM( void* thisptr, IDXGISwapChain* pDxgiSwapChain, unsigned int a3, unsigned int a4, const struct tagRECT* a5, unsigned int a6, const struct DXGI_SCROLL_RECT* a7, unsigned int a8, struct IDXGIResource* a9, unsigned int a10 )
{
	DrawEverything(pDxgiSwapChain);
	return oPresentDWM( thisptr, pDxgiSwapChain, a3, a4, a5, a6, a7, a8, a9, a10 );
}

UINT WINAPI MainThread( PVOID )
{
	MH_Initialize();

	while ( !GetModuleHandleA( "dwmcore.dll" ) )
		Sleep( 150 );

	//
	// [ E8 ? ? ? ? ] the relative addr will be converted to absolute addr
	auto ResolveCall = []( DWORD_PTR sig )
	{
		return sig = sig + *reinterpret_cast< PULONG >( sig + 1 ) + 5;
	};

	//
	// [ 48 8D 05 ? ? ? ? ] the relative addr will be converted to absolute addr
	auto ResolveRelative = []( DWORD_PTR sig )
	{
		return sig = sig + *reinterpret_cast< PULONG >( sig + 0x3 ) + 0x7;
	};

	auto dwRender = FindPattern( "d2d1.dll", PBYTE( "\x48\x8D\x05\x00\x00\x00\x00\x33\xED\x48\x8D\x71\x08" ), "xxx????xxxxxx" );
	if ( dwRender )
	{
		dwRender = ResolveRelative( dwRender );

		PDWORD_PTR Vtbl = PDWORD_PTR( dwRender );
		MH_CreateHook( PVOID( Vtbl[ 6 ] ), PVOID( &hkPresentDWM ), reinterpret_cast< PVOID* >( &oPresentDWM ) );
		MH_CreateHook( PVOID( Vtbl[ 7 ] ), PVOID( &hkPresentMPO ), reinterpret_cast< PVOID* >( &oPresentMPO ) );
		MH_EnableHook( MH_ALL_HOOKS );
	}
	return 0;
}
void DLLInitialization() {
	_beginthreadex(nullptr, NULL, MainThread, nullptr, NULL, nullptr);
}


BOOL WINAPI DllMain( HMODULE hDll, DWORD dwReason, PVOID )
{
	if ( dwReason == DLL_PROCESS_ATTACH )
	{
			DLLInitialization();
	}
	return true;
}
