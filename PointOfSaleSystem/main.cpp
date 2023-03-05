#include "gui.h"

#include <thread>

int __stdcall wWinMain(
	HINSTANCE instance, 
	HINSTANCE previousInstance, 
	PWSTR arguments, 
	int commandShow) 
{

	// create gui
	gui::createHWindow("Cheat menu", "Cheat Menu Class");
	gui::CreateDevice();
	gui::CreateImGui();

	while (gui::exit) 
	{

		gui::BeginRender();
		gui::Render();
		gui::EndRender();
			
		// prevent the loop from running more often than necessary
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	}

	// destroy gui
	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();

	return EXIT_SUCCESS;
}