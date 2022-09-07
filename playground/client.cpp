#include "sws/TCP.h"
#include <stdio.h>
#include <string.h>

int
main(int argc, char *argv[])
{
    sws::Client client;

	char message[1000] = {};
	while (true)
	{
		printf("CLIENT: "); scanf("%s", message);
        client.send_message({message, strlen(message) + 1});

        auto bytes = client.receive_message();
		if (bytes.size == 0)
            break;

		printf("SERVER: %s\n", bytes.data);
    }
    return 0;
}