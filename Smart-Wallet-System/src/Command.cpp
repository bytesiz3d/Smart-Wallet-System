#include "sws/Command.h"

namespace sws
{
	ICommand::ICommand(id_t _client_id)
		: client_id{_client_id}
	{
	}
}