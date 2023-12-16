/*
(C) 2023 Серый MLGamer. All freedoms preserved.
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

#include <Modularis_Core/modules/system/Module.hpp>

#include <Modularis_Core/ports/Note.hpp>
#include <Modularis_Core/ports/controllers/Real_controller.hpp>
#include <Modularis_Core/ports/controllers/Integer_controller.hpp>
#include <Modularis_Core/ports/controllers/ADSR.hpp>
#include <Modularis_Core/ports/Sound.hpp>
#include <cstdint>
#include <cstdlib>

namespace MDLRS
{
	struct Oscillation;
	struct Modularis;

	struct Oscillator: Module
	{
		Note input;
		Real_controller volume;
		Integer_controller waveform;
		ADSR envelope;
		Sound output;
		Oscillation *oscillations;
		uint32_t oscillations_size;
		uint32_t oscillations_count;

		Oscillator(Modularis &project);
		void process();
		inline ~Oscillator();
	};
	Oscillator::~Oscillator()
	{
		disconnect();
		if (oscillations) free(oscillations);
	}
}