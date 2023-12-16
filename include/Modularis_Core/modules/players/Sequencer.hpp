/*
(C) 2023 Серый MLGamer. All freedoms preserved.
Дзен: <https://dzen.ru/seriy_mlgamer>
SoundCloud: <https://soundcloud.com/seriy_mlgamer>
YouTube: <https://www.youtube.com/@Seriy_MLGamer>
GitHub: <https://github.com/Seriy-MLGamer>
E-mail: <Seriy-MLGamer@yandex.ru>

This file is part of Modularis Core.
Modularis Core is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Modularis Core is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with Modularis Core. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <Modularis_Core/modules/system/Module.hpp>

#include <Modularis_Core/ports/controllers/Real_controller.hpp>
#include <Modularis_Core/ports/controllers/Integer_controller.hpp>
#include <Modularis_Core/ports/Note.hpp>
#include <cstdint>
#include <Modularis_Core/modules/system/Sequence.hpp>

namespace MDLRS
{
	struct Modularis;

	struct Sequencer: Module
	{
		Real_controller BPM;
		Integer_controller LPB;
		Integer_controller cursor_position;
		Integer_controller play;
		Note output;
		Sequence *tracks;
		uint32_t tracks_count;
		uint32_t length;
		uint32_t time;
		bool is_playing;
		bool is_position_changed;

		Sequencer(Modularis &project);
		void set_position(uint32_t cursor_position);
		void process();
		inline ~Sequencer();
	};
	Sequencer::~Sequencer()
	{
		disconnect();
		delete[] tracks;
	}
}