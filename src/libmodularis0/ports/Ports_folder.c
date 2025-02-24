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

#include <Modularis_Core/ports/Ports_folder.h>

#include <stddef.h>
#include <Modularis_Core/system/ports/Any_port.h>
#include <stdint.h>
#include <Modularis_Core/system/ports/Port.h>

static struct MDLRS_Any_port_f f=
{
	(int (*)(void *, MDLRS_Any_port *))MDLRS_Ports_folder_connect,
	(int (*)(void *, MDLRS_Port *))MDLRS_Ports_folder_connect_port,
	(int (*)(void *, MDLRS_Ports_folder *))MDLRS_Ports_folder_connect_folder,
	(int (*)(void *))MDLRS_Ports_folder_disconnect,
	(int (*)(void *, MDLRS_Any_port *))MDLRS_Ports_folder_disconnect_from_port,
	(int (*)(void *, MDLRS_Port *))MDLRS_Ports_folder_disconnect_port,
	(int (*)(void *, MDLRS_Ports_folder *))MDLRS_Ports_folder_disconnect_folder,
	(int (*)(void *))MDLRS_Ports_folder_disconnect_input,
	(void (*)(void *))MDLRS_Ports_folder_update,
	(void (*)(void *))MDLRS_Ports_folder_get_ready
};

void MDLRS_Ports_folder_new(MDLRS_Ports_folder *self)
{
	self->f=&f;

	self->ports=NULL;
	self->ports_size=0;
	self->ports_count=0;
}
int MDLRS_Ports_folder_connect(MDLRS_Ports_folder *self, MDLRS_Any_port *port)
{
	return ((MDLRS_Any_port_f)port->f)->connect_folder(port, self);
}
int MDLRS_Ports_folder_connect_port(MDLRS_Ports_folder *self, MDLRS_Port *port)
{
	for (uint32_t a=0; a!=self->ports_count; a++) MDLRS_Port_connect(port, self->ports[a]);
	return 0;
}
int MDLRS_Ports_folder_connect_folder(MDLRS_Ports_folder *self, MDLRS_Ports_folder *folder)
{
	uint32_t minimum_count=(self->ports_count>folder->ports_count)*folder->ports_count+(self->ports_count<=folder->ports_count)*self->ports_count;
	for (uint32_t a=0; a!=minimum_count; a++)
	{
		MDLRS_Any_port *port=folder->ports[a];
		((MDLRS_Any_port_f)port->f)->connect(port, self->ports[a]);
	}
	return 0;
}
int MDLRS_Ports_folder_disconnect(MDLRS_Ports_folder *self)
{
	for (uint32_t a=0; a!=self->ports_count; a++)
	{
		MDLRS_Any_port *port=self->ports[a];
		((MDLRS_Any_port_f)port->f)->disconnect(port);
	}
	return 0;
}
int MDLRS_Ports_folder_disconnect_from_port(MDLRS_Ports_folder *self, MDLRS_Any_port *port)
{
	return ((MDLRS_Any_port_f)port->f)->disconnect_folder(port, self);
}
int MDLRS_Ports_folder_disconnect_port(MDLRS_Ports_folder *self, MDLRS_Port *port)
{
	for (uint32_t a=0; a!=self->ports_count; a++) MDLRS_Port_disconnect_from_port(port, self->ports[a]);
	return 0;
}
int MDLRS_Ports_folder_disconnect_folder(MDLRS_Ports_folder *self, MDLRS_Ports_folder *folder)
{
	uint32_t minimum_count=(self->ports_count>folder->ports_count)*folder->ports_count+(self->ports_count<=folder->ports_count)*self->ports_count;
	for (uint32_t a=0; a!=minimum_count; a++)
	{
		MDLRS_Any_port *port=folder->ports[a];
		((MDLRS_Any_port_f)port->f)->disconnect_from_port(port, self->ports[a]);
	}
	return 0;
}
int MDLRS_Ports_folder_disconnect_input(MDLRS_Ports_folder *self)
{
	for (uint32_t a=0; a!=self->ports_count; a++)
	{
		MDLRS_Any_port *port=self->ports[a];
		((MDLRS_Any_port_f)port->f)->disconnect_input(port);
	}
	return 0;
}
void MDLRS_Ports_folder_update(MDLRS_Ports_folder *self)
{
	for (uint32_t a=0; a!=self->ports_count; a++)
	{
		MDLRS_Any_port *port=self->ports[a];
		((MDLRS_Any_port_f)port->f)->update(port);
	}
}
void MDLRS_Ports_folder_get_ready(MDLRS_Ports_folder *self)
{
	for (uint32_t a=0; a!=self->ports_count; a++)
	{
		MDLRS_Any_port *port=self->ports[a];
		((MDLRS_Any_port_f)port->f)->get_ready(port);
	}
}