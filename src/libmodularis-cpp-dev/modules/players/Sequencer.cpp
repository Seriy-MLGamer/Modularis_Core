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

#include <Modularis_Core_C++/modules/players/Sequencer.hpp>

extern "C" void MDLRS_Sequencer_new_body(MDLRS::Sequencer *self);
extern "C" void MDLRS_Sequencer_add(MDLRS::Sequencer *self, MDLRS::Any_pattern ***tracks, uint32_t tracks_count);
extern "C" void MDLRS_Sequencer_on_update(MDLRS::Sequencer *self);

namespace MDLRS
{
	Sequencer::Sequencer(Modularis &project): Module(project), BPM(this, 120), LPB(this, 8), cursor_position(this, 0), loop(this, 0), play(this, 0), output(this)
	{
		MDLRS_Sequencer_new_body(this);
	}
	void Sequencer::add(Any_pattern ***tracks, uint32_t tracks_count)
	{
		MDLRS_Sequencer_add(this, tracks, tracks_count);
	}
	void Sequencer::on_update()
	{
		MDLRS_Sequencer_on_update(this);
	}
}