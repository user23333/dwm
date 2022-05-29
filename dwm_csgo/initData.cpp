#include "initData.h"
#include "Dwm_Dram.h"
extern Dwm_Dram* pDwm;
int nCnt; //优化性能 记录次数



namespace G
{
	HWND g_hwnd;
	HANDLE g_hanle;
	ULONG_PTR g_matrix_address;
	ULONG_PTR g_angle_address;
	ULONG_PTR g_self_address;
	ULONG_PTR g_players_address;
	ULONG_PTR g_players_bar;
	player_list g_players[g_players_count];
	Matrix4x4 g_matrix;
	RECT rect;
}

namespace Menum 
{
	BOOL show;
	BOOL rect;
	BOOL ambot;
	BOOL point;
}

void initialize_address()
{

	ULONG_PTR engine_module_address = (ULONG_PTR)GetModuleHandleW(L"engine.dll");
	ULONG_PTR client_module_address = (ULONG_PTR)GetModuleHandleW(L"client.dll");
	//ULONG_PTR server_module_address = (ULONG_PTR)GetModuleHandleW(L"server.dll");


	int matrix_address = engine_module_address + 0x596EF0;
	int angle_address = engine_module_address + 0x4622D0 - 4;
	int self_address = client_module_address + 0x04D3904;
	int players_address = client_module_address + 0x04D3904;
	ULONG_PTR players_bar = client_module_address + 0x04D3904;


	G::g_matrix_address = matrix_address;
	G::g_angle_address = angle_address;  //鼠标 xy
	G::g_self_address = self_address;
	G::g_players_address = players_address;
	G::g_players_bar = players_bar;
	G::g_hanle = GetCurrentProcess();
	G::g_hwnd = FindWindowW(L"Valve001", L"Counter-Strike Source");
}

DWORD read_memory(HANDLE process, int address, void* recv, int size)
{
	DWORD read_size;
	//ReadProcessMemory(process, (LPCVOID)address, recv, size, &read_size);
	__try
	{
		RtlCopyMemory(recv, (LPCVOID)address, size);
		read_size = size;
	}
	__except (1)
	{
		RtlZeroMemory(recv, size);
		read_size = 0;
	}
	return read_size;
}


void add_play_list(player_list * pTagrgetPlayers, player_list * pTemp)
{
	Lock lock;

	RtlCopyMemory(pTagrgetPlayers, pTemp, sizeof(player_list));
}

void clear_play_list()
{
	Lock lock;

	nCnt = 0;
	RtlZeroMemory(G::g_players, sizeof(G::g_players));
}


void init_play_list(player_list * players)
{
	int player_base_address = 0;
	int bone_base_address = 0;
	ULONG offset1 = 0;
	ULONG offset2 = 0;
	ULONG offset3 = 0;
	ULONG offset4 = 0;
	player_list pTemp = { 0 };

	clear_play_list();


	for (int i = 0; i < g_players_count; i++)
	{
		player_base_address = 0;
		bone_base_address = 0;
		offset1 = 0;
		offset2 = 0;
		offset3 = 0;
		offset4 = 0;
		RtlZeroMemory(&pTemp, sizeof(player_list));


		read_memory(G::g_hanle, (G::g_players_address + i * 0x10), &player_base_address, sizeof(int));
		if (player_base_address == 0) continue;

		read_memory(G::g_hanle, player_base_address + 血量, &pTemp.blood, sizeof(int));
		if (pTemp.blood <= 0) continue;

		read_memory(G::g_hanle, player_base_address + 更新时机, &pTemp.更新时机EX, sizeof(short));
		if (pTemp.更新时机EX != 0) {
			continue;
		}

		pTemp.effective = true;
		pTemp.aimbot_len = 9999;


		if (read_memory(G::g_hanle, (G::g_players_bar + i * 0x10), &offset1, sizeof(int)))
		{

			read_memory(G::g_hanle, (int)(offset1 + 0x14), &offset2, sizeof(float));
			read_memory(G::g_hanle, (int)(offset2 + 0x44), &offset3, sizeof(float));
			read_memory(G::g_hanle, (int)(offset3 + 0xc0), &offset4, sizeof(float));
			bone_base_address = offset4 + 0xc;

			//为什么 +32 ？ 因为 观测骨骼数据可知  第一个 x距离 与 第二个 x距离 相差 32
			bone_base_address = bone_base_address + 21 * 32;//21是头

			read_memory(G::g_hanle, (bone_base_address), &pTemp.head_bone.x, sizeof(float));
			read_memory(G::g_hanle, (bone_base_address + 16), &pTemp.head_bone.y, sizeof(float));
			read_memory(G::g_hanle, (bone_base_address + 32), &pTemp.head_bone.z, sizeof(float));

		}

		read_memory(G::g_hanle, player_base_address + 坐标X偏移, &pTemp.d3d_pos, sizeof(pTemp.d3d_pos));
		read_memory(G::g_hanle, player_base_address + 阵型偏移, &pTemp.camp, sizeof(int));


		add_play_list(&players[nCnt], &pTemp);
		nCnt++;

	}
	
}

