/*
(C) 2024 Серый MLGamer. All freedoms preserved.
Дзен: <https://dzen.ru/seriy_mlgamer>
SoundCloud: <https://soundcloud.com/seriy_mlgamer>
YouTube: <https://www.youtube.com/@Seriy_MLGamer>
GitHub: <https://github.com/Seriy-MLGamer>
E-mail: <Seriy-MLGamer@yandex.ru>

This file is part of Modularis Core.
Modularis Core is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Modularis Core is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with Modularis Core. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <Modularis_Core/typedefs/user/modules/players/Sequencer/Note_key.h>
#include <Modularis_Core/user/modules/players/Sequencer/Continuous_key.h>

#include <stdbool.h>
#include <Modularis_Core/user/modules/players/Sequencer/Interpolation.h>
#define _USE_MATH_DEFINES
#include <math.h>

struct MDLRS_Note_key
{
	MDLRS_Continuous_key;

	bool pressed;
};
inline float MDLRS_Note_key_get_value(MDLRS_Note_key *self, float position)
{
	switch (self->curve)
	{
		case INTERPOLATION_NONE: return self->value;
		case INTERPOLATION_LINEAR: return self->value+(self[1].value-self->value)*position/self->duration;
		case INTERPOLATION_FAST: return self->value+(self[1].value-self->value)*(1-(self->duration-position)*(self->duration-position)/(self->duration*self->duration));
		case INTERPOLATION_SLOW: return self->value+(self[1].value-self->value)*position*position/(self->duration*self->duration);
		case INTERPOLATION_SMOOTH: return self->value+(self[1].value-self->value)*(1-cosf((float)M_PI*position/self->duration))/2;
	}
}