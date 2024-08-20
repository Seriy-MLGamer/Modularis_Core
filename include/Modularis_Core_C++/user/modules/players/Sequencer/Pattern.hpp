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

#include <Modularis_Core_C++/system/modules/players/Sequencer/Any_pattern.hpp>

#include <cstdint>

namespace MDLRS
{
	struct Sequence;
	struct Pattern_data;
	struct Note;

	struct Pattern: Any_pattern
	{
		Sequence *tracks;
		uint32_t tracks_count;

		Pattern(float length, Sequence *tracks, uint32_t tracks_count);
		uint32_t get_tracks_count();
		void
			null_position(Pattern_data &data),
			change_position(Pattern_data &data, float time),
			start(Note &output, Pattern_data &data, uint32_t scancode, float time),
			play(Note &output, Pattern_data &data, uint32_t scancode, float time);
	};
}