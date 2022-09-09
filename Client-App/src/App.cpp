#include "client/App.h"
#include <sws/Transaction.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

namespace client
{
	void
	App::wait_for_response(sws::Response_Future &&ft)
	{
		ImGui::OpenPopup(response_popup_id);
		response_future = std::move(ft);
		last_response = nullptr;
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
	App::response_popup_modal()
	{
		response_popup_id = ImGui::GetID(RESPONSE_POPUP_MODAL_TITLE);

		constexpr static ImGuiWindowFlags FLAGS = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
		ImGui::SetNextWindowSize({320.f, 180.f}, ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2{0.5f, 0.5f});

		if (ImGui::BeginPopupModal(RESPONSE_POPUP_MODAL_TITLE, nullptr, FLAGS) == false)
			return;
		defer { ImGui::EndPopup(); };

		// Waiting or just finished
		if (response_future.valid())
		{
			if (response_future.is_done() == false) // Waiting
			{
				ImGui::Text("Waiting for server...");
				return;
			}
			else // Just finished
			{
				last_response = response_future.get();
			}
		}

		// Finished, data is in last_response
		if (last_response->error)
		{
			auto err_string = fmt::format("ERROR: {}", last_response->error);
			ImGui::TextColored({1.f, 0.f, 0.f, 1.f}, "%s", err_string.c_str());
		}
		else
		{
			ImGui::TextColored({0.f, 1.f, 0.f, 1.f}, "SUCCESS");

			// Balance query response, contains amount
			if (auto query_balance = dynamic_cast<sws::Response_Query_Balance*>(last_response.get()))
				ImGui::Text("Balance is: %zu", query_balance->get_balance());
		}

		if (ImGui::Button("Ok"))
		{
			last_response = nullptr;
			ImGui::CloseCurrentPopup();
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

		static int64_t amount = 0;
		ImGui::InputScalar("Amount", ImGuiDataType_S64, &amount, nullptr, nullptr, nullptr, ImGuiInputTextFlags_CharsDecimal);

		if (ImGui::Button("Deposit"))
			wait_for_response(client.deposit(amount));

		if (ImGui::SameLine(); ImGui::Button("Withdraw"))
			wait_for_response(client.withdraw(amount));

		if (ImGui::SameLine(); ImGui::Button("Query Balance"))
			wait_for_response(client.query_balance());

		ImGui::Separator();

		ImGui::Text("Client info");
		static char name[255 + 1] = {};
		ImGui::InputText("Name", name, sizeof(name));

		static uint8_t age = 0;
		ImGui::InputScalar("Age", ImGuiDataType_U8, &age, nullptr, nullptr, nullptr, ImGuiInputTextFlags_CharsDecimal);

		static char national_id[14 + 1] = {};
		ImGui::InputText("National ID", national_id, sizeof(national_id), ImGuiInputTextFlags_CharsDecimal);

		static char address[255 + 1] = {};
		ImGui::InputText("Address", address, sizeof(address));

		if (ImGui::Button("Update info"))
		{
			sws::Client_Info new_info{
				.name = name,
				.age = age,
				.address = address
			};
			strcpy(new_info.national_id, national_id);
			wait_for_response(client.update_info(new_info));
		}

		ImGui::Separator();

		if (ImGui::Button("Undo"))
			wait_for_response(client.undo());

		if (ImGui::Button("Redo"))
			wait_for_response(client.redo());
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
		response_popup_modal();
		main_window();
	}
}
