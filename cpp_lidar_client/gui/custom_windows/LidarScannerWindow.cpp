#include "LidarScannerWindow.hpp"

LidarScannerWindow:: LidarScannerWindow()
{
	current_packet = { 0,0,0,"" };
}
void LidarScannerWindow:: show_window(bool* p_open)
{
	std::lock_guard<std::mutex> lock(mutex_current_packet);
	double data[4] = { current_packet.rx_data_1, 0, current_packet.rx_data_2, 0};
	ImGuiIO& io = ImGui::GetIO();
	if (!ImGui::Begin("Scanner", p_open, 0))
	{
		ImGui::End();
		return;
	}
	else
	{

		ImPlot::SetNextAxisLimits(ImAxis_Y1, 0, 60);
		ImPlot::SetNextAxisLimits(ImAxis_X1, 0, 10);
		if (ImPlot::BeginPlot("TF-Mini-S Distance Measured"))
		{
			ImPlot::PlotBars("Distance Plot", data, 4, 0.67, 4, 0, 2, sizeof(double));
			ImPlot::EndPlot();
		}


		ImGui::Separator();

		if (ImGui::Button("Clear Plot"))
		{
			current_packet = { 0,0,0,"" };
		}

		ImGui::Separator();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();

		return;
	}
}

void LidarScannerWindow::set_new_data(lidar_data_t new_packet)
{
	std::lock_guard<std::mutex> lock(mutex_current_packet);
	if (new_packet.rx_data_1 != 0 && new_packet.rx_data_2 != 0)
	{
		current_packet = new_packet;
	}
}