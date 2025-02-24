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

#include <Modularis_Core_C++/system/modules/synthesizers/Oscillator/Released_oscillations.hpp>

extern "C" void MDLRS_Released_oscillations_push(MDLRS::Released_oscillations *self, MDLRS::Oscillation &oscillation);
extern "C" void MDLRS_Released_oscillations_update(MDLRS::Released_oscillations *self, MDLRS::ADSR &envelope);

namespace MDLRS
{
	void Released_oscillations::operator+=(Oscillation &oscillation)
	{
		MDLRS_Released_oscillations_push(this, oscillation);
	}
	void Released_oscillations::update(ADSR &envelope)
	{
		MDLRS_Released_oscillations_update(this, envelope);
	}
}