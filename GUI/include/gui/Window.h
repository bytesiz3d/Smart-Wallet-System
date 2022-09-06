#pragma once

namespace gui
{
	class IRenderable
	{
	public:
		virtual void render_frame() = 0;
	};

	class Window
	{
		using Handle = void*;

		Handle window;
		Handle context;

		bool should_close;

		void
		poll_events();

	public:
		Window(const char *title, int width, int height);

		void
		start_loop(IRenderable *ui);

		~Window();
	};
}
