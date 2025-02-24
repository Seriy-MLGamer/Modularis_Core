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

#include <Modularis_Core_C++/system/modules/synthesizers/Oscillator/Pressed_oscillations.hpp>

extern "C" void MDLRS_Pressed_oscillations_start(MDLRS::Pressed_oscillations *self, uint32_t scancode, MDLRS::Released_oscillations &released, float phase_speed, float phase, float velocity);
extern "C" void MDLRS_Pressed_oscillations_stop(MDLRS::Pressed_oscillations *self, uint32_t scancode, MDLRS::Released_oscillations &released);

namespace MDLRS
{
	void Pressed_oscillations::start(uint32_t scancode, Released_oscillations &released, float phase_speed, float phase, float velocity)
	{
		MDLRS_Pressed_oscillations_start(this, scancode, released, phase_speed, phase, velocity);
	}
	void Pressed_oscillations::stop(uint32_t scancode, Released_oscillations &released)
	{
		MDLRS_Pressed_oscillations_stop(this, scancode, released);
	}
}