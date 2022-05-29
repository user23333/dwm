#pragma once
#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <mutex>
#pragma warning(disable:4244)
#pragma warning(disable:4267)
#define BUF_LEN 256
#define MAX_DRAW 500
#define MAPNAME L"hzw_dwm"
#define MAPINGLEN 144008

struct ImVec4
{
	float                                                     x, y, z, w;
	constexpr ImVec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
	constexpr ImVec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) { }
#ifdef IM_VEC4_CLASS_EXTRA
	IM_VEC4_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and ImVec4.
#endif
};

struct DrawTextStr
{
	float	m_X;
	float	m_Y;
	ImVec4	m_Color;
	bool	m_Outlined;
	char	m_Str[BUF_LEN];
};
struct DrawCircleFillStr
{
	float		m_X;
	float		m_Y;
	float		m_Radius;
	ImVec4		m_Color;
	int		m_Segments;
};
struct DrawCircleStr
{
	float		m_X;
	float		m_Y;
	float		m_Radius;
	ImVec4		m_Color;
	int		m_Segments;
	float m_thickness;
};
struct DrawRectStr
{
	float m_X;
	float m_Y;
	float m_W;
	float m_H;
	ImVec4 m_Color;
	float m_thickness;
};
struct DrawRectExStr
{
	float m_X;
	float m_Y;
	float m_W;
	float m_H;
	ImVec4	m_Color;
	float	m_thickness;
};
struct DrawFilledRectStr
{
	float m_X;
	float m_Y;
	float m_W;
	float m_H;
	ImVec4	m_Color;
};
struct DrawLineStr
{
	float m_X1;
	float m_Y1;
	float m_X2;
	float m_Y2;
	ImVec4	m_Color;
	float thickness;
};
struct FPS {
	float Fps;
};
enum DRAWTYPE { Text_M, Text2_M, CircleFill_M, Circle_M, Rect_M, RectEx_M, FilledRect_M, Line_M, FPS_M };
struct DrawInfo
{
	DRAWTYPE m_DrawType;
	union
	{
		DrawTextStr			Text;
		DrawCircleFillStr	CircleFill;
		DrawCircleStr		Circle;
		DrawRectStr			Rect;
		DrawRectExStr		RectEx;
		DrawFilledRectStr	FilledRect;
		DrawLineStr			Line;
		FPS					Fps;
	}From;
};
typedef struct DrawArr
{
	bool		m_DrawDll;
	int			m_DrawCount;
	DrawInfo	m_DrawInfoArr[MAX_DRAW];
}DRAWARR, * PDRWARR;

class CDwmMsg
{
public:
	CDwmMsg();
	~CDwmMsg();
	bool InitMsg();

public:
	bool ExeStartDraw();
	void ExeEndDraw();
	VOID DrawNewText(float X, float Y, ImVec4 Color, bool Outlined, const char* Str);
	VOID DrawNewTextStr(float X, float Y, ImVec4 Color, bool Outlined, const char* Str);
	VOID DrawCircleFilled(float X, float Y, float Radius, ImVec4 Color, int Segments);
	VOID DrawCircle(float X, float Y, float Radius, ImVec4 Color, int Segments, float thickness);
	VOID DrawRect(float X, float Y, float W, float H, ImVec4 Color, float thickness);
	VOID DrawRectEx(float X, float Y, float W, float H, ImVec4 Color, float thickness);
	VOID DrawFilledRect(float X, float Y, float W, float H, ImVec4 Color);
	VOID DrawLine(float X1, float Y1, float X2, float Y2, ImVec4 Color, float thickness);
	float GetFps();
private:
	bool _InitExeMsg();
private:
	HANDLE						m_hFileMapping = NULL;
	PDRWARR						m_pDrawList = nullptr;
	int							m_Count = 0;
	HANDLE hEvent;
};



VOID EnterSpinLock(VOID);
VOID LeaveSpinLock(VOID);