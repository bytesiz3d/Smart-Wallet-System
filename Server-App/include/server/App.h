#pragma once

#include <memory>
#include <gui/Window.h>
#include <sws/Server.h>
#include <imgui.h>

namespace server
{
	// TODO: Singleton
	class App : public gui::IApp
	{
		bool should_exit;
		std::shared_ptr<sws::Server> server;

		constexpr static auto CLIENTS_LIST_WINDOW_TITLE = "Clients";
		constexpr static auto CLIENT_DATA_WINDOW_TITLE = "Client Data";
		constexpr static ImGuiWindowFlags DOCKING_WINDOW_FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

		void
		setup_dockspace();

		void
		clients_list_window();

		void
		clients_data_window();

	public:
		App();

		void
		exit() override;

		void
		render_frame() override;
	};
}