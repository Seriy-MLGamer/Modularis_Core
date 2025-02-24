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

#include <Modularis_Core_C++/system/modules/synthesizers/Sampler/Released_samples.hpp>

extern "C" void MDLRS_Released_samples_push(MDLRS::Released_samples *self, MDLRS::Sample &sample);
extern "C" void MDLRS_Released_samples_update(MDLRS::Released_samples *self, MDLRS::Sampler *data);

namespace MDLRS
{
	void Released_samples::operator+=(Sample &sample)
	{
		MDLRS_Released_samples_push(this, sample);
	}
	void Released_samples::update(Sampler *data)
	{
		MDLRS_Released_samples_update(this, data);
	}
}