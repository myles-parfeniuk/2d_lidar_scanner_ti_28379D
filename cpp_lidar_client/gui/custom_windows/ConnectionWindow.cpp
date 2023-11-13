#include "ConnectionWindow.hpp"

ConnectionWindow::ConnectionWindow():
	connection(false),
	show_connection_error(false)
{
	
}


void ConnectionWindow::show_window(bool* p_open)
{
	static char inputBuffer[7]; 
	std::string comport = "";

	if (!ImGui::Begin("Connection", p_open, 0))
	{
		ImGui::End();
		return;
	}
	else
	{
		ImGui::Text("Connection Status: ");
		ImGui::SameLine();
		if (connection)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1.0f, 0, 1.0f));
			ImGui::Text("connected");
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0, 0, 1.0f));
			ImGui::Text("disconnected");
			ImGui::PopStyleColor();
		}

		ImGui::Separator();

		if (ImGui::Button("Connect"))
		{
			comport = inputBuffer;
			attempt_connect_cb(comport);
		}
		ImGui::SameLine();
	
		ImGui::Text("COM Port:");
		ImGui::SameLine();

		ImGui::SetNextItemWidth(50);
		ImGui::InputText("##comport", inputBuffer, 7);

		if (show_connection_error)
		{
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0, 1.0f));
			ImGui::Text("connection failed");
			ImGui::PopStyleColor();
		}

		ImGui::Separator();

		if (ImGui::Button("Disconnect"))
		{
			if (connection)
			{
				end_connect_cb();
				show_connection_error = false;
			}
		
		}
	}

	ImGui::End();
	return;
}

void ConnectionWindow::set_connection_status(bool status)
{
	connection = status; 

	if (!status)
		show_connection_error = true;
	else
		show_connection_error = false; 

}

void ConnectionWindow::set_attempt_connect_cb(std::function<void(std::string)> cb)
{
	attempt_connect_cb = cb; 
}

void ConnectionWindow::send_end_connect_cb(std::function<void(void)> cb)
{
	end_connect_cb = cb; 
}

bool ConnectionWindow::get_connection_status()
{
	return connection; 
}

