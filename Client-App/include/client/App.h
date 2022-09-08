#pragma once
#include <sws/Client.h>
#include <gui/Window.h>
#include <imgui.h>

namespace client
{
	class App : public gui::IApp
	{
		bool should_exit{false};
		sws::Client client;

		sws::Response_Future response_future;
		std::unique_ptr<sws::IResponse> last_response;
		ImGuiID response_popup_id{};

		constexpr static auto CLIENT_MAIN_WINDOW_TITLE = "Client";
		constexpr static auto RESPONSE_POPUP_MODAL_TITLE = "Sending request to server...";
		constexpr static ImGuiWindowFlags DOCKING_WINDOW_FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

		void
		wait_for_response(sws::Response_Future &&ft);

		void
		setup_dockspace();

		void
		response_popup_modal();

		void
		main_window();

	public:
		App() = default;

		void
		exit() override;

		void
		render_frame() override;
	};
}
