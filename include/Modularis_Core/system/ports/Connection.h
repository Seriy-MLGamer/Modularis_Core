#pragma once

#include <Modularis_Core/typedefs/system/ports/Connection.h>

#include <Modularis_Core/typedefs/system/ports/Port.h>
#include <stdint.h>

struct MDLRS_Connection
{
	MDLRS_Port *port;
	uint32_t index;
};