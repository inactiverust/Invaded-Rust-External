#pragma once

#include "dxsdk/d3d9.h"
#include "dxsdk/d3dx9.h"
#include "dxsdk/d3d9_x.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_internal.h"

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

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

	void draw_circle(ImVec2 centre, ImVec4 rgba, float thickness)
	{
		ImGui::GetWindowDrawList()->AddCircle(ImVec2(100, 100), 100, ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), 0, thickness);
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
		UnregisterClassA(("svchost"), NULL);
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
		wc.lpszClassName = "svchost";
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

		Window = CreateWindowExA(NULL, ("svchost"), ("svchost1"), WS_POPUP | WS_VISIBLE, 0, 0, Width, Height, 0, 0, 0, 0);

		DwmExtendFrameIntoClientArea(Window, &Margin);
		SetWindowLong(Window, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
		ShowWindow(Window, SW_SHOW);
		UpdateWindow(Window);
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
		ImGui::End();

		if (GetAsyncKeyState(VK_INSERT) != 0) {  //Menu Key
			Sleep(200);
			toggle = !toggle;
		}

		if (toggle)
		{

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

			if (GetAsyncKeyState(VK_LBUTTON)) {
				io.MouseDown[0] = true;
				io.MouseClicked[0] = true;
				io.MouseClickedPos[0].x = io.MousePos.x;
				io.MouseClickedPos[0].x = io.MousePos.y;
			}
			else
				io.MouseDown[0] = false;

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