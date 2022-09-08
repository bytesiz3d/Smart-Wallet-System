#pragma once
#include <sws/Client.h>
#include <gui/Window.h>
#include <imgui.h>

namespace client
{
	class App : public gui::IApp
	{
		bool should_exit;
		sws::Client client;

		constexpr static auto CLIENT_MAIN_WINDOW_TITLE = "Client";
		constexpr static ImGuiWindowFlags DOCKING_WINDOW_FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

		void
		setup_dockspace();

		void
		main_window();

	public:
		App();

		void
		exit() override;

		void
		render_frame() override;
	};
}
