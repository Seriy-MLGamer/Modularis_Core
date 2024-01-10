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

#include <Modularis_Core_C++/modules/synthesizers/Oscillator.hpp>

extern "C" void MDLRS_Oscillator_new_body(MDLRS::Oscillator *self);
extern "C" void MDLRS_Oscillator_process(MDLRS::Oscillator *self);

namespace MDLRS
{
	Oscillator::Oscillator(Modularis &project): Module(project), input(this), volume(this, 1), waveform(this, 0), envelope(this, 0, 0, 1, 0), output(this)
	{
		MDLRS_Oscillator_new_body(this);
	}
	void Oscillator::process()
	{
		MDLRS_Oscillator_process(this);
	}
}