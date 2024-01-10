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

#include <Modularis_Core_C++/ports/system/Port.hpp>

#include <cstdint>
#include <Modularis_Core_C++/ports/system/Note_event.hpp>
#include <cstdlib>

namespace MDLRS
{
	struct Module;

	struct Note: Port
	{
		Note_event *events;
		uint32_t events_size;
		uint32_t events_count;
		uint32_t max_scancode;
		bool has_connection;

		Note(Module *module);
		void process();
		inline void add(Note_event event);
		inline ~Note();
	};
	void Note::add(Note_event event)
	{
		if (events_size)
		{
			if (events_count==events_size) events=(Note_event *)realloc(events, sizeof(Note_event)*(events_size+=8));
			events[events_count++]=event;
		}
		else
		{
			events=(Note_event *)malloc(sizeof(Note_event)*8);
			*events=event;
			events_size=8;
			events_count=1;
		}
	}
	Note::~Note()
	{
		if (events) free(events);
	}
}