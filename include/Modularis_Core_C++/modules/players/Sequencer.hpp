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

#include <Modularis_Core_C++/system/modules/Module.hpp>

#include <Modularis_Core_C++/ports/controllers/Real_controller.hpp>
#include <Modularis_Core_C++/ports/controllers/Integer_controller.hpp>
#include <Modularis_Core_C++/ports/Note.hpp>
#include <cstdint>
#include <cstdlib>

namespace MDLRS
{
	struct Any_pattern;
	struct Patterns_sequence_data;
	struct Pattern_data;
	struct Modularis;

	struct Sequencer: Module
	{
		Real_controller BPM;
		Integer_controller LPB;
		Real_controller cursor_position;
		Integer_controller loop;
		Integer_controller play;
		Note output;
		Any_pattern ***tracks;
		Patterns_sequence_data *tracks_data;
		Pattern_data *patterns_data;
		uint32_t tracks_count;
		uint32_t sequences_count;
		uint32_t time;
		bool is_playing;
		bool is_position_changed;

		Sequencer(Modularis &project);
		void add(Any_pattern ***tracks, uint32_t tracks_count);
		inline void set_position(float cursor_position);
		void on_update();
		inline ~Sequencer();
	};
	void Sequencer::set_position(float cursor_position)
	{
		this->cursor_position.value=cursor_position;
		is_position_changed=true;
	}
	Sequencer::~Sequencer()
	{
		disconnect();
		if (tracks_data) free(tracks_data);
		if (patterns_data) free(patterns_data);
	}
}