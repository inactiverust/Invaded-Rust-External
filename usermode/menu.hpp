#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d9.h>
#include "globals.hpp"
#include <random>

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

std::string random_string()
{
	std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

	std::random_device rd;
	std::mt19937 generator(rd());

	std::shuffle(str.begin(), str.end(), generator);

	return str.substr(0, 10);    // assumes 32 < number of characters in str         
}

namespace menu
{
	void render()
	{
		std::string rand = random_string();
		WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, rand.c_str(), NULL };
		::RegisterClassEx(&wc);
		HWND hwnd = ::CreateWindow(wc.lpszClassName, rand.c_str(), WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, 50, 50, 450, 600, NULL, NULL, wc.hInstance, NULL);
		if (!CreateDeviceD3D(hwnd))
		{
			CleanupDeviceD3D();
			::UnregisterClass(wc.lpszClassName, wc.hInstance);
			return;
		}
		::ShowWindow(hwnd, SW_SHOWDEFAULT);
		::UpdateWindow(hwnd);
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX9_Init(g_pd3dDevice);
		int selectedBone = 0;
		bool done = false;
		while (!done)
		{
			MSG msg;
			while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					done = true;
			}
			if (done)
				break;

			// Start the Dear ImGui frame
			static const char* options[]{ "Head", "Chest", "Leg" };
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2(450, 600));
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

			static const auto dwFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;
			if (!settings::connected)
			{
				ImGui::Begin("Invaded", 0, dwFlags);
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.612, 0.f, 0.f, 1.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.58, 0.f, 0.f, 1.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.612, 0.f, 0.f, 1.f));
					ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.075, 0.078, 0.094, 1.f));
					if (ImGui::Button("Open Game and Then Click Me"))
						settings::connected = true;
					ImGui::PopStyleColor(4);
				}
				ImGui::End();
			}
			else
			{
				ImGui::Begin("Invaded", 0, dwFlags);
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
					ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.075,0.078, 0.094, 1.f));;
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
							ImGui::Text("Aim Bone"); ImGui::SameLine(); ImGui::Combo(" ", &selectedBone, options, IM_ARRAYSIZE(options));
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
						ImGui::TreePop();
					}
					if (ImGui::TreeNode("Visuals"))
					{
						ImGui::Text("ESP"); ImGui::SameLine(); ImGui::ToggleButton("#esptoggle", &settings::esp);
						ImGui::Text("FOV Changer"); ImGui::SameLine(); ImGui::ToggleButton("#fovtoggle", &settings::FOVChanger);
						if (settings::FOVChanger)
							ImGui::SliderFloat("#fovslider", &settings::fov, 10, 150, "%.3f", 0);
						ImGui::Text("Time Changer"); ImGui::SameLine(); ImGui::ToggleButton("#timetoggle", &settings::timeChanger);
						if (settings::timeChanger)
							ImGui::SliderFloat("#timeslider", &settings::time, 0, 24, "%.3f", 0);
						ImGui::Text("Full Bright"); ImGui::SameLine(); ImGui::ToggleButton("#brighttoggle", &settings::full_bright);
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
					if (ImGui::Button("Disconnect From Server"))
					{
						pointers::local_player = 0;
					}
				}
				ImGui::PopStyleColor(12);
				ImGui::End();
			}
			switch (selectedBone)
			{
			case 0:
				settings::aim::aim_bone = BasePlayer::bone_list::head;
				break;
			case 1:
				settings::aim::aim_bone = BasePlayer::bone_list::spine3;
				break;
			case 2:
				settings::aim::aim_bone = BasePlayer::bone_list::pelvis;
			}
			// Rendering
			ImGui::EndFrame();
			g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
			D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
			g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
			if (g_pd3dDevice->BeginScene() >= 0)
			{
				ImGui::Render();
				ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
				g_pd3dDevice->EndScene();
			}
			HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

			// Handle loss of D3D9 device
			if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
				ResetDevice();
		}

		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		CleanupDeviceD3D();
		::DestroyWindow(hwnd);
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
	}
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;
	return true;
}

void CleanupDeviceD3D()
{
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
}