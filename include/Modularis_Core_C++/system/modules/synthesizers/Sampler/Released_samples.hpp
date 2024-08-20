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

#include <cstdint>
#include <Modularis_Core_C++/system/modules/synthesizers/Sampler/Pressed_samples.hpp>
#include <Modularis_Core_C++/system/modules/synthesizers/Sampler/Sample.hpp>
#include <cstdlib>

namespace MDLRS
{
	struct Sampler;

	struct Released_samples
	{
		Sample *samples;
		uint32_t samples_size;
		uint32_t samples_count;

		inline Released_samples();
		void operator+=(Sample &sample);
		inline void operator<<(Pressed_samples &pressed);
		inline void operator-=(uint32_t sample);
		void update(Sampler *data);
		inline ~Released_samples();
	};
	Released_samples::Released_samples()
	{
		samples=NULL;
		samples_size=0;
		samples_count=0;
	}
	void Released_samples::operator<<(Pressed_samples &pressed)
	{
		for (uint32_t a=0; a!=pressed.samples_count; a++) if (pressed.samples[a].exists) *this+=pressed.samples[a];
		pressed.~Pressed_samples();
	}
	void Released_samples::operator-=(uint32_t sample)
	{
		uint32_t count=--samples_count;
		for (; sample!=count; sample++) samples[sample]=samples[sample+1];
	}
	Released_samples::~Released_samples()
	{
		if (samples) free(samples);
	}
}