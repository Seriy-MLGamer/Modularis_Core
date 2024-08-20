#pragma once

#include <Modularis_Core/typedefs/user/modules/players/Sequencer/Pattern_none.h>
#include <Modularis_Core/system/modules/players/Sequencer/Any_pattern.h>

struct MDLRS_Pattern_none
{
	MDLRS_Any_pattern;
};
void MDLRS_Pattern_none_new(MDLRS_Pattern_none *self, float length);