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

#include <Modularis_Core_C++/system/modules/Module.hpp>

#include <Modularis_Core_C++/ports/Note.hpp>
#include <Modularis_Core_C++/ports/controllers/Real_controller.hpp>
#include <Modularis_Core_C++/ports/controllers/ADSR.hpp>
#include <Modularis_Core_C++/ports/controllers/Integer_controller.hpp>
#include <Modularis_Core_C++/system/modules/synthesizers/Sampler/Released_samples.hpp>
#include <Modularis_Core_C++/system/types/Sound_value.hpp>
#include <cstdint>

namespace MDLRS
{
	struct Pressed_samples;
	struct Sound;
	struct Modularis;

	struct Sampler: Module
	{
		Note input;
		Real_controller volume;
		ADSR envelope;
		Integer_controller loop;
		Integer_controller loop_start;
		Integer_controller loop_length;
		Released_samples released;
		Pressed_samples *pressed;
		Sound *outputs_array;
		Sound_value *sample;
		uint32_t pressed_count;
		uint32_t sample_rate;
		uint32_t length;
		unsigned channels;

		Sampler(Modularis &project, const char *file);
		void on_update();
		~Sampler();
	};
}