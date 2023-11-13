#include "LidarDataWindow.hpp"

void LidarDataWindow::show_window(bool* p_open)
{
    bool static auto_scroll = true;

    if (!ImGui::Begin("Lidar Data", p_open, 0))
    {
        ImGui::End();
        return;
    }
    else
    {
        ImGuiIO& io = ImGui::GetIO();


        //options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &auto_scroll);
            ImGui::EndPopup();
        }

        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");

        ImGui::Separator();

      
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::Selectable("Clear"))
                    clear_log(); 

                ImGui::EndPopup();
            }


            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
            
            std::unique_lock<std::mutex> disp_item_lock(mutex_disp_items);

            for (lidar_data_log_t data_log : disp_items)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, data_log.log_level);
                ImGui::TextUnformatted(data_log.display_item.c_str());
                ImGui::PopStyleColor();
            }

            disp_item_lock.unlock(); 



            // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
            // Using a scrollbar or mouse-wheel will take away from the bottom edge.
            if ((auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                ImGui::SetScrollHereY(1.0f);


            ImGui::PopStyleVar();

        }

        ImGui::EndChild();
        ImGui::End();
        return;
    }

}

void LidarDataWindow::add_log(const char* fmt, ImVec4 log_color)
{
    lidar_data_log_t data_log;
    char buff[500];

    snprintf(buff, 500, ">>   %s\n\r", fmt);

    data_log.display_item = buff;
    data_log.log_level = log_color; 
    
    std::unique_lock<std::mutex> disp_item_lock(mutex_disp_items);

    disp_items.push_back(data_log);

    while (disp_items.size() > max_disp_history)
    {
        disp_items.erase(disp_items.begin());
    }

    disp_item_lock.unlock();
}

void LidarDataWindow::clear_log()
{
    std::unique_lock<std::mutex> disp_item_lock(mutex_disp_items);
    disp_items.clear(); 
    disp_item_lock.unlock();
}