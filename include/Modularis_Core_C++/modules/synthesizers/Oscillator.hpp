/*
(C) 2023-2024 Серый MLGamer. All freedoms preserved.
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
#include <Modularis_Core_C++/ports/controllers/Integer_controller.hpp>
#include <Modularis_Core_C++/ports/controllers/ADSR.hpp>
#include <Modularis_Core_C++/ports/Sound.hpp>
#include <Modularis_Core_C++/system/modules/synthesizers/Oscillator/Released_oscillations.hpp>
#include <cstdint>

namespace MDLRS
{
	struct Pressed_oscillations;
	struct Modularis;

	struct Oscillator: Module
	{
		Note input;
		Real_controller volume;
		Integer_controller waveform;
		ADSR envelope;
		Sound output;
		Released_oscillations released;
		Pressed_oscillations *pressed;
		uint32_t pressed_count;

		Oscillator(Modularis &project);
		void on_update();
		~Oscillator();
	};
}