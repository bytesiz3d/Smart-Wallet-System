#include "server/App.h"
#include <gui/Window.h>

int
main()
{
	gui::Window window{"Smart Wallet System - App", 1280, 720};
	server::App server{};
	window.start_loop(&server);

	return 0;
}