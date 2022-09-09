#include "server/App.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <implot.h>
#include <imgui_internal.h>

namespace server
{
	App::App()
		: server{sws::Server::instance()}, should_exit{false}, selected_client_id{-1}
	{
	}

	void
	App::exit()
	{
		should_exit = true;
	}

	void
	App::render_frame()
	{
		server->update_state();
		if (should_exit)
		{
			server->stop_listening_for_connections();
			return;
		}

		setup_dockspace();
		clients_list_window();
		clients_data_window();
		//statistics_window();
	}

	void
	App::setup_dockspace()
	{
		auto dockspace_id = ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
		if (static bool setup_dockspace = true; setup_dockspace)
		{
			setup_dockspace = false;
			ImGui::DockBuilderRemoveNodeChildNodes(dockspace_id);

			ImGuiID left_id, right_id;
			ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.3f, &left_id, &right_id);

			ImGui::DockBuilderDockWindow(CLIENTS_LIST_WINDOW_TITLE, left_id);
			ImGui::DockBuilderDockWindow(CLIENT_DATA_WINDOW_TITLE, right_id);
			ImGui::DockBuilderDockWindow(STATISTICS_WINDOW_TITLE, right_id);
		}
	}

	void
	App::clients_list_window()
	{
		defer { ImGui::End(); };
		if (ImGui::Begin(CLIENTS_LIST_WINDOW_TITLE, nullptr, DOCKING_WINDOW_FLAGS) == false)
			return;

		bool selected_client_found = false;
		auto registered_clients = server->clients();
		for (auto [id, active] : registered_clients)
		{
			auto label = fmt::format("Client #{}", id);

			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::RadioButton("##", active);
			ImGui::PopItemFlag();
			ImGui::SameLine();

			if (ImGui::Selectable(label.c_str(), id == selected_client_id))
				selected_client_id = id;

			if (id == selected_client_id)
				selected_client_found = true;
		}

		if (selected_client_found == false)
			selected_client_id = registered_clients.empty() ? -1 : registered_clients[0].first;
	}

	void
	App::clients_data_window()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImGui::GetStyle().ItemSpacing + ImVec2{3.f, 5.f});
		defer { ImGui::PopStyleVar(); };

		defer { ImGui::End(); };
		if (ImGui::Begin(CLIENT_DATA_WINDOW_TITLE, nullptr, DOCKING_WINDOW_FLAGS) == false)
			return;

		if (selected_client_id == -1)
		{
			ImGui::Text("No client selected");
			return;
		}

		// TODO: Use an Observer/Detector
		auto client_data = server->client_data(selected_client_id);

		// Vertical split
		if (ImGui::BeginTable("##Client_Data_Logs", 2, ImGuiTableFlags_BordersInnerV))
		{
			ImGui::TableNextColumn();
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			{
				ImGui::InputScalar("Client ID", ImGuiDataType_U32, &client_data.client_id);

				ImGui::InputText("Name", client_data.name.data(), client_data.name.size());
				ImGui::InputScalar("Age", ImGuiDataType_U8, &client_data.age);

				ImGui::InputText("National ID", client_data.national_id, sizeof(client_data.national_id));

				ImGui::InputText("Address", client_data.address.data(), client_data.address.size());
				ImGui::InputScalar("Balance", ImGuiDataType_U32, &client_data.balance);
			}
			ImGui::PopItemFlag();

			ImGui::TableNextColumn();
			if (ImGui::BeginChild("##Logs", ImVec2{}, false))
			{
				for (auto &log: client_data.logs)
					ImGui::Text("> %s", log.c_str());
			}
			ImGui::EndChild();

			ImGui::EndTable();
		}
	}

	void
	App::statistics_window()
	{
		defer { ImGui::End(); };
		if (ImGui::Begin(STATISTICS_WINDOW_TITLE, nullptr, DOCKING_WINDOW_FLAGS) == false)
			return;

		static const char *labels[] = {"A", "B", "C", "D", "E"};
		static int data[]           = {1, 1, 2, 3, 5};
		if (ImPlot::BeginPlot("##Pie2", ImVec2{-1, -1}, ImPlotFlags_Equal | ImPlotFlags_NoMouseText))
		{
			ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
			ImPlot::SetupAxesLimits(0, 1, 0, 1);
			ImPlot::PlotPieChart(labels, data, 5, 0.5, 0.5, 0.4, true, "%.0f", 180);
			ImPlot::EndPlot();
		}
	}
}