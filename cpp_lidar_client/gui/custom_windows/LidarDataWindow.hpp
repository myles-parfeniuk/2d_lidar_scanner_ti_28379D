#pragma once
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <d3d9.h>
#include <tchar.h>
class LidarDataWindow
{
	public:
		static constexpr ImVec4 error_log = ImVec4(1.0f, 0, 0, 1.0f); //red
		static constexpr ImVec4 warning_log = ImVec4(1.0f, 1.0f, 0, 1.0f); //yellow
		static constexpr ImVec4 normal_log = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); //white
		void show_window(bool* p_open);
		void add_log(const char* fmt, ImVec4 log_color = normal_log);

	private:

		typedef struct lidar_data_log_t
		{
			std::string display_item;
			ImVec4 log_level; 
		};

		void clear_log(); 

		std::mutex mutex_disp_items; 
		std::vector<lidar_data_log_t> disp_items;
		static constexpr uint16_t max_disp_history = 1000;
};

