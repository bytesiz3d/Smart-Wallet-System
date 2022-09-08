#include <gui/Window.h>
#include <imgui.h>

struct UI : public gui::IApp
{
	void
	render_frame() override
	{
		ImGui::Button("Yo");
	}
};

// Main code
int
main(int, char **)
{
	gui::Window window{"Playground", 1280, 720};

	UI ui{};

	window.start_loop(&ui);

	return 0;
}