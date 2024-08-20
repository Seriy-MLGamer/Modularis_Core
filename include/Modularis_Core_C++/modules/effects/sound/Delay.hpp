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

#include <Modularis_Core_C++/ports/Sound.hpp>
#include <Modularis_Core_C++/ports/controllers/Real_controller.hpp>
#include <Modularis_Core_C++/system/types/Sound_value.hpp>
#include <cstdint>
#include <cstdlib>

namespace MDLRS
{
	struct Modularis;

	struct Delay: Module
	{
		Sound input;
		Real_controller delay;
		Sound output;
		Sound_value *buffer;
		uint32_t buffer_size;
		uint32_t position;

		Delay(Modularis &project, float delay);
		void on_update();
		inline ~Delay();
	};
	Delay::~Delay()
	{
		disconnect();
		free(buffer);
	}
}