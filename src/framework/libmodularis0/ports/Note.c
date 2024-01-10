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
#include <stdbool.h>
#include <stdint.h>
#include <Modularis_Core/ports/system/Connection.h>
#include <stdlib.h>
#include <Modularis_Core/ports/system/Note_event.h>
#include <Modularis_Core/ports/system/Note_type.h>

static struct MDLRS_Port_f f=
{
	(int (*)(void *, MDLRS_Port_base *))MDLRS_Port_connect,
	(int (*)(void *, MDLRS_Port *))MDLRS_Port_connect_port,
	(int (*)(void *, MDLRS_Ports_folder *))MDLRS_Port_connect_folder,
	(int (*)(void *))MDLRS_Port_disconnect,
	(int (*)(void *, MDLRS_Port_base *))MDLRS_Port_disconnect_from_port,
	(int (*)(void *, MDLRS_Port *))MDLRS_Port_disconnect_port,
	(int (*)(void *, MDLRS_Ports_folder *))MDLRS_Port_disconnect_folder,
	(int (*)(void *))MDLRS_Port_disconnect_input,
	(void (*)(void *))MDLRS_Port_update,
	(void (*)(void *))MDLRS_Port_get_ready,
	(void (*)(void *))MDLRS_Note_process
};

void MDLRS_Note_new_body(MDLRS_Note *self)
{
	self->f=&f;

	self->events=NULL;
	self->events_size=0;
	self->events_count=0;
	self->max_scancode=0;
	self->has_connection=false;
}
void MDLRS_Note_new(MDLRS_Note *self, MDLRS_Module *module)
{
	MDLRS_Port_new((MDLRS_Port *)self, "Note", module);
	self->f=&f;

	self->events=NULL;
	self->events_size=0;
	self->events_count=0;
	self->max_scancode=0;
	self->has_connection=false;
}
void MDLRS_Note_process(MDLRS_Note *self)
{
	if (self->connections_count)
	{
		if (!self->has_connection)
		{
			self->max_scancode=0;
			self->has_connection=true;
		}
		uint32_t count=((MDLRS_Note *)self->connections->port)->events_count;
		if (count)
		{
			if (self->events)
			{
				if (self->events_size<count)
				{
					self->events=realloc(self->events, sizeof(MDLRS_Note_event)*count);
					self->events_size=count;
				}
			}
			else
			{
				self->events=malloc(sizeof(MDLRS_Note_event)*count);
				self->events_size=count;
			}
			MDLRS_Note_event *events=((MDLRS_Note *)self->connections->port)->events;
			for (uint32_t a=0; a!=count; a++)
			{
				uint32_t scancode=events[a].scancode+1;
				self->max_scancode=(self->max_scancode>scancode)*self->max_scancode+(self->max_scancode<=scancode)*scancode;
				self->events[a]=events[a];
			}
			self->events_count=count;
		}
		else self->events_count=0;
	}
	else if (self->has_connection)
	{
		if (self->events_size<self->max_scancode)
		{
			self->events=realloc(self->events, sizeof(MDLRS_Note_event)*self->max_scancode);
			self->events_size=self->max_scancode;
		}
		for (uint32_t a=0; a!=self->max_scancode; a++) self->events[a]=(MDLRS_Note_event){NOTE_STOP, a};
		self->events_count=self->max_scancode;
		self->has_connection=false;
	}
	else self->events_count=0;
}