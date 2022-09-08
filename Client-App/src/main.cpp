#include "client/App.h"
#include <gui/Window.h>

int
main()
{
	gui::Window window{"Smart Wallet System - Client", 1280, 720};
	client::App client{};
	window.start_loop(&client);

	return 0;
}