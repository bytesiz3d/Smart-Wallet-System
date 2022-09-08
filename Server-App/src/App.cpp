#include "server/App.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace server
{
	App::App()
		: server{sws::Server::instance()}, should_exit{false}
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
		if (ImGui::Begin(CLIENTS_LIST_WINDOW_TITLE, nullptr, DOCKING_WINDOW_FLAGS))
		{
			ImGui::Button("App");
		}
		ImGui::End();
	}

	void
	App::clients_data_window()
	{
		if (ImGui::Begin(CLIENT_DATA_WINDOW_TITLE, nullptr, DOCKING_WINDOW_FLAGS))
		{
			ImGui::Button("App");
		}
		ImGui::End();
	}
}