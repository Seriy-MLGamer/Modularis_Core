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

#include <Modularis_Core_C++/system/ports/Port.hpp>

#include <cstdint>
#include <Modularis_Core_C++/system/ports/Note/Note_event.hpp>
#include <Modularis_Core_C++/system/ports/Note/Note_events.hpp>
#include <cstdlib>

namespace MDLRS
{
	struct Module;

	struct Note: Port
	{
		Note_events *events;
		uint32_t events_size;

		Note(Module *module);
		void on_update();
		void operator+=(Note_event event);
		inline ~Note();
	};
	Note::~Note()
	{
		if (events)
		{
			for (uint32_t a=0; a!=events_size; a++) if (events[a].events) free(events[a].events);
			free(events);
		}
	}
}