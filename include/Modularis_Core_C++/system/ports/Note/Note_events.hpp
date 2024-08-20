#pragma once

#include <cstdint>

namespace MDLRS
{
	struct Note_event;

	struct Note_events
	{
		Note_event *events;
		uint32_t events_size;
		uint32_t events_count;
	};
}