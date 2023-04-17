#include "gui.h"
#include "../imgui_imports/imgui.h"
#include "../imgui_imports/imgui_impl_dx9.h"
#include "../imgui_imports/imgui_impl_win32.h"

#include <iostream>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);

long __stdcall WindowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter) {

	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message) {

		case WM_SIZE: {

			if (gui::device && wideParameter != SIZE_MINIMIZED) {
				gui::presetParameters.BackBufferWidth = LOWORD(longParameter);
				gui::presetParameters.BackBufferHeight = LOWORD(longParameter);
				gui::ResetDevice();
			}
		}return 0;

		case WM_SYSCOMMAND: {

			if ((wideParameter && 0xfff0) == SC_KEYMENU)	// disable ALT application menu
				return 0;

		}break;

		case WM_DESTROY: {
			PostQuitMessage(0);
		}return 0;

		case WM_LBUTTONDOWN: {
			gui::position = MAKEPOINTS(longParameter);	//set click points
		}return 0;

		case WM_MOUSEMOVE: {
			
			if (wideParameter == MK_LBUTTON) {

				const auto points = MAKEPOINTS(longParameter);
				auto rect = ::RECT{ };

				GetWindowRect(gui::window, &rect);

				// update window position
				rect.left += points.x - gui::position.x;
				rect.top += points.y - gui::position.y;

				if (gui::position.x >= 0 && gui::position.x <= gui::WIDTH && gui::position.y >= 0 && gui::position.y <= 19) {

					SetWindowPos(gui::window, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);

				}

			}

		}
	}

	return DefWindowProc(window, message, wideParameter, longParameter);
}

void gui::createHWindow(const char* windowName, const char* className) noexcept {

	windowClass.cbSize = sizeof(tagWNDCLASSEXA);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassExA(&windowClass);

	// set up the window
	window = CreateWindow(
		className, 
		windowName, 
		WS_POPUP,
		100, 
		100,
		WIDTH,
		HEIGHT,
		0,
		0,
		windowClass.hInstance,
		0
	);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept 
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept 
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&presetParameters, sizeof(presetParameters));

	// establish present parameters
	presetParameters.Windowed = TRUE;
	presetParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presetParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presetParameters.EnableAutoDepthStencil = TRUE;
	presetParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presetParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	// if CreateDevice returns a value < 0, it failed
	if (d3d->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			window,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&presetParameters,
			&device) < 0) 
		return false;
	
	return true;


}

// called whenever the window is resized or when an update is needed
void gui::ResetDevice() noexcept 
{

	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presetParameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();

}


void gui::DestroyDevice() noexcept 
{

	if (device) {
		device->Release();
		device = nullptr;
	}

	if (d3d) {
		d3d->Release();
		d3d = nullptr;
	}

}

// initialize ImGui context
void gui::CreateImGui() noexcept 
{

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.IniFilename = NULL;	// prevents ImGui.ini file
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;	// enable multiple viewports

	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::StyleColorsDark();
	style.WindowRounding = 0.0f;
	
	// specify colors if viewports are enabled
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}


	// make main viewport
	/*const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
	const ImVec2 base_pos = main_viewport->Pos;*/


	// handle child window creation and buffer swapping
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	device->Present(NULL, NULL, NULL, NULL);

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

}

void gui::CreateSecondaryViewportContext() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.IniFilename = NULL;	// prevents ImGui.ini file
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;	// enable multiple viewports

	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::StyleColorsDark();
	style.WindowRounding = 0.0f;

	// specify colors if viewports are enabled
	/*if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}*/


	// make main viewport
	//const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	//ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
	//ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
	//const ImVec2 base_pos = main_viewport->Pos;


	// handle child window creation and buffer swapping
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	device->Present(NULL, NULL, NULL, NULL);

	//ImGui_ImplWin32_Init(&main_viewport);
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

// destroy ImGui 
void gui::DestroyImGui() noexcept 
{

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}

void gui::BeginRender() noexcept 
{

	MSG message;

	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}


	// start the ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Update and Render additional Platform Windows
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

}

void gui::EndRender() noexcept 
{

	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	// Handle the loss of the D3D9 device
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}

void gui::Render() noexcept 
{

	ImGuiIO& io = ImGui::GetIO();

	ImGui::SetNextWindowPos({0, 0});
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin(
		"This is a window",
		&exit,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove
	);

	ImGui::Button("300 lines of code for this?");

	if (ImGui::Button("Caramel Macchiato")) {
		ImGui::Button("The click worked!");
	}
	

	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	ImGui::End();

}