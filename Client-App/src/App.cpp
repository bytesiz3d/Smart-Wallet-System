#include "client/App.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

namespace client
{
	App::App()
		: client{}, should_exit{false}
	{
	}

	void
	App::setup_dockspace()
	{
		auto dockspace_id = ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
		if (static bool setup_dockspace = true; setup_dockspace)
		{
			setup_dockspace = false;
			ImGui::DockBuilderRemoveNodeChildNodes(dockspace_id);

			ImGui::DockBuilderDockWindow(CLIENT_MAIN_WINDOW_TITLE, dockspace_id);
		}
	}

	void
	App::main_window()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImGui::GetStyle().ItemSpacing + ImVec2{3.f, 5.f});
		defer { ImGui::PopStyleVar(); };

		defer { ImGui::End(); };
		if (ImGui::Begin(CLIENT_MAIN_WINDOW_TITLE, nullptr, DOCKING_WINDOW_FLAGS) == false)
			return;

		if (client.has_valid_connection() == false)
		{
			ImGui::TextColored({0.10f, 0.04f, 0.07f, 1.00f}, "Couldn't connect to server");
			if (ImGui::Button("Reconnect"))
				client = sws::Client{};

			return;
		}

		static int amount = 0;
		ImGui::InputInt("##Deposit", &amount, 10, 100);

		if (ImGui::Button("Deposit"))
			client.deposit(amount);

		if (ImGui::SameLine(); ImGui::Button("Withdraw"))
			client.withdraw(amount);

		ImGui::Separator();

		ImGui::Text("Client info");
		static char name[255 + 1] = {};
		ImGui::InputText("Name", name, sizeof(name));

		static uint8_t age = 0;
		ImGui::InputScalar("Age", ImGuiDataType_U8, &age);

		static char national_id[14 + 1] = {};
		ImGui::InputText("National ID", national_id, sizeof(national_id), ImGuiInputTextFlags_CharsDecimal);

		static char address[255 + 1] = {};
		ImGui::InputText("Address", address, sizeof(address));

		if (ImGui::Button("Update info"))
		{
			client.update_info({
				.name = name,
				.age = age,
				.national_id = std::stoull(national_id),
				.address = address
			});
		}
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
			return;

		setup_dockspace();
		main_window();
	}

}
