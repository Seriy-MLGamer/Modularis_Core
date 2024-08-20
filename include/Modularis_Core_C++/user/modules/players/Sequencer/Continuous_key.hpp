/*
(C) 2024 Серый MLGamer. All freedoms preserved.
Дзен: <https://dzen.ru/seriy_mlgamer>
SoundCloud: <https://soundcloud.com/seriy_mlgamer>
YouTube: <https://www.youtube.com/@Seriy_MLGamer>
GitHub: <https://github.com/Seriy-MLGamer>
E-mail: <Seriy-MLGamer@yandex.ru>

This file is part of Modularis Core C++.
Modularis Core C++ is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Modularis Core C++ is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with Modularis Core C++. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <Modularis_Core_C++/user/modules/players/Sequencer/Interpolation.hpp>
#define _USE_MATH_DEFINES
#include <cmath>

namespace MDLRS
{
	struct Continuous_key
	{
		float value;
		float duration;
		Interpolation curve;

		inline Continuous_key(float value, float duration, Interpolation curve);
		inline float get_value(float position);
	};
	Continuous_key::Continuous_key(float value, float duration, Interpolation curve)
	{
		this->value=value;
		this->duration=duration;
		this->curve=curve;
	}
	float Continuous_key::get_value(float position)
	{
		switch (curve)
		{
			case INTERPOLATION_NONE: return value;
			case INTERPOLATION_LINEAR: return value+(this[1].value-value)*position/duration;
			case INTERPOLATION_FAST: return value+(this[1].value-value)*(1-(duration-position)*(duration-position)/(duration*duration));
			case INTERPOLATION_SLOW: return value+(this[1].value-value)*position*position/(duration*duration);
			case INTERPOLATION_SMOOTH: return value+(this[1].value-value)*(1-cosf((float)M_PI*position/duration))/2;
		}
	}
}