#include "CDwmMsg.h"
#include <string>


std::string AnisToUTF8(const std::string& Str)
{
    int nwLen = ::MultiByteToWideChar(CP_ACP, 0, Str.c_str(), -1, NULL, 0);

    wchar_t* pwBuf = new wchar_t[(size_t)nwLen + 1];
    ZeroMemory(pwBuf, (size_t)nwLen * 2 + 2);

    ::MultiByteToWideChar(CP_ACP, 0, Str.c_str(), Str.length(), pwBuf, nwLen);

    int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

    char* pBuf = new char[(size_t)nLen + 1];
    ZeroMemory(pBuf, (size_t)nLen + 1);

    ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string retStr(pBuf);

    delete[]pwBuf;
    delete[]pBuf;

    pwBuf = NULL;
    pBuf = NULL;

    return retStr;
}

CDwmMsg::CDwmMsg()
{
}

CDwmMsg::~CDwmMsg()
{
}

bool CDwmMsg::InitMsg()
{
	return _InitExeMsg();
}

bool CDwmMsg::ExeStartDraw()
{
    if (m_pDrawList == nullptr)
    {
        return false;
    }
    DWORD wr = 0;

    // dwm.exe 画完后,我们向共享内存再次写入数据
    do 
    {
        wr = WaitForSingleObject(hEvent, 5000);

    } while (wr == WAIT_TIMEOUT);
 

    return TRUE;
}
void CDwmMsg::ExeEndDraw()
{
	m_pDrawList->m_DrawDll = true;
}
float CDwmMsg::GetFps() 
{
	 m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].m_DrawType = FPS_M;
    FPS& Fps = m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].From.Fps;
    return Fps.Fps;
}
VOID CDwmMsg::DrawNewText(float X, float Y, ImVec4 Color, bool Outlined, const char* Str)
{
   m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].m_DrawType = Text_M;
	
    DrawTextStr& Text = m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].From.Text;
    Text.m_X = X;
    Text.m_Y = Y;
    Text.m_Color = Color;
    Text.m_Outlined = Outlined;
    strcpy(Text.m_Str, Str);
	m_pDrawList->m_DrawCount++;
    return VOID();
}

VOID CDwmMsg::DrawNewTextStr(float X, float Y, ImVec4 Color, bool Outlined, const char* Str)
{
    m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].m_DrawType = Text2_M;
    DrawTextStr& Text = m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].From.Text;
    Text.m_X = X;
    Text.m_Y = Y;
    Text.m_Color = Color;
    Text.m_Outlined = Outlined;
    strcpy(Text.m_Str, Str);
    m_pDrawList->m_DrawCount++;
    return VOID();
}

VOID CDwmMsg::DrawCircleFilled(float X, float Y, float Radius, ImVec4 Color, int Segments)
{
    m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].m_DrawType = CircleFill_M;
	
    DrawCircleFillStr& CircleFill = m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].From.CircleFill;
    CircleFill.m_X = X;
    CircleFill.m_Y = Y;
    CircleFill.m_Radius = Radius;
    CircleFill.m_Color = Color;
    CircleFill.m_Segments = Segments;
	m_pDrawList->m_DrawCount++;
    return VOID();
}

VOID CDwmMsg::DrawCircle(float X, float Y, float Radius, ImVec4 Color, int Segments, float thickness)
{
    m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].m_DrawType = Circle_M;
	
    DrawCircleStr& Circle = m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].From.Circle;
    Circle.m_X = X;
    Circle.m_Y = Y;
    Circle.m_Radius = Radius;
    Circle.m_Color = Color;
    Circle.m_Segments = Segments;
    Circle.m_thickness = thickness;
	m_pDrawList->m_DrawCount++;
    return VOID();
}

VOID CDwmMsg::DrawRect(float X, float Y, float W, float H, ImVec4 Color, float thickness)
{
    m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].m_DrawType = Rect_M;
    DrawRectStr& Rect = m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].From.Rect;
    Rect.m_X = X;
    Rect.m_Y = Y;
    Rect.m_W = W;
    Rect.m_H = H;
    Rect.m_Color = Color;
    Rect.m_thickness = thickness;
	m_pDrawList->m_DrawCount++;
    return VOID();
}

VOID CDwmMsg::DrawRectEx(float X, float Y, float W, float H, ImVec4 Color, float thickness)
{
    m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].m_DrawType = RectEx_M;
    DrawRectExStr& RectEx = m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].From.RectEx;
    RectEx.m_X = X;
    RectEx.m_Y = Y;
    RectEx.m_W = W;
    RectEx.m_H = H;
    RectEx.m_Color = Color;
    RectEx.m_thickness = thickness;
	m_pDrawList->m_DrawCount++;
    return VOID();
}

VOID CDwmMsg::DrawFilledRect(float X, float Y, float W, float H, ImVec4 Color)
{
   m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].m_DrawType = FilledRect_M;
	
    DrawFilledRectStr& FilledRect = m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].From.FilledRect;
    FilledRect.m_X = X;
    FilledRect.m_Y = Y;
    FilledRect.m_W = W;
    FilledRect.m_H = H;
    FilledRect.m_Color = Color;
	m_pDrawList->m_DrawCount++;
    return VOID();
}
VOID CDwmMsg::DrawLine(float X1, float Y1, float X2, float Y2, ImVec4 Color, float thickness)
{
    m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].m_DrawType = Line_M;
    DrawLineStr& Line = m_pDrawList->m_DrawInfoArr[m_pDrawList->m_DrawCount].From.Line;
    Line.m_X1 = X1;
    Line.m_Y1 = Y1;
    Line.m_X2 = X2;
    Line.m_Y2 = Y2;
    Line.m_Color = Color;
    Line.thickness = thickness;
	m_pDrawList->m_DrawCount++;
    return VOID();
}
bool CDwmMsg::_InitExeMsg()
{

    m_hFileMapping = OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE, MAPNAME);
    if (m_hFileMapping == NULL)
    {
        return false;
    }

    m_pDrawList = (PDRWARR)MapViewOfFile(m_hFileMapping,FILE_MAP_WRITE | FILE_MAP_READ, 0,0,0);
    if (m_pDrawList == NULL)
    {
        return false;
    }

    hEvent = OpenEventW(EVENT_ALL_ACCESS, FALSE, L"dwm_event");
	
    m_pDrawList->m_DrawDll = false;
	CloseHandle(m_hFileMapping);
    return true;
}



volatile LONG g_isLocked = FALSE;
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