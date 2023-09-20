#pragma once
#include "SerialPort.h"
#include "../gui/GUIManager.hpp"
#include "lidar_data_defs.h"
class LidarBackend
{
	public:
		LidarBackend(GUIManager &gui);

	private:
		enum {HEADING_BYTE, RX_DISTANCE_1_BYTE, RX_DISTANCE_2_BYTE};

		std::thread lidar_thread;
		void attempt_connection_cb(std::string comport);
		void end_connection_cb();
		void data_grabber_thread(); 
		GUIManager& gui; 
		SerialPort port;
		lidar_data_t newest_packet;

};

