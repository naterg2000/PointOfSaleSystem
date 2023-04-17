#include "gui.h"
#include "imgui.h"
#include "Drink.h"

#include <iostream>
#include <thread>
#include <string>

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
	gui::CreateSecondaryViewportContext();


	while (gui::exit) 
	{

		gui::BeginRender();
		std::cout << "hi\n";
		gui::Render();
		std::cout << "hi\n";
		gui::EndRender();
		std::cout << "hi\n";
			
		// prevent the loop from running more often than necessary
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	}

	

	// destroy gui
	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();

	

	return EXIT_SUCCESS;
}
