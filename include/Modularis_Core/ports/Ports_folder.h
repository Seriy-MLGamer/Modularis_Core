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

#include <Modularis_Core/typedefs/ports/Ports_folder.h>
#include <Modularis_Core/system/ports/Any_port.h>

#include <stdint.h>
#include <Modularis_Core/typedefs/system/ports/Port.h>
#include <stdlib.h>

struct MDLRS_Ports_folder
{
	MDLRS_Any_port;

	MDLRS_Any_port **ports;
	uint32_t ports_size;
	uint32_t ports_count;
};
void MDLRS_Ports_folder_new(MDLRS_Ports_folder *self);
int
	MDLRS_Ports_folder_connect(MDLRS_Ports_folder *self, MDLRS_Any_port *port),
	MDLRS_Ports_folder_connect_port(MDLRS_Ports_folder *self, MDLRS_Port *port),
	MDLRS_Ports_folder_connect_folder(MDLRS_Ports_folder *self, MDLRS_Ports_folder *folder),
	MDLRS_Ports_folder_disconnect(MDLRS_Ports_folder *self),
	MDLRS_Ports_folder_disconnect_from_port(MDLRS_Ports_folder *self, MDLRS_Any_port *port),
	MDLRS_Ports_folder_disconnect_port(MDLRS_Ports_folder *self, MDLRS_Port *port),
	MDLRS_Ports_folder_disconnect_folder(MDLRS_Ports_folder *self, MDLRS_Ports_folder *folder),
	MDLRS_Ports_folder_disconnect_input(MDLRS_Ports_folder *self);
void MDLRS_Ports_folder_update(MDLRS_Ports_folder *self);
void MDLRS_Ports_folder_get_ready(MDLRS_Ports_folder *self);
inline void MDLRS_Ports_folder_add(MDLRS_Ports_folder *self, MDLRS_Any_port *port)
{
	if (self->ports)
	{
		if (self->ports_count==self->ports_size) self->ports=realloc(self->ports, sizeof(MDLRS_Any_port *)*(self->ports_size+=8));
		self->ports[self->ports_count++]=port;
	}
	else
	{
		self->ports=malloc(sizeof(MDLRS_Any_port *)*8);
		*self->ports=port;
		self->ports_size=8;
		self->ports_count=1;
	}
}
inline void MDLRS_Ports_folder_remove(MDLRS_Ports_folder *self)
{
	if (self->ports) free(self->ports);
}