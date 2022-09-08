#pragma once

namespace gui
{
	class IApp
	{
	public:
		virtual void exit() = 0;
		virtual void render_frame() = 0;
	};

	class Window
	{
		using Handle = void*;

		Handle window;
		Handle context;

		bool should_close{false};

		void
		poll_events(IApp *app);

	public:
		Window(const char *title, int width, int height);

		void
		start_loop(IApp *ui);

		~Window();
	};
}
