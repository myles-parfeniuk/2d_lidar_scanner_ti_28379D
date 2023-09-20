#pragma once
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <thread>
#include <vector>
#include <string>
#include <functional>
#include <d3d9.h>
#include <tchar.h>

class ConnectionWindow
{
public:
	ConnectionWindow();
	void show_window(bool* p_open);
	bool get_connection_status();
	void set_connection_status(bool status);
	void set_attempt_connect_cb(std::function<void(std::string)> cb);
	void send_end_connect_cb(std::function<void(void)>  cb);
private:
	bool connection; 
	bool show_connection_error; 
	std::function<void(std::string)> attempt_connect_cb;
	std::function<void(void)> end_connect_cb; 
};