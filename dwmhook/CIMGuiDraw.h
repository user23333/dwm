#pragma once
#include "IMGUI/MyImGui.h"
#include "ck.h"
/*文字的长度*/
#define BUF_LEN 256
/*画的最大个数*/
#define MAX_DRAW 500
/*共享内存*/
#define MAPNAME L"hzw_dwm"

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
/*图形种类枚举*/
enum DRAWTYPE { Text_M, Text2_M,CircleFill_M, Circle_M, Rect_M, RectEx_M, FilledRect_M, Line_M,FPS_M};

//图形的信息
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


//对画图状态 以及画的东西描述
typedef struct DrawArr
{
	bool		m_DrawDll;//true exe可以画 false停止画
	/*
	* 这边可以自己扩展相应的结构体描述图像
	*/
	int			m_DrawCount;
	DrawInfo	m_DrawInfoArr[MAX_DRAW];
}DRAWARR, * PDRWARR;

class CIMGuiDraw
{
public:
	CIMGuiDraw();
	~CIMGuiDraw();
	
public:
	/*
	* 提交所有画
	*/
	void ImGuiDx11Draw();
	bool InitMiGuiDx11(IDXGISwapChain* pSwapChain, ID3D11Device* pd3dDevice);
	/*
	* 初始化通讯 m_pDrawAll
	*/
	bool InitMessage();

private:
	/*
	* 共享内存
	*/
	bool InitFileMapping();

	/*
	* 
	*/

private:
	PDRWARR m_pDrawAll = nullptr;
};


VOID WINAPI EnterSpinLock(VOID);
VOID WINAPI LeaveSpinLock(VOID);
void OutputDebugPrintf(const char* strOutputString, ...);