void set_self_camp(player_list* players)
{
	ImVec3 self_pos = { 0 };
	get_self_location(&self_pos);

	for (int i = 0; i < g_players_count; i++)
	{
		if (players[i].effective)
		{

			int temp_x = abs((int)self_pos.x - (int)players[i].d3d_pos.x);
			int temp_y = abs((int)self_pos.y - (int)players[i].d3d_pos.y);
			if (temp_x < 10.0f && temp_y < 10.f)
			{
				//判断 依据
				// 根据坐标x,y相减 ，只要少于10.f ，就认为 是自己

				players[i].self = true;
				//InterlockedExchange((long*)&players[i].self, true);
				break;
			}
		}
	}
}

//获取自己的位置
void get_self_location(ImVec3* location)
{

	int location_base_address = 0;
	read_memory(G::g_hanle, G::g_self_address, &location_base_address, sizeof(int));
	if (location_base_address)
		read_memory(G::g_hanle, location_base_address + 坐标X偏移, location, sizeof(ImVec3));
}




//绘制玩家方框
void render_player_box(player_list* players)
{
	ImVec2 pos = { 0,0 };
	
	LONG window_x = 0; //窗口左顶点x
	LONG window_y = 0;//窗口左顶点y
	LONG window_w = 0;//窗口的宽
	LONG window_h = 0;//窗口的高

	get_window_size(G::g_hwnd, window_x, window_y, window_w, window_h);
	window_w /= 2;
	window_h /= 2;

	RtlZeroMemory(&G::g_matrix, sizeof(Matrix4x4));
	read_memory(G::g_hanle, G::g_matrix_address, &G::g_matrix, sizeof(Matrix4x4));


	int self_camp = get_self_camp(players);


	for (int i = 0; i < g_players_count; i++)
	{
		if (players[i].effective  && players[i].self == false && players[i].blood > 1 && (players[i].更新时机EX == 0) && self_camp != players[i].camp)
		{
			int x=0, y=0, w=0, h=0;
			if (to_rect_info(&G::g_matrix, &players[i].d3d_pos, window_w, window_h, x, y, w, h))
			{
				if (IsPointInWindowsRect({ window_x ,window_y }, window_w * 2, window_h * 2, { x + window_x ,y + window_y }))
				{
					players[i].aimbot_len = get_aimbot_len(window_w, window_h, x + (w / 2), y + (h / 2));

				

					if (Menum::rect) 
					{
						x += window_x;
						y += window_y;

						pDwm->draw_rect(x, y, w, h, 绿色, 2);
					}
				
					if (Menum::point)
					{
						pos = { 0,0 };
						worldToScreen(&G::g_matrix, &players[i].head_bone, window_w, window_h, &pos);

						if (IsPointInWindowsRect({ window_x ,window_y }, window_w * 2, window_h * 2, { (LONG)pos.x + window_x ,(LONG)pos.y + window_y }))
						{
							pos.x += window_x;
							pos.y += window_y;

							pDwm->draw_text(pos.x , pos.y , 绿色, "*");
							//pDwm->DrawCircleFilled(pos.x + window_x, pos.y + window_y, 5.0f,绿色, 10);
						}
						
						
					}


				}
			}
		}
	}

}


