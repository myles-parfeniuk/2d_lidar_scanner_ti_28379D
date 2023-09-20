#pragma once
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "implot.h"
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <d3d9.h>
#include <tchar.h>
#include "../../backends/lidar_data_defs.h"
class LidarScannerWindow
{
	public:
		LidarScannerWindow(); 
		void show_window(bool* p_open);
		void set_new_data(lidar_data_t new_packet);
	private:
		lidar_data_t current_packet; 
		std::mutex mutex_current_packet;

};

