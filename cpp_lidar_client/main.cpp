#include <iostream>
#include <string.h>
#include <vector>
#include <thread>
#include "gui/GUIManager.hpp"
#include "backends/LidarBackend.hpp"


// Main code
int main(int, char**)
{
	GUIManager gui;
	LidarBackend lidar(gui);
	gui.start_gui();

	while (gui.running)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

	}


	return 0;
}

