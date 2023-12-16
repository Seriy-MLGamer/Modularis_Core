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

#include <Modularis_Core/ports/Note.hpp>

#include <cstdint>
#include <Modularis_Core/ports/system/Connection.hpp>
#include <cstdlib>
#include <Modularis_Core/ports/system/Note_event.hpp>
#include <Modularis_Core/ports/system/Note_type.hpp>

namespace MDLRS
{
	void Note::process()
	{
		if (connections_count)
		{
			if (!has_connection)
			{
				max_scancode=0;
				has_connection=true;
			}
			uint32_t count=((Note *)connections->port)->events_count;
			if (count)
			{
				if (events)
				{
					if (events_size<count)
					{
						events=(Note_event *)realloc(events, sizeof(Note_event)*count);
						events_size=count;
					}
				}
				else
				{
					events=(Note_event *)malloc(sizeof(Note_event)*count);
					events_size=count;
				}
				Note_event *notes=((Note *)connections->port)->events;
				for (uint32_t a=0; a!=count; a++)
				{
					uint32_t scancode=notes[a].scancode+1;
					max_scancode=(max_scancode>scancode)*max_scancode+(max_scancode<=scancode)*scancode;
					events[a]=notes[a];
				}
				events_count=count;
			}
			else events_count=0;
		}
		else if (has_connection)
		{
			if (events_size<max_scancode)
			{
				events=(Note_event *)realloc(events, sizeof(Note_event)*max_scancode);
				events_size=max_scancode;
			}
			for (uint32_t a=0; a!=max_scancode; a++) events[a]={NOTE_STOP, a};
			events_count=max_scancode;
			has_connection=false;
		}
		else events_count=0;
	}
}