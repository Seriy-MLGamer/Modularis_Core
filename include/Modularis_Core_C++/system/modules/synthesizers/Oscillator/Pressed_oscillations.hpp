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
#include <Modularis_Core_C++/system/modules/synthesizers/Oscillator/Oscillation.hpp>
#include <cstdlib>

namespace MDLRS
{
	struct Port;
	struct Released_oscillations;

	struct Pressed_oscillations
	{
		Port *connection;
		Oscillation *oscillations;
		uint32_t oscillations_size;
		uint32_t oscillations_count;

		inline Pressed_oscillations(Port *connection);
		void start(uint32_t scancode, Released_oscillations &released, float phase_speed, float phase, float velocity);
		inline void change(uint32_t scancode, float phase_speed, float velocity);
		void stop(uint32_t scancode, Released_oscillations &released);
		inline void update();
		inline ~Pressed_oscillations();
	};
	Pressed_oscillations::Pressed_oscillations(Port *connection)
	{
		this->connection=connection;
		oscillations=NULL;
		oscillations_size=0;
		oscillations_count=0;
	}
	void Pressed_oscillations::change(uint32_t scancode, float phase_speed, float velocity)
	{
		if (scancode<oscillations_count) if (oscillations[scancode].exists) oscillations[scancode].change(phase_speed, velocity);
	}
	void Pressed_oscillations::update()
	{
		for (uint32_t a=0; a!=oscillations_count; a++) if (oscillations[a].exists) oscillations[a].update();
	}
	Pressed_oscillations::~Pressed_oscillations()
	{
		if (oscillations) free(oscillations);
	}
}