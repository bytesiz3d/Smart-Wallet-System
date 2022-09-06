#include <gui/Window.h>
#include <imgui.h>
#include <sws/Message.h>

struct UI : public gui::IRenderable
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
	gui::Window window("Playground", 1280, 720);

	UI ui{};

	sws::IRequest *req = nullptr;
	//req->execute();

	window.start_loop(&ui);

	return 0;
}