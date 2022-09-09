#include "gui/Window.h"

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

#include <implot.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <fonts/DroidSans.h>

#include <assert.h>

namespace gui
{
	Window::Window(const char *title, int width, int height)
	{
		// Setup SDL
		int sdl_err = SDL_Init(SDL_INIT_VIDEO);
		assert(sdl_err == 0);

		// GL 3.0 + GLSL 130
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		// Create window with graphics context
		SDL_WindowFlags window_flags = (SDL_WindowFlags) (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		SDL_Window *window           = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);
		SDL_GLContext gl_context     = SDL_GL_CreateContext(window);
		SDL_GL_MakeCurrent(window, gl_context);
		SDL_GL_SetSwapInterval(1); // Enable vsync

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking

		// Setup Dear ImGui style
		ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
		ImGui_ImplOpenGL3_Init("#version 130");

		// Load font
		ImFontConfig font_config = {};
		font_config.FontDataOwnedByAtlas = false;
		auto font = io.Fonts->AddFontFromMemoryTTF(DroidSans_ttf, DroidSans_ttf_len, 20.0f, &font_config);
		assert(font);

		// Setup ImPlot
		ImPlot::CreateContext();

		this->window = window;
		this->context = gl_context;
	}

	void
	Window::start_loop(IApp *app)
	{
		constexpr ImVec4 CLEAR_COLOR{0.04f, 0.07f, 0.10f, 1.00f};

		while (this->should_close == false)
		{
			this->poll_events(app);

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			app->render_frame();

			ImGui::Render();

			auto &io = ImGui::GetIO();
			glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);
			glClearColor(CLEAR_COLOR.x * CLEAR_COLOR.w, CLEAR_COLOR.y * CLEAR_COLOR.w, CLEAR_COLOR.z * CLEAR_COLOR.w, CLEAR_COLOR.w);
			glClear(GL_COLOR_BUFFER_BIT);

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			SDL_GL_SwapWindow((SDL_Window*)window);
		}
	}

	void
	Window::poll_events(IApp *app)
	{
		SDL_Event event = {};
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);

			if (event.type == SDL_QUIT)
				this->should_close = true;

			if (event.type == SDL_WINDOWEVENT &&
				event.window.event == SDL_WINDOWEVENT_CLOSE &&
				event.window.windowID == SDL_GetWindowID((SDL_Window*)this->window))
				this->should_close = true;
		}

		if (this->should_close)
			app->exit();
	}

	Window::~Window()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();

		ImPlot::DestroyContext();
		ImGui::DestroyContext();

		SDL_GL_DeleteContext(this->context);
		SDL_DestroyWindow((SDL_Window*)this->window);
		SDL_Quit();
	}
}