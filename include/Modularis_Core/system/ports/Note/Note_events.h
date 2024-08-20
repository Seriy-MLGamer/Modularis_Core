#pragma once

#include <Modularis_Core/typedefs/system/ports/Note/Note_events.h>

#include <Modularis_Core/typedefs/system/ports/Note/Note_event.h>
#include <stdint.h>

struct MDLRS_Note_events
{
	MDLRS_Note_event *events;
	uint32_t events_size;
	uint32_t events_count;
};