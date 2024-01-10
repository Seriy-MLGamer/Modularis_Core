#pragma once

#include <Modularis_Core/typedefs/ports/system/Connection.h>

#include <Modularis_Core/typedefs/ports/system/Port.h>
#include <stdint.h>

struct MDLRS_Connection
{
	MDLRS_Port *port;
	uint32_t index;
};