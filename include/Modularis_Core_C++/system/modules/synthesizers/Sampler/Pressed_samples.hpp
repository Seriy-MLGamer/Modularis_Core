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
#include <Modularis_Core_C++/system/modules/synthesizers/Sampler/Sample.hpp>
#include <cstdlib>

namespace MDLRS
{
	struct Port;
	struct Released_samples;
	struct Sampler;

	struct Pressed_samples
	{
		Port *connection;
		Sample *samples;
		uint32_t samples_size;
		uint32_t samples_count;

		inline Pressed_samples(Port *connection);
		void start(uint32_t scancode, Released_samples &released, float speed, float frame, float velocity);
		inline void change(uint32_t scancode, float speed, float velocity);
		void stop(uint32_t scancode, Released_samples &released);
		void update(Sampler *data);
		inline ~Pressed_samples();
	};
	Pressed_samples::Pressed_samples(Port *connection)
	{
		this->connection=connection;
		samples=NULL;
		samples_size=0;
		samples_count=0;
	}
	void Pressed_samples::change(uint32_t scancode, float speed, float velocity)
	{
		if (scancode<samples_count) if (samples[scancode].exists) samples[scancode].change(speed, velocity);
	}
	Pressed_samples::~Pressed_samples()
	{
		if (samples) free(samples);
	}
}