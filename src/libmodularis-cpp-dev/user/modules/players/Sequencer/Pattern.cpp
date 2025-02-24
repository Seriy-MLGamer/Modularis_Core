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

#include <Modularis_Core_C++/user/modules/players/Sequencer/Pattern.hpp>

extern "C"
{
	using namespace MDLRS;

	void
		MDLRS_Pattern_new(Pattern *self, float length, Sequence *tracks, uint32_t tracks_count),
		MDLRS_Pattern_null_position(Pattern *self, Pattern_data &data),
		MDLRS_Pattern_change_position(Pattern *self, Pattern_data &data, float time),
		MDLRS_Pattern_start(Pattern *self, Note &output, Pattern_data &data, uint32_t scancode, float time),
		MDLRS_Pattern_play(Pattern *self, Note &output, Pattern_data &data, uint32_t scancode, float time);
}

namespace MDLRS
{
	Pattern::Pattern(float length, Sequence *tracks, uint32_t tracks_count)
	{
		MDLRS_Pattern_new(this, length, tracks, tracks_count);
	}
	uint32_t Pattern::get_tracks_count()
	{
		return tracks_count;
	}
	void Pattern::null_position(Pattern_data &data)
	{
		MDLRS_Pattern_null_position(this, data);
	}
	void Pattern::change_position(Pattern_data &data, float time)
	{
		MDLRS_Pattern_change_position(this, data, time);
	}
	void Pattern::start(Note &output, Pattern_data &data, uint32_t scancode, float time)
	{
		MDLRS_Pattern_start(this, output, data, scancode, time);
	}
	void Pattern::play(Note &output, Pattern_data &data, uint32_t scancode, float time)
	{
		MDLRS_Pattern_play(this, output, data, scancode, time);
	}
}