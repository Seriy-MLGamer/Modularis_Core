/*
(C) 2023-2024 Серый MLGamer. All freedoms preserved.
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

#include <Modularis_Core/typedefs/ports/Note.h>
#include <Modularis_Core/ports/system/Port.h>

#include <stdint.h>
#include <stdbool.h>
#include <Modularis_Core/typedefs/modules/system/Module.h>
#include <Modularis_Core/ports/system/Note_event.h>
#include <stdlib.h>

struct MDLRS_Note
{
	MDLRS_Port;

	MDLRS_Note_event *events;
	uint32_t events_size;
	uint32_t events_count;
	uint32_t max_scancode;
	bool has_connection;
};
void MDLRS_Note_new(MDLRS_Note *self, MDLRS_Module *module);
void MDLRS_Note_process(MDLRS_Note *self);
inline void MDLRS_Note_add(MDLRS_Note *self, MDLRS_Note_event event)
{
	if (self->events_size)
	{
		if (self->events_count==self->events_size) self->events=(MDLRS_Note_event *)realloc(self->events, sizeof(MDLRS_Note_event)*(self->events_size+=8));
		self->events[self->events_count++]=event;
	}
	else
	{
		self->events=(MDLRS_Note_event *)malloc(sizeof(MDLRS_Note_event)*8);
		*self->events=event;
		self->events_size=8;
		self->events_count=1;
	}
}
inline void MDLRS_Note_remove(MDLRS_Note *self)
{
	if (self->events) free(self->events);

	MDLRS_Port_remove((MDLRS_Port *)self);
}