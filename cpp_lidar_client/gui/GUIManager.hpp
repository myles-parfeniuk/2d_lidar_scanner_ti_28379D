#pragma once
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <thread>
#include <vector>
#include <string>
#include <d3d9.h>
#include <tchar.h>

#include "custom_windows/LidarDataWindow.hpp"
#include "custom_windows/ConnectionWindow.hpp"
#include "custom_windows/LidarScannerWindow.hpp"



// Forward declarations of helper functions


class GUIManager
{
	public:
		GUIManager();
		~GUIManager();
		void start_gui();
		bool running; 
		//windows
		LidarDataWindow data_window;
		LidarScannerWindow scanner_window; 
		ConnectionWindow connection_window; 

	private:

		
		//main thread
		void gui_core();
		std::thread gui_thread; 
		//gui creation functions
		void show_dock_space();
		//sub-menu creation functions
		void show_view_menu();

		//helper functions
		bool CreateDeviceD3D(HWND hWnd);
		void CleanupDeviceD3D();
		void ResetDevice();

		//window open/close statuses
		bool open_lidar_data_window;
		bool open_connection_window; 
		bool open_lidar_scanner_window; 



		//gui data
		static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LPDIRECT3D9 g_pD3D;
		static LPDIRECT3DDEVICE9 g_pd3dDevice;
		static D3DPRESENT_PARAMETERS g_d3dpp;
		static UINT g_ResizeWidth;
		static UINT g_ResizeHeight;
	};

