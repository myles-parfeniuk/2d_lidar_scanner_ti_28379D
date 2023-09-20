#include "LidarBackend.hpp"
#include <functional>
#include <iostream>
#include <sstream>

LidarBackend::LidarBackend(GUIManager& gui) :
	gui(gui)
{
	gui.connection_window.set_attempt_connect_cb(std::bind(&LidarBackend::attempt_connection_cb, this, std::placeholders::_1));
	gui.connection_window.send_end_connect_cb(std::bind(&LidarBackend::end_connection_cb, this));
	lidar_thread = std::thread(&LidarBackend::data_grabber_thread, this);
}


void LidarBackend::data_grabber_thread()
{
		std::string rx_line;
		std::istringstream ss;
		std::string token;
		lidar_data_t new_packet = { 0,0,0, "" };
		bool bad_data = false;
		uint8_t i = 0; 
		char buff[100];

		while (1)
		{
			while (gui.connection_window.get_connection_status())
			{

				if (port.read_line(2, rx_line))
				{
					ss.clear();
					ss.str(rx_line);
					i = 0;
					while (ss >> token)
					{
						switch (i)
						{
						case HEADING_BYTE:
							new_packet.heading = stod(token);
							break;

						case RX_DISTANCE_1_BYTE:
							new_packet.rx_data_1 = stod(token);
							break;

						case RX_DISTANCE_2_BYTE:
							new_packet.rx_data_2 = stod(token);
							break;

						default:
							bad_data = true;
							gui.data_window.add_log("bad data", LidarDataWindow::error_log);
							break;
						}

						i++;					
					}

					if (!bad_data)
					{
						snprintf(buff, 100, "Distance 1: %05.1f	Distance 2: %05.1f", new_packet.rx_data_1, new_packet.rx_data_2);
						strcpy_s(new_packet.disp_line, buff);
						newest_packet = new_packet;
						gui.data_window.add_log(newest_packet.disp_line);
						gui.scanner_window.set_new_data(newest_packet);
					}
					else
					{
						bad_data = false;
						gui.data_window.add_log("Bad data detected.", LidarDataWindow::error_log);
					}
		

				}
				else
					port.close();

			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));

		}
}

void LidarBackend::attempt_connection_cb(std::string comport)
{
	port.init(comport.c_str(), (DWORD)3100000, std::vector<char> { '\n', '\r' });
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	gui.connection_window.set_connection_status(port.connection);
}

void LidarBackend::end_connection_cb()
{
	port.close();
	gui.connection_window.set_connection_status(port.connection);
}


