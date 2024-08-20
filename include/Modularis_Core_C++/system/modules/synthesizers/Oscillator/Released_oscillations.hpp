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
#include <Modularis_Core_C++/system/modules/synthesizers/Oscillator/Pressed_oscillations.hpp>
#include <Modularis_Core_C++/system/modules/synthesizers/Oscillator/Oscillation.hpp>
#include <cstdlib>

namespace MDLRS
{
	struct ADSR;

	struct Released_oscillations
	{
		Oscillation *oscillations;
		uint32_t oscillations_size;
		uint32_t oscillations_count;

		inline Released_oscillations();
		void operator+=(Oscillation &oscillation);
		inline void operator<<(Pressed_oscillations &pressed);
		inline void operator-=(uint32_t oscillation);
		void update(ADSR &envelope);
		inline ~Released_oscillations();
	};
	Released_oscillations::Released_oscillations()
	{
		oscillations=NULL;
		oscillations_size=0;
		oscillations_count=0;
	}
	void Released_oscillations::operator<<(Pressed_oscillations &pressed)
	{
		for (uint32_t a=0; a!=pressed.oscillations_count; a++) if (pressed.oscillations[a].exists) *this+=pressed.oscillations[a];
		pressed.~Pressed_oscillations();
	}
	void Released_oscillations::operator-=(uint32_t oscillation)
	{
		uint32_t count=--oscillations_count;
		for (; oscillation!=count; oscillation++) oscillations[oscillation]=oscillations[oscillation+1];
	}
	Released_oscillations::~Released_oscillations()
	{
		if (oscillations) free(oscillations);
	}
}