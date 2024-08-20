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

#include <Modularis_Core_C++/system/modules/synthesizers/Sampler/Pressed_samples.hpp>

extern "C" void MDLRS_Pressed_samples_start(MDLRS::Pressed_samples *self, uint32_t scancode, MDLRS::Released_samples &released, float speed, float frame, float velocity);
extern "C" void MDLRS_Pressed_samples_stop(MDLRS::Pressed_samples *self, uint32_t scancode, MDLRS::Released_samples &released);
extern "C" void MDLRS_Pressed_samples_update(MDLRS::Pressed_samples *self, MDLRS::Sampler *data);

namespace MDLRS
{
	void Pressed_samples::start(uint32_t scancode, Released_samples &released, float speed, float frame, float velocity)
	{
		MDLRS_Pressed_samples_start(this, scancode, released, speed, frame, velocity);
	}
	void Pressed_samples::stop(uint32_t scancode, Released_samples &released)
	{
		MDLRS_Pressed_samples_stop(this, scancode, released);
	}
	void Pressed_samples::update(Sampler *data)
	{
		MDLRS_Pressed_samples_update(this, data);
	}
}