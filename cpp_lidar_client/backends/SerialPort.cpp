#include "SerialPort.h"

SerialPort::SerialPort()
{

}

SerialPort::~SerialPort()
{
	if (connection) {
		connection = false;
		CloseHandle(comport_hdl);
	}
}

bool SerialPort::init(string com_port, DWORD COM_BAUD_RATE, vector<char> delims)
{
	connection = false; 
	com_port.insert(0, "\\\\.\\");
	comport_hdl = CreateFileA(static_cast<LPCSTR>(com_port.c_str()),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	this->delims = delims; 


	if (comport_hdl == INVALID_HANDLE_VALUE) {

		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			printf("Warning: Handle was not attached. Reason: %s not available\n", com_port.c_str());
	}
	else {

		DCB device_control_block = { 0 };

		if (!GetCommState(comport_hdl, &device_control_block)) {

			printf("Warning: Failed to get current serial params");
		}
		else 
		{
			device_control_block.BaudRate = COM_BAUD_RATE;
			device_control_block.ByteSize = 8;
			device_control_block.StopBits = ONESTOPBIT;
			device_control_block.Parity = NOPARITY;
			device_control_block.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(comport_hdl, &device_control_block))
				printf("Warning: could not set serial port params\n");
			else {
				connection = true;
				PurgeComm(comport_hdl, PURGE_RXCLEAR | PURGE_TXCLEAR);
			}
		}
	}

	return connection;
}


bool SerialPort::read_line(int reply_wait_time, string &rx_str)
{
	DWORD bytes_read;
	char rx_byte[1];
	bool delim_detected = false;
	unsigned long start_time = time(nullptr);

	
	rx_str.clear();

	while (!delim_detected && ((time(nullptr) - start_time) < reply_wait_time))
	{
		ClearCommError(comport_hdl, &error, &status);
		Sleep(5 / 100); //delay for 50us
		if (status.cbInQue > 0)
		{
			if (ReadFile(comport_hdl, rx_byte, 1, &bytes_read, NULL))
			{

				for (uint16_t i = 0; i < delims.size(); i++)
				{
					if (rx_byte[0] == delims.at(i))
						delim_detected = true;
				}

				if (!delim_detected)
				{
					rx_str.push_back(rx_byte[0]);
					start_time = time(nullptr);
				}

			}

		}
		
	}

	if (delim_detected)
		return true;
	else
		return false; 
}


bool SerialPort::close()
{
	if (connection) {
		connection = false;
		CloseHandle(comport_hdl);
		return true;
	}
	else
		return false;
}

