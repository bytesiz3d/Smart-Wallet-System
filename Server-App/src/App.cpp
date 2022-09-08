#include "server/App.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

namespace server
{
	App::App()
		: server{sws::Server::instance()}, should_exit{false}, active_client_id{id_t(-1)}
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
		}
	}

	void
	App::clients_list_window()
	{
		defer { ImGui::End(); };
		if (ImGui::Begin(CLIENTS_LIST_WINDOW_TITLE, nullptr, DOCKING_WINDOW_FLAGS) == false)
			return;

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::Button("Clients", ImVec2{-FLT_MIN, 0.f});
		ImGui::PopItemFlag();

		bool active_client_found = false;
		auto active_clients = server->clients();
		for (auto id : active_clients)
		{
			auto label = fmt::format("Client #{}", id);
			if (ImGui::Selectable(label.c_str(), id == active_client_id))
				active_client_id = id;

			if (id == active_client_id)
				active_client_found = true;
		}

		if (active_client_found == false)
			active_client_id = active_clients.empty() ? -1 : active_clients[0];
	}

	void
	App::clients_data_window()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImGui::GetStyle().ItemSpacing + ImVec2{3.f, 5.f});
		defer { ImGui::PopStyleVar(); };

		defer { ImGui::End(); };
		if (ImGui::Begin(CLIENT_DATA_WINDOW_TITLE, nullptr, DOCKING_WINDOW_FLAGS) == false)
			return;

		if (active_client_id == id_t(-1))
		{
			ImGui::Text("No client selected");
			return;
		}

		// TODO: Use an Observer/Detector
		auto client_data = server->client_data(active_client_id);

		float width = ImGui::GetContentRegionAvail().x;
		if (ImGui::BeginChild("##Data", ImVec2{width/2, 0.f}, true))
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			{
				ImGui::InputScalar("Client ID", ImGuiDataType_U32, &client_data.client_id);

				ImGui::InputText("Name", client_data.name.data(), client_data.name.size());
				ImGui::InputScalar("Age", ImGuiDataType_U8, &client_data.age);

				auto national_id_str = fmt::format("{:014}", client_data.national_id);
				ImGui::InputText("National ID", national_id_str.data(), national_id_str.size());

				ImGui::InputText("Address", client_data.address.data(), client_data.address.size());
				ImGui::InputScalar("Balance", ImGuiDataType_U32, &client_data.balance);
			}
			ImGui::PopItemFlag();
		}
		ImGui::EndChild();

		ImGui::SameLine();

		if (ImGui::BeginChild("##Logs", ImVec2{}, true))
		{
			for (auto &log: client_data.logs)
				ImGui::Text("> %s", log.c_str());
		}
		ImGui::EndChild();
	}
}