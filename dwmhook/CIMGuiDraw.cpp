#include "CIMGuiDraw.h"
#define MAPINGLEN 144008
volatile LONG g_isLocked = FALSE;

extern HANDLE hEvent;;

CIMGuiDraw::CIMGuiDraw()
{
}

CIMGuiDraw::~CIMGuiDraw()
{
}

bool CIMGuiDraw::InitMiGuiDx11(IDXGISwapChain* pSwapChain, ID3D11Device* pd3dDevice)
{
	return ImGuiDx11_init(pSwapChain, pd3dDevice);
}

void CIMGuiDraw::ImGuiDx11Draw()
{
	//当前数组 图形数量检查
	if ((m_pDrawAll->m_DrawDll != true) || (m_pDrawAll->m_DrawCount ==0) )
	{
		return;
	}


	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	//画所有图形
	for (int i = 0; i < m_pDrawAll->m_DrawCount; i++)
	{
		switch (m_pDrawAll->m_DrawInfoArr[i].m_DrawType)
		{
		case Text_M:
		{
			DrawTextStr& Text = m_pDrawAll->m_DrawInfoArr[i].From.Text;
			DrawNewText(Text.m_X, Text.m_Y, Text.m_Color, Text.m_Outlined, Text.m_Str);
		}
			break;
		case Text2_M:
		{
			DrawTextStr& Text = m_pDrawAll->m_DrawInfoArr[i].From.Text;
			DrawNewTextStr(Text.m_X, Text.m_Y, Text.m_Color, Text.m_Outlined, Text.m_Str);
		}
			break;
		case CircleFill_M:
		{
			DrawCircleFillStr& CircleFill = m_pDrawAll->m_DrawInfoArr[i].From.CircleFill;
			DrawCircleFilled(CircleFill.m_X, CircleFill.m_Y, CircleFill.m_Radius, CircleFill.m_Color, CircleFill.m_Segments);
		}
			break;
		case Circle_M:
		{
			DrawCircleStr& Circle = m_pDrawAll->m_DrawInfoArr[i].From.Circle;
			DrawCircle(Circle.m_X, Circle.m_Y, Circle.m_Radius, Circle.m_Color, Circle.m_Segments, Circle.m_thickness);
		}
			break;
		case Rect_M:
		{
			DrawRectStr& Rect = m_pDrawAll->m_DrawInfoArr[i].From.Rect;
			DrawRect(Rect.m_X, Rect.m_Y, Rect.m_W, Rect.m_H, Rect.m_Color, Rect.m_thickness);
		}
			break;
		case RectEx_M:
		{
			DrawRectExStr& RectEx = m_pDrawAll->m_DrawInfoArr[i].From.RectEx;
			DrawRectEx(RectEx.m_X, RectEx.m_Y, RectEx.m_W, RectEx.m_H, RectEx.m_Color, RectEx.m_thickness);
		}
			break;
		case FilledRect_M:
		{
			DrawFilledRectStr& FilledRect = m_pDrawAll->m_DrawInfoArr[i].From.FilledRect;
			DrawFilledRect(FilledRect.m_X, FilledRect.m_Y, FilledRect.m_W, FilledRect.m_H, FilledRect.m_Color);
		}
			break;
		case Line_M:
		{
			DrawLineStr& Line = m_pDrawAll->m_DrawInfoArr[i].From.Line;
			DrawLine(Line.m_X1, Line.m_Y1, Line.m_X2, Line.m_Y2, Line.m_Color, Line.thickness);
		}
			break;
		case FPS_M:
		{
			FPS& Fps = m_pDrawAll->m_DrawInfoArr[i].From.Fps;
			GetFps(Fps.Fps);
		}
		break;
		default:
			break;
		}
	}

	//OutputDebugPrintf("hzw:currentThread:%d  Count:%d\n", GetCurrentThreadId(), m_pDrawAll->m_DrawCount);
	//画图标志检查
	m_pDrawAll->m_DrawCount = 0;
	m_pDrawAll->m_DrawDll = false;
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	//解决重叠 问题
	SetEvent(hEvent);

}

bool CIMGuiDraw::InitMessage()
{
	bool flag = false;

	//共享内存
	flag = InitFileMapping();

	return flag;
}

bool CIMGuiDraw::InitFileMapping()
{
	SECURITY_ATTRIBUTES sa = { 0 };
	SECURITY_DESCRIPTOR sd = { 0 };
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = &sd;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	HANDLE hFileMapping = CreateFileMappingW(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, MAPINGLEN, MAPNAME);
	m_pDrawAll = (PDRWARR)MapViewOfFile(hFileMapping, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
	if (hFileMapping == NULL)
	{
		CK_TRACE_INFO("hzw:OpenFileMappingA:%d\n", GetLastError());
		return false;
	}

	CK_TRACE_INFO("hzw:m_pDrawAll:%llx\n", m_pDrawAll);

	if (m_pDrawAll == NULL)
	{
		return false;
	}

	return true;
}



VOID EnterSpinLock(VOID)
{
	SIZE_T spinCount = 0;

	// Wait until the flag is FALSE.
	while (InterlockedCompareExchange(&g_isLocked, TRUE, FALSE) != FALSE)
	{
		// No need to generate a memory barrier here, since InterlockedCompareExchange()
		// generates a full memory barrier itself.

		// Prevent the loop from being too busy.
		if (spinCount < 32)
			Sleep(0);
		else
			Sleep(1);

		spinCount++;
	}
}

//-------------------------------------------------------------------------
VOID LeaveSpinLock(VOID)
{
	// No need to generate a memory barrier here, since InterlockedExchange()
	// generates a full memory barrier itself.

	InterlockedExchange(&g_isLocked, FALSE);
}

//-------------------------------------------------------------------------


void OutputDebugPrintf(const char* strOutputString, ...)
{
#define PUT_PUT_DEBUG_BUF_LEN   256
	char strBuffer[PUT_PUT_DEBUG_BUF_LEN] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	_vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);  //_vsnprintf_s  _vsnprintf
	//vsprintf(strBuffer,strOutputString,vlArgs);
	va_end(vlArgs);
	OutputDebugStringA(strBuffer);  //OutputDebugString    // OutputDebugStringW

}