bool worldToScreen(IN Matrix4x4 * matrix, IN ImVec3 * world_pos, int window_width, int window_heigt, OUT ImVec2* ScreenPos)
{
	//相机Z

	ImVec2 pos = { 0,0 };

	float to_target = matrix->m[2][0] * world_pos->x
		+ matrix->m[2][1] * world_pos->y
		+ matrix->m[2][2] * world_pos->z
		+ matrix->m[2][3];
	if (to_target < 0.01f)
	{
		ScreenPos->x = 0;
		ScreenPos->y = 0;
		return false;
	}

	to_target = 1.0f / to_target;

	//相机x
	ScreenPos->x = window_width + (matrix->m[0][0] * world_pos->x
		+ matrix->m[0][1] * world_pos->y
		+ matrix->m[0][2] * world_pos->z
		+ matrix->m[0][3]) * to_target * window_width;


	ScreenPos->y = window_heigt - (matrix->m[1][0] * world_pos->x
		+ matrix->m[1][1] * world_pos->y
		+ matrix->m[1][2] * world_pos->z
		+ matrix->m[1][3]) * to_target * window_heigt;



	return true;
}
/*
判断 点是否在窗口范围内
参数一:窗口左顶点
参数二：窗口宽
参数三:窗口高
参数四: 需要判断的点
*/
BOOL IsPointInWindowsRect(POINT wnd_left_pos, ULONG 窗口宽, ULONG 窗口高, POINT target_pos)
{


	if ((target_pos.x > wnd_left_pos.x) && (target_pos.y > wnd_left_pos.y) && (target_pos.x < (wnd_left_pos.x + 窗口宽)) &&
		(target_pos.y < (wnd_left_pos.y + 窗口高)))
	{
		return TRUE;
	}

	return FALSE;
}


//获取窗口位置大小
void get_window_size(HWND target, LONG& x/*左顶点x*/, LONG& y/*左顶点y*/, LONG& width/*窗口宽*/, LONG& height/*窗口高*/)
{
	//返回窗口左顶点 x,y
	x = G::rect.left;
	y = G::rect.top;

	//返回窗口的大小
	width = G::rect.right - G::rect.left;
	height = G::rect.bottom - G::rect.top;

	if (GetWindowLongW(target, GWL_STYLE) & WS_CAPTION)
	{
		x += 8;
		width -= 8;
		y += 30;
		height -= 30;
	}
}

//获取自己的阵营
int get_self_camp(player_list* players)
{
	for (int i = 0; i < g_players_count; i++)
		if (players[i].effective && players[i].self) return players[i].camp;
	return 2;
}

//转化为矩阵信息
bool to_rect_info(Matrix4x4* matrix, ImVec3* d3d_pos, int window_width, int window_heigt, int& x, int& y, int& w, int& h)
{
	//相机Z
	float to_target = matrix->m[2][0] * d3d_pos->x
		+ matrix->m[2][1] * d3d_pos->y
		+ matrix->m[2][2] * d3d_pos->z
		+ matrix->m[2][3];
	if (to_target < 0.01f)
	{
		x = y = w = h = 0;
		return false;
	}

	to_target = 1.0f / to_target;

	//相机x
	float to_width = window_width + (matrix->m[0][0] * d3d_pos->x
		+ matrix->m[0][1] * d3d_pos->y
		+ matrix->m[0][2] * d3d_pos->z
		+ matrix->m[0][3]) * to_target * window_width;

	//相机y
	float to_height_h = window_heigt - (matrix->m[1][0] * d3d_pos->x
		+ matrix->m[1][1] * d3d_pos->y
		+ matrix->m[1][2] * (d3d_pos->z + 75.0f)/*这个+75.f 是要经常 调*/
		+ matrix->m[1][3]) * to_target * window_heigt;

	float to_height_w = window_heigt - (matrix->m[1][0] * d3d_pos->x
		+ matrix->m[1][1] * d3d_pos->y
		+ matrix->m[1][2] * (d3d_pos->z - 5.0f)
		+ matrix->m[1][3]) * to_target * window_heigt;

	x = (int)(to_width - (to_height_w - to_height_h) / 4.0f);
	y = (int)(to_height_h);
	w = (int)((to_height_w - to_height_h) / 2.0f);
	h = (int)(to_height_w - to_height_h);
	return true;
}

//计算自瞄距离
int get_aimbot_len(int window_w, int window_h, int x, int y)
{
	int temp_x = abs(window_w - x);
	int temp_y = abs(window_h - y);
	return (int)sqrt((temp_x * temp_x) + (temp_y * temp_y));
}

BOOL CNot(BOOL a)
{
	if (a)
	{
		return FALSE;
	}

	return TRUE;
}

//获取鼠标左键按下状态
bool get_mouse_left_down()
{
	return GetAsyncKeyState(VK_LBUTTON) & 0x8000;
}

//获取最近的任务骨骼位置
int get_recent_head_location(player_list* players, ImVec3* self_location)
{
	int index = -1;
	int camp = get_self_camp(players);
	for (int i = 0; i < g_players_count; i++)
	{
		if (players[i].effective && camp != players[i].camp)
		{
			if (index == -1) index = i;
			else if (players[index].aimbot_len > players[i].aimbot_len) index = i;
		}
	}
	return index;
}

