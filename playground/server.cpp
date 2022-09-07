#include "sws/TCP.h"
#include <stdio.h>
#include <thread>

void
serve(sws::tcp::Connection client, size_t id)
{
	while (true)
	{
        auto bytes = client.receive_message();
		if (bytes.size == 0)
            break;

        printf("[#%zu] %s\n", id, bytes.data);
        client.send_message(bytes);
    }
}

int
main()
{
    sws::tcp::Server server;

    std::thread threads[8] = {};
    for (size_t i = 0; i < 8; i++)
    {
        threads[i] = std::thread(serve, std::move(server.accept()), i);
	}
    for (size_t i = 0; i < 8; i++)
    {
        threads[i].join();
    }

    return 0;
}