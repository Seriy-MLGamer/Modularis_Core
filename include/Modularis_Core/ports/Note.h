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
#include <Modularis_Core/system/ports/Port.h>

#include <stdint.h>
#include <Modularis_Core/typedefs/system/modules/Module.h>
#include <Modularis_Core/system/ports/Note/Note_event.h>
#include <Modularis_Core/system/ports/Note/Note_events.h>
#include <stdlib.h>

struct MDLRS_Note
{
	MDLRS_Port;

	MDLRS_Note_events *events;
	uint32_t events_size;
};
void MDLRS_Note_new(MDLRS_Note *self, MDLRS_Module *module);
void MDLRS_Note_on_update(MDLRS_Note *self);
void MDLRS_Note_add(MDLRS_Note *self, MDLRS_Note_event event);
inline void MDLRS_Note_remove(MDLRS_Note *self)
{
	if (self->events)
	{
		for (uint32_t a=0; a!=self->events_size; a++) if (self->events[a].events) free(self->events[a].events);
		free(self->events);
	}

	MDLRS_Port_remove((MDLRS_Port *)self);
}