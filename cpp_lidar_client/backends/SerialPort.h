#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>

using namespace std;

#define BAUD_RATE 460800

class SerialPort
{

private:
	HANDLE comport_hdl;
	COMSTAT status; //status of comport
	DWORD error; //64 bit integer error code

	vector<char> delims;

public:
	SerialPort();
	~SerialPort();
	bool init(string com_port, DWORD COM_BAUD_RATE, vector<char> delims);
	bool read_line(int reply_wait_time, string& rx_str);
	bool close();
	bool connection;
};