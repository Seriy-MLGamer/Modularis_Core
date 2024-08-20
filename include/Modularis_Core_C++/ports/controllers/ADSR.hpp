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

#include <Modularis_Core_C++/ports/Ports_folder.hpp>

#include <Modularis_Core_C++/ports/controllers/Real_controller.hpp>

namespace MDLRS
{
	struct Module;

	struct ADSR: Ports_folder
	{
		Real_controller attack, decay, sustain, release;

		ADSR(Module *module, float attack, float decay, float sustain, float release);
		inline float pressed(float time);
		inline float released(float time);
	};
	float ADSR::pressed(float time)
	{
		if (time<attack.value) return time/attack.value;
		if (time<attack.value+decay.value) return sustain.value+(1-sustain.value)*(1-(time-attack.value)/decay.value);
		return sustain.value;
	}
	float ADSR::released(float time)
	{
		if (time<release.value) return sustain.value*(1-time/release.value);
		return 0;
	}
}