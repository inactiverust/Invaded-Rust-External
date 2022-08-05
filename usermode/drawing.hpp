#pragma once
/*
#include "dxsdk/d3d9.h"
#include "dxsdk/d3dx9.h"
#include "dxsdk/d3d9_x.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_internal.h"
#include "globals.hpp"
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <string>
#include <Uxtheme.h>

int wLeft, wTop;
DWORD ScreenCenterX;
DWORD ScreenCenterY;
int Width;
int Height;
HWND hwnd = NULL;
static HWND Window = NULL;
IDirect3D9Ex* p_Object = NULL;
static LPDIRECT3DDEVICE9 D3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9 TriBuf = NULL;
RECT GameRect = { NULL };
D3DPRESENT_PARAMETERS d3dpp;


extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ImGuiShapes
{
	void draw_line(ImVec2 p1, ImVec2 p2, ImVec4 rgba, float thickness)
	{
		ImGui::GetWindowDrawList()->AddLine(p1, p2, ImGui::GetColorU32(rgba), thickness);
	}

	void draw_rect(ImVec2 p1, ImVec2 p2, ImVec4 rgba, float thickness)
	{
		ImGui::GetWindowDrawList()->AddRect(p1, p2, ImGui::GetColorU32(rgba), 0, 0, thickness);
	}

	void draw_circle(ImVec2 centre, float radius, ImVec4 rgba, float thickness)
	{
		ImGui::GetWindowDrawList()->AddCircle(centre, radius, ImGui::GetColorU32(rgba), 0, thickness);
	}

	void draw_center_text(ImVec2 pos, ImVec4 rgba, const char* text, float fs, bool center)
	{
		int len = sizeof(text) / sizeof(char);
		if (fs > 13)
			fs = 13;
		if(center)
			ImGui::GetWindowDrawList()->AddText(ImVec2(pos.x - len * fs / 5, pos.y), ImGui::GetColorU32(rgba), text, NULL, NULL, fs);
		else
			ImGui::GetWindowDrawList()->AddText(ImVec2(pos.x, pos.y), ImGui::GetColorU32(rgba), text, NULL, NULL, fs);
	}

	void draw_filled_rect(ImVec2 min, ImVec2 max, ImVec4 rgba, float rounding)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImGui::GetColorU32(rgba), rounding);
	}
}

namespace drawing {
	void get_hwnd()
	{
		while (hwnd == NULL)
		{
			hwnd = FindWindow(("UnityWndClass"), ("Rust"));
			Sleep(100);
		}
	}

	void shutdown()
	{
		TriBuf->Release();
		D3dDevice->Release();
		p_Object->Release();

		DestroyWindow(Window);
		UnregisterClassA(_("ezscreenrecorder"), NULL);
	}

	LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
			return true;

		switch (Message)
		{
		case WM_DESTROY:
			shutdown();
			PostQuitMessage(0);
			exit(4);
			break;
		case WM_SIZE:
			if (D3dDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				ImGui_ImplDX9_InvalidateDeviceObjects();
				d3dpp.BackBufferWidth = LOWORD(lParam);
				d3dpp.BackBufferHeight = HIWORD(lParam);
				HRESULT hr = D3dDevice->Reset(&d3dpp);
				if (hr == D3DERR_INVALIDCALL)
					IM_ASSERT(0);
				ImGui_ImplDX9_CreateDeviceObjects();
			}
			break;
		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);
			break;
		}
		return 0;
	}

	void snap_window()
	{
		while (true)
		{
			if (hwnd)
			{
				ZeroMemory(&GameRect, sizeof(GameRect));
				GetWindowRect(hwnd, &GameRect);
				Width = GameRect.right - GameRect.left;
				Height = GameRect.bottom - GameRect.top;
				DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);

				if (dwStyle & WS_BORDER)
				{
					GameRect.top += 32;
					Height -= 39;
				}
				ScreenCenterX = Width / 2;
				ScreenCenterY = Height / 2;
				MoveWindow(Window, GameRect.left, GameRect.top, Width, Height, true);
			}
			else
			{
				exit(0);
			}
		}
	}

	const MARGINS Margin = { -1 };

	void create_window()
	{
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)snap_window, 0, 0, 0);

		WNDCLASSEX wc;
		ZeroMemory(&wc, sizeof(wc));
		wc.cbSize = sizeof(wc);
		wc.lpszClassName = _("ezscreenrecorder");
		wc.lpfnWndProc = WinProc;
		RegisterClassEx(&wc);

		if (hwnd)
		{
			GetClientRect(hwnd, &GameRect);
			POINT xy;
			ClientToScreen(hwnd, &xy);
			GameRect.left = xy.x;
			GameRect.top = xy.y;

			Width = GameRect.right;
			Height = GameRect.bottom;
		}
		else
			exit(2);

		Window = CreateWindowExA(NULL, _("ezscreenrecorder"), _("ezscreenrecorder"), WS_POPUP | WS_VISIBLE, 0, 0, Width, Height, 0, 0, 0, 0);

		DwmExtendFrameIntoClientArea(Window, &Margin);
		SetWindowLong(Window, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
		ShowWindow(Window, SW_SHOW);
		UpdateWindow(Window);
	}

	bool is_valid(Vector2 check)
	{
		if (check.x <= 0 || check.y <= 0 || check.x >= Width || check.y >= Width)
			return false;
		return true;
	}

	void InitializeD3D()
	{
		if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
			exit(3);

		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.BackBufferWidth = Width;
		d3dpp.BackBufferHeight = Height;
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		d3dpp.MultiSampleQuality = D3DMULTISAMPLE_NONE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.hDeviceWindow = Window;
		d3dpp.Windowed = TRUE;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

		p_Object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &D3dDevice);

		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;

		ImGui_ImplWin32_Init(Window);
		ImGui_ImplDX9_Init(D3dDevice);

		ImGui::StyleColorsClassic();
		ImGuiStyle* style = &ImGui::GetStyle();

		p_Object->Release();
	}
	bool toggle = false;
	static const char* options[]{ "Head", "Chest", "Leg" };
	int selected;
	void draw_imgui() {
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowSize(ImVec2(Width, Height));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 1.f);
		ImGui::Begin(" ", (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);
		if (settings::aimBot)
			ImGuiShapes::draw_circle(ImVec2(ScreenCenterX, ScreenCenterY), settings::aim::fov, ImVec4(menu_settings::fov_circle_color.x, menu_settings::fov_circle_color.y, menu_settings::fov_circle_color.z, 1), 3.f);

		for (auto& info : vars::playerPosList)
		{
			if (!is_valid(info.HeadScreenPos) || !is_valid(info.ToeScreenPos))
				continue;
			float bheight = info.ToeScreenPos.y - info.HeadScreenPos.y;
			float bwidth = bheight / 2.f;
			const float x = info.ToeScreenPos.x - (bwidth / 2.f);
			const float y = info.HeadScreenPos.y;

			if (info.isTarget)
			{
				ImGuiShapes::draw_line(ImVec2(ScreenCenterX, ScreenCenterY), ImVec2(info.HeadScreenPos.x, info.HeadScreenPos.y + (info.ToeScreenPos.y - info.HeadScreenPos.y)/2), ImVec4(menu_settings::aim_target_color.x, menu_settings::aim_target_color.y, menu_settings::aim_target_color.z, 1), 1.f);
			}
			ImGuiShapes::draw_rect(ImVec2(x, y), ImVec2(x + bwidth, y + bheight), ImVec4(menu_settings::normal_esp_color.x, menu_settings::normal_esp_color.y, menu_settings::normal_esp_color.z, 1), 1.25f);

			if (settings::ESP::show_name && settings::ESP::show_distance)
			{
				ImGuiShapes::draw_center_text(ImVec2(x + bwidth / 2, y + bheight * 1.1), ImVec4(1, 1, 1, 1), info.name.c_str(), bwidth / 1.5, true);
				ImGuiShapes::draw_center_text(ImVec2(x + bwidth / 2, y + bheight * 1.4), ImVec4(1, 1, 1, 1), std::to_string(info.distance).c_str(), bwidth, false);
			}
			else if (settings::ESP::show_name)
			{
				ImGuiShapes::draw_center_text(ImVec2(x + bwidth / 2, y + bheight * 1.1), ImVec4(1, 1, 1, 1), info.name.c_str(), bwidth / 1.5, true);
			}
			else if (settings::ESP::show_distance)
			{
				ImGuiShapes::draw_center_text(ImVec2(x + bwidth / 2, y + bheight * 1.2), ImVec4(1, 1, 1, 1), std::to_string(info.distance).c_str(), bwidth, false);
			}

			if(settings::ESP::show_health)
				ImGuiShapes::draw_line(ImVec2(x + bwidth * 1.25, y + bheight), ImVec2(x + bwidth * 1.25, (y + bheight) - bheight * (info.health / info.max_health)), ImVec4(0, 1, 0, 1), bwidth / 20.f);
		}

		if (settings::ESP::show_panel)
		{
			std::string name = "Name: " + vars::aim_player_info.name;
			std::string distance = "Distance: " + std::to_string(vars::aim_player_info.distance);
			std::string HP = "Health: " + std::to_string(vars::aim_player_info.hp);
			ImGuiShapes::draw_filled_rect(ImVec2(Width * 0.95, Height * 0.05), ImVec2(Width * 0.85, Height * 0.2), ImVec4(.25, .25, .25, 1), 0.f);
			ImGuiShapes::draw_center_text(ImVec2(Width * 0.855, Height * 0.06), ImVec4(1, 1, 1, 1), name.c_str(), 0, false);
			ImGuiShapes::draw_center_text(ImVec2(Width * 0.855, Height * 0.06 + 15), ImVec4(1, 1, 1, 1), distance.c_str(), 0, false);
			ImGuiShapes::draw_center_text(ImVec2(Width * 0.855, Height * 0.06 + 30), ImVec4(1, 1, 1, 1), HP.c_str(), 0, false);
			ImGuiShapes::draw_center_text(ImVec2(Width * 0.855, Height * 0.06 + 45), ImVec4(1, 1, 1, 1), vars::aim_player_info.slot[0].c_str(), 0, false);
			ImGuiShapes::draw_center_text(ImVec2(Width * 0.855, Height * 0.06 + 60), ImVec4(1, 1, 1, 1), vars::aim_player_info.slot[1].c_str(), 0, false);
			ImGuiShapes::draw_center_text(ImVec2(Width * 0.855, Height * 0.06 + 75), ImVec4(1, 1, 1, 1), vars::aim_player_info.slot[2].c_str(), 0, false);
			ImGuiShapes::draw_center_text(ImVec2(Width * 0.855, Height * 0.06 + 90), ImVec4(1, 1, 1, 1), vars::aim_player_info.slot[3].c_str(), 0, false);
			ImGuiShapes::draw_center_text(ImVec2(Width * 0.855, Height * 0.06 + 105), ImVec4(1, 1, 1, 1), vars::aim_player_info.slot[4].c_str(), 0, false);
			ImGuiShapes::draw_center_text(ImVec2(Width * 0.855, Height * 0.06 + 120), ImVec4(1, 1, 1, 1), vars::aim_player_info.slot[5].c_str(), 0, false);
		}
		ImGui::End();

		if (GetAsyncKeyState(VK_INSERT) != 0) {  //Menu Key
			Sleep(200);
			toggle = !toggle;
		}


		if (toggle)
		{
			RECT rc;
			POINT xy;

			ZeroMemory(&rc, sizeof(RECT));
			ZeroMemory(&xy, sizeof(POINT));
			GetClientRect(hwnd, &rc);
			ClientToScreen(hwnd, &xy);
			rc.left = xy.x;
			rc.top = xy.y;

			ImGuiIO& io = ImGui::GetIO();
			io.ImeWindowHandle = hwnd;
			io.DeltaTime = 1.0f / 60.0f;
			POINT p;
			GetCursorPos(&p);
			io.MousePos.x = (float)p.x - (float)xy.x;
			io.MousePos.y = (float)p.y - (float)xy.y;

			if (GetAsyncKeyState(VK_LBUTTON)) {
				io.MouseDown[0] = true;
				io.MouseClicked[0] = true;
				io.MouseClickedPos[0].x = io.MousePos.x;
				io.MouseClickedPos[0].x = io.MousePos.y;
			}
			else
				io.MouseDown[0] = false;


			ImGui::Begin("Invaded", 0, 0);
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.612, 0.f, 0.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.58, 0.f, 0.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.612, 0.f, 0.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.075, 0.078, 0.094, 1.f));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(118.f, 0.f, 0.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.612, 0.f, 0.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.612, 0.f, 0.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.687, 0.f, 0.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.612, 0.f, 0.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.075, 0.078, 0.094, 1.f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.075, 0.078, 0.094, 1.f));
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.075, 0.078, 0.094, 1.f));;
				if (ImGui::TreeNode("Aim"))
				{
					ImGui::Text("Aimbot"); ImGui::SameLine(); ImGui::ToggleButton("#aimtoggle", &settings::aimBot);
					if (settings::aimBot)
					{
						ImGui::HotKey("Aim Key", &keybinds::aimkey, ImVec2(200, 20));
						ImGui::Text("Silent Aim"); ImGui::SameLine(); ImGui::ToggleButton("#silenttoggle", &settings::aim::silent);
						ImGui::Text("Movement Prediction"); ImGui::SameLine(); ImGui::ToggleButton("#predicttoggle", &settings::aim::move_prediction);
						ImGui::Text("Target Wounded"); ImGui::SameLine(); ImGui::ToggleButton("#woundedtoggle", &settings::aim::target_wounded);
						ImGui::Text("Target Sleeping"); ImGui::SameLine(); ImGui::ToggleButton("#sleeptoggle", &settings::aim::target_sleeping);
						ImGui::Text("Target NPC"); ImGui::SameLine(); ImGui::ToggleButton("#npctoggle", &settings::aim::target_npc);
						ImGui::Text("Aim Bone"); ImGui::SameLine(); ImGui::Combo(" ", &selected, options, IM_ARRAYSIZE(options));
						ImGui::Text("Aimbot FOV:");
						ImGui::SliderFloat("#aimfovslider", &settings::aim::fov, 5, 500, "%.3f", 0);
						ImGui::Text("Aimbot Distance:");
						ImGui::SliderFloat("#aimdistslider", &settings::aim::aim_distance, 50, 400, "%.3f", 0);
						switch (selected)
						{
						case 0:
							settings::aim::aim_bone = BasePlayer::head;
							break;
						case 1:
							settings::aim::aim_bone = BasePlayer::spine3;
							break;
						case 2:
							settings::aim::aim_bone = BasePlayer::pelvis;
							break;
						}
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Exploits"))
				{
					ImGui::Text("Admin Flags"); ImGui::SameLine(); ImGui::ToggleButton("#flagtoggle", &settings::adminFlags);
					ImGui::Text("Spiderman"); ImGui::SameLine(); ImGui::ToggleButton("#spidertoggle", &settings::Spiderman);
					ImGui::Text("Super Jump"); ImGui::SameLine(); ImGui::ToggleButton("#supertoggle", &settings::superJump);
					ImGui::Text("Walk on Water"); ImGui::SameLine(); ImGui::ToggleButton("#watertoggle", &settings::waterWalk);
					ImGui::Text("Shoot While Mounted"); ImGui::SameLine(); ImGui::ToggleButton("#mounttoggle", &settings::heliShoot);
					ImGui::Text("No Heavy/Visor"); ImGui::SameLine(); ImGui::ToggleButton("#heavytoggle", &settings::noHeavy);
					ImGui::Text("Chams"); ImGui::SameLine(); ImGui::ToggleButton("#chamtoggle", &settings::chams);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("ESP"))
				{
					ImGui::Text("ESP"); ImGui::SameLine(); ImGui::ToggleButton("#esptoggle", &settings::esp);
					if (settings::esp)
					{
						ImGui::Text("Show Panel"); ImGui::SameLine(); ImGui::ToggleButton("#paneltoggle", &settings::ESP::show_panel);
						ImGui::Text("Show Health Bar"); ImGui::SameLine(); ImGui::ToggleButton("#healthtoggle", &settings::ESP::show_health);
						ImGui::Text("Show Name"); ImGui::SameLine(); ImGui::ToggleButton("#nametoggle", &settings::ESP::show_name);
						ImGui::Text("Show Distance"); ImGui::SameLine(); ImGui::ToggleButton("#disttoggle", &settings::ESP::show_distance);
						ImGui::Text("Show Wounded"); ImGui::SameLine(); ImGui::ToggleButton("#ewoundedtoggle", &settings::ESP::show_wounded);
						ImGui::Text("Show Sleeping"); ImGui::SameLine(); ImGui::ToggleButton("#esleeptoggle", &settings::ESP::show_sleeping);
						ImGui::Text("Show NPC"); ImGui::SameLine(); ImGui::ToggleButton("#enpctoggle", &settings::ESP::show_npc);
						ImGui::Text("ESP Distance:");
						ImGui::SliderFloat("#espdistslider", &settings::ESP::esp_distance, 50, 400, "%.3f", 0);
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Visuals"))
				{
					ImGui::Text("Bright Night"); ImGui::SameLine(); ImGui::ToggleButton("#brighttoggle", &settings::full_bright);
					ImGui::Text("FOV Changer"); ImGui::SameLine(); ImGui::ToggleButton("#fovtoggle", &settings::FOVChanger);
					if (settings::FOVChanger)
						ImGui::SliderFloat("#fovslider", &settings::fov, 10, 150, "%.3f", 0);
					ImGui::Text("Time Changer"); ImGui::SameLine(); ImGui::ToggleButton("#timetoggle", &settings::timeChanger);
					if (settings::timeChanger)
						ImGui::SliderFloat("#timeslider", &settings::time, 0, 24, "%.3f", 0);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Weapon Mods"))
				{
					ImGui::Text("Recoil Control"); ImGui::SameLine(); ImGui::ToggleButton("#recoiltoggle", &settings::noRecoil);
					if (settings::noRecoil)
						ImGui::SliderFloat("#recoilslider", &settings::recoilPercentage, 0, 100, "%.3f", 0);
					ImGui::Text("No Spread"); ImGui::SameLine(); ImGui::ToggleButton("#spreadtoggle", &settings::noSpread);
					ImGui::Text("No Sway"); ImGui::SameLine(); ImGui::ToggleButton("#swaytoggle", &settings::noSway);
					ImGui::Text("Force Auto"); ImGui::SameLine(); ImGui::ToggleButton("#autotoggle", &settings::automatic);
					ImGui::Text("Super Eoka"); ImGui::SameLine(); ImGui::ToggleButton("#eokatoggle", &settings::superEoka);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Misc"))
				{
					float col1[3] = { menu_settings::normal_esp_color.x, menu_settings::normal_esp_color.y, menu_settings::normal_esp_color.z };
					ImGui::ColorEdit3("ESP Color", col1, 0);
					menu_settings::normal_esp_color.x = col1[0]; menu_settings::normal_esp_color.y = col1[1]; menu_settings::normal_esp_color.z = col1[2];
					float col2[3] = { menu_settings::aim_target_color.x, menu_settings::aim_target_color.y, menu_settings::aim_target_color.z };
					ImGui::ColorEdit3("Aim Target Color", col2, 0);
					menu_settings::aim_target_color.x = col2[0]; menu_settings::aim_target_color.y = col2[1]; menu_settings::aim_target_color.z = col2[2];
					float col3[3] = { menu_settings::fov_circle_color.x, menu_settings::fov_circle_color.y, menu_settings::fov_circle_color.z };
					ImGui::ColorEdit3("FOV Color", col3, 0);
					menu_settings::fov_circle_color.x = col3[0]; menu_settings::fov_circle_color.y = col3[1]; menu_settings::fov_circle_color.z = col3[2];
					ImGui::TreePop();
				}
			}
			ImGui::PopStyleColor(12);
			ImGui::End();
		}

		ImGui::EndFrame();

		D3dDevice->SetRenderState(D3DRS_ZENABLE, false);
		D3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		D3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		D3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

		if (D3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			D3dDevice->EndScene();
		}
		HRESULT result = D3dDevice->Present(NULL, NULL, NULL, NULL);

		if (result == D3DERR_DEVICELOST && D3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			D3dDevice->Reset(&d3dpp);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}

	MSG Message = { NULL };
	void loop()
	{
		static RECT old_rc;
		ZeroMemory(&Message, sizeof(MSG));

		while (Message.message != WM_QUIT)
		{
			if (PeekMessage(&Message, Window, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			}

			HWND hwnd_active = GetForegroundWindow();

			if (hwnd_active == hwnd) {
				HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
				SetWindowPos(Window, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}

			RECT rc;
			POINT xy;

			ZeroMemory(&rc, sizeof(RECT));
			ZeroMemory(&xy, sizeof(POINT));
			GetClientRect(hwnd, &rc);
			ClientToScreen(hwnd, &xy);
			rc.left = xy.x;
			rc.top = xy.y;

			ImGuiIO& io = ImGui::GetIO();
			io.ImeWindowHandle = hwnd;
			io.DeltaTime = 1.0f / 60.0f;

			POINT p;
			GetCursorPos(&p);
			io.MousePos.x = (float)p.x - (float)xy.x;
			io.MousePos.y = (float)p.y - (float)xy.y;

			if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
			{
				old_rc = rc;

				Width = rc.right;
				Height = rc.bottom;
				wLeft = rc.left;
				wTop = rc.top;

				d3dpp.BackBufferWidth = Width;
				d3dpp.BackBufferHeight = Height;
				SetWindowPos(Window, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
				D3dDevice->Reset(&d3dpp);
			}
			draw_imgui();
		}
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		DestroyWindow(Window);
	}

}
*/