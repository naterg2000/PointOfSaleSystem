#pragma once

#include <d3d9.h>

namespace gui 
{

	// constant window size
	constexpr int WIDTH = 500;
	constexpr int HEIGHT = 300;

	inline bool exit = true;

	// winapi window cars
	inline HWND window = nullptr;
	inline WNDCLASSEXA windowClass = { };

	// points for window moncement
	inline POINTS position = { };

	// direct x state var
	inline PDIRECT3D9 d3d = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS presetParameters = { };

	// handle creation and destruction of window
	void createHWindow(const char* windowName, const char* className) noexcept;
	void DestroyHWindow() noexcept;

	// handle device creation and desctruction
	bool CreateDevice() noexcept;
	void ResetDevice() noexcept;
	void DestroyDevice() noexcept;

	// handle ImGui creation and destruction
	void CreateImGui() noexcept;
	void DestroyImGui() noexcept;

	void BeginRender() noexcept;
	void EndRender() noexcept;
	void Render() noexcept;

}