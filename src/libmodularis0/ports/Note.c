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

#include <Modularis_Core/ports/Note.h>

#include <stddef.h>
#include <stdlib.h>
#include <Modularis_Core/system/ports/Note/Note_events.h>
#include <string.h>
#include <stdint.h>
#include <Modularis_Core/system/ports/Connection.h>
#include <Modularis_Core/system/ports/Note/Note_event.h>
#include <Modularis_Core/system/ports/Note/Note_type.h>

static struct MDLRS_Port_f f=
{
	(int (*)(void *, MDLRS_Any_port *))MDLRS_Port_connect,
	(int (*)(void *, MDLRS_Port *))MDLRS_Port_connect_port,
	(int (*)(void *, MDLRS_Ports_folder *))MDLRS_Port_connect_folder,
	(int (*)(void *))MDLRS_Port_disconnect,
	(int (*)(void *, MDLRS_Any_port *))MDLRS_Port_disconnect_from_port,
	(int (*)(void *, MDLRS_Port *))MDLRS_Port_disconnect_port,
	(int (*)(void *, MDLRS_Ports_folder *))MDLRS_Port_disconnect_folder,
	(int (*)(void *))MDLRS_Port_disconnect_input,
	(void (*)(void *))MDLRS_Port_update,
	(void (*)(void *))MDLRS_Port_get_ready,
	(void (*)(void *))MDLRS_Note_on_update
};

void MDLRS_Note_new_body(MDLRS_Note *self)
{
	self->f=&f;

	self->events=NULL;
	self->events_size=0;
}
void MDLRS_Note_new(MDLRS_Note *self, MDLRS_Module *module)
{
	MDLRS_Port_new((MDLRS_Port *)self, "Note", module);
	self->f=&f;

	self->events=NULL;
	self->events_size=0;
}
void MDLRS_Note_on_update(MDLRS_Note *self)
{
	if (self->events_size<self->connections_count)
	{
		if (self->events)
		{
			self->events=realloc(self->events, sizeof(MDLRS_Note_events)*self->connections_count);
			memset(self->events+self->events_size, 0, sizeof(MDLRS_Note_events)*(self->connections_count-self->events_size));
		}
		else
		{
			self->events=malloc(sizeof(MDLRS_Note_events)*self->connections_count);
			memset(self->events, 0, sizeof(MDLRS_Note_events)*self->connections_count);
		}
		self->events_size=self->connections_count;
	}
	for (uint32_t a=0; a!=self->connections_count; a++)
	{
		MDLRS_Note_events *events=self->events+a;
		MDLRS_Note_events *connection_events=((MDLRS_Note *)self->connections[a].port)->events;
		if (connection_events)
		{
			if (connection_events->events_count)
			{
				if (events->events_size<connection_events->events_count)
				{
					if (events->events) events->events=realloc(events->events, sizeof(MDLRS_Note_event)*connection_events->events_count);
					else events->events=malloc(sizeof(MDLRS_Note_event)*connection_events->events_count);
					events->events_size=connection_events->events_count;
				}
				for (uint32_t a=0; a!=connection_events->events_count; a++) events->events[a]=connection_events->events[a];
				events->events_count=connection_events->events_count;
			}
			else events->events_count=0;
		}
		else events->events_count=0;
	}
}
void MDLRS_Note_add(MDLRS_Note *self, MDLRS_Note_event event)
{
	if (self->events)
	{
		if (self->events->events_count==self->events->events_size) self->events->events=realloc(self->events->events, sizeof(MDLRS_Note_event)*(self->events->events_size+=8));
		self->events->events[self->events->events_count++]=event;
	}
	else
	{
		self->events=malloc(sizeof(MDLRS_Note_events));
		memset(self->events, 0, sizeof(MDLRS_Note_events));
		self->events_size=1;
		self->events->events=malloc(sizeof(MDLRS_Note_event)*8);
		*self->events->events=event;
		self->events->events_size=8;
		self->events->events_count=1;
	}
}