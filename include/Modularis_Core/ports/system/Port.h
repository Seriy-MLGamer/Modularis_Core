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

#include <Modularis_Core/typedefs/ports/system/Port.h>
#include <Modularis_Core/ports/system/Port_base.h>

#include <Modularis_Core/typedefs/modules/system/Module.h>
#include <stdint.h>
#include <Modularis_Core/typedefs/ports/system/Ports_folder.h>
#include <Modularis_Core/ports/system/Connection.h>
#include <stdlib.h>

struct MDLRS_Port
{
	MDLRS_Port_base;

	const char *type;
	MDLRS_Module *module;
	MDLRS_Connection *connections;
	uint32_t connections_size;
	uint32_t connections_count;
};
typedef struct MDLRS_Port_f *MDLRS_Port_f;
struct MDLRS_Port_f
{
	struct MDLRS_Port_base_f;

	void (*process)(void *self);
};
void MDLRS_Port_new(MDLRS_Port *self, const char *type, MDLRS_Module *module);
int
	MDLRS_Port_connect(MDLRS_Port *self, MDLRS_Port_base *port),
	MDLRS_Port_connect_port(MDLRS_Port *self, MDLRS_Port *port),
	MDLRS_Port_connect_folder(MDLRS_Port *self, MDLRS_Ports_folder *folder),
	MDLRS_Port_disconnect(MDLRS_Port *self),
	MDLRS_Port_disconnect_from_port(MDLRS_Port *self, MDLRS_Port_base *port),
	MDLRS_Port_disconnect_port(MDLRS_Port *self, MDLRS_Port *port),
	MDLRS_Port_disconnect_folder(MDLRS_Port *self, MDLRS_Ports_folder *folder),
	MDLRS_Port_disconnect_input(MDLRS_Port *self);
void MDLRS_Port_update(MDLRS_Port *self);
void MDLRS_Port_get_ready(MDLRS_Port *self);
inline uint32_t MDLRS_Port_find_connection(MDLRS_Port *self, MDLRS_Port *port)
{
	for (uint32_t a=0; a!=self->connections_count; a++) if (self->connections[a].port==port) return a+1;
	return 0;
}
inline void MDLRS_Port_remove(MDLRS_Port *self)
{
	if (self->connections) free(self->connections);
}