ULONG write_memory(HANDLE process, int address, void* data, int size)
{
	ULONG write_size = 0;
//	WriteProcessMemory(process, (LPVOID)address, data, size, &write_size);
	__try {
		RtlCopyMemory((LPVOID)address, data, size);
		write_size = size;
	}
	__except (1)
	{
		write_size = 0;
	}


	return write_size;
}

//获取自瞄角度
void get_aimbot_angle(ImVec3* self_location, ImVec3* player_location, float* aim_angle, bool squat, float recoil)
{
	float x = self_location->x - player_location->x;
	float y = self_location->y - player_location->y;
	float z = self_location->z - player_location->z + 65.0f + 15.0f;
	if (squat) z -= 15.0f;
	z += recoil;

	const float pi = 3.1415f;
	aim_angle[0] = (float)atan(z / sqrt(x * x + y * y)) / pi * 180.f;
	aim_angle[1] = (float)atan(y / x);

	if (x >= 0.0f && y >= 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.0f - 180.0f;
	else if (x < 0.0f && y >= 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.0f;
	else if (x < 0.0f && y < 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.0f;
	else if (x >= 0.0f && y < 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.f + 180.0f;
}
//设置当前角度
void set_current_angle(float* angle)
{
	write_memory(G::g_hanle, G::g_angle_address, angle, sizeof(float) * 2);
}
//自瞄开启
void aimbot_players(player_list* player, float max_fov)
{
	
	ImVec3 self_location = { 0 };
	get_self_location(&self_location);

	int aim_index = get_recent_head_location(player, &self_location);
	if (aim_index == -1) return;

	float current_angle[2] = { 0 };
	get_current_angle(current_angle);



	float aim_angle[2] = { 0 };
	get_aimbot_angle(&self_location, &player[aim_index].head_bone, aim_angle, FALSE, -12);

	if (abs((int)aim_angle[0] - (int)current_angle[0]) > max_fov
		|| abs((int)aim_angle[1] - (int)current_angle[1]) > max_fov)
		return;

	set_current_angle(aim_angle);
}
/*
获取 鼠标x
获取 鼠标y
*/
void get_current_angle(float* angle)
{

	read_memory(G::g_hanle, G::g_angle_address, angle, sizeof(float) * 2);
}
bool key_status(ULONG key)
{
	if (GetAsyncKeyState(key) & 1)
	{
		return true;
	}
	return false;
}


void monitor_key()
{

	if (key_status(VK_F1))
	{
		Menum::rect = CNot(Menum::rect);
	}

	if (key_status(VK_F2))
	{
		Menum::point = CNot(Menum::point);
	}
	if (key_status(VK_F3))
	{
		Menum::ambot = CNot(Menum::ambot);
	}
	if (key_status(VK_INSERT))
	{
		Menum::show = CNot(Menum::show);
	}

}

void dram_menum()
{
	if (!Menum::show)
	{
		pDwm->draw_text(50 + G::rect.left, 200 - 30 + G::rect.top, 绿色, "Ins / End ");
		pDwm->draw_text(50 + G::rect.left, 200 + G::rect.top, 绿色, (Menum::rect ? "F1 Rect On" : "F1 Rect Off"));
		pDwm->draw_text(50 + G::rect.left, 200 + 30 + G::rect.top, 绿色, (Menum::point ? "F2 point On" : "F2 point Off"));
		pDwm->draw_text(50 + G::rect.left, 200 + 60 + G::rect.top, 绿色, (Menum::ambot ? "F3 ambot On" : "F3 ambot Off"));
	}
}






void render()
{

	static BOOL bOnce = FALSE;
	if (!bOnce)
	{
		initialize_address();
		bOnce = TRUE;
	}

	GetWindowRect(G::g_hwnd, &G::rect);

	monitor_key();

	init_play_list(G::g_players);

	set_self_camp(G::g_players);

	if (Menum::ambot) {
		if (get_mouse_left_down())
		{
			aimbot_players(G::g_players);
		}
	}


	pDwm->Dwm_StartDraw();

	dram_menum();

	if (Menum::rect || Menum::point) 
	{
		render_player_box(G::g_players);
	}
	pDwm->Dwm_EndDraw();
	Sleep(1);

}

string get_distance_len(ImVec3* pos)
{
	string szTemp;
	ImVec3 self_pos = { 0 };
	get_self_location(&self_pos);

	ULONG temp_x = abs(pos->x - self_pos.x);
	ULONG temp_y = abs(pos->y - self_pos.y);
	ULONG temp_z = abs(pos->z - self_pos.z);


	return std::to_string((ULONG)sqrt((temp_x * temp_x) + (temp_y * temp_y) + (temp_z * temp_z)) - 32);

}
