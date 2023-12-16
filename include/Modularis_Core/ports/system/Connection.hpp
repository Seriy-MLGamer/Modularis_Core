#pragma once

#include <cstdint>

namespace MDLRS
{
	struct Port;

	struct Connection
	{
		Port *port;
		uint32_t index;
	};
}