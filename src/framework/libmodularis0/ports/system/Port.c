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

#include <Modularis_Core/ports/system/Port.h>

#include <stddef.h>
#include <Modularis_Core/ports/system/Port_base.h>
#include <Modularis_Core/modules/system/Module.h>
#include <Modularis_Core/Modularis.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <Modularis_Core/ports/system/Connection.h>
#include <Modularis_Core/ports/system/Ports_folder.h>

static struct MDLRS_Port_base_f f=
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
	(void (*)(void *))MDLRS_Port_get_ready
};

void MDLRS_Port_new(MDLRS_Port *self, const char *type, MDLRS_Module *module)
{
	self->f=&f;

	self->type=type;
	self->module=module;
	self->connections=NULL;
	self->connections_size=0;
	self->connections_count=0;
}
int MDLRS_Port_connect(MDLRS_Port *self, MDLRS_Port_base *port)
{
	int result=((MDLRS_Port_base_f)port->f)->connect_port(port, self);
	if (self->module->index&&self->module->output_connections)
	{
		MDLRS_Module **modules=self->module->project->disconnected_modules;
		uint32_t count=self->module->project->disconnected_modules_count--;
		for (uint32_t a=self->module->index; a!=count; a++)
		{
			modules[a]->index--;
			modules[a-1]=modules[a];
		}
		self->module->index=0;
	}
	return result;
}
int MDLRS_Port_connect_port(MDLRS_Port *self, MDLRS_Port *port)
{
	if (strcmp(self->type, port->type)) return 1;
	if (MDLRS_Port_find_connection(self, port)) return 2;
	if (self->connections)
	{
		if (self->connections_count==self->connections_size) self->connections=realloc(self->connections, sizeof(MDLRS_Connection)*(self->connections_size+=8));
		self->connections[self->connections_count++]=(MDLRS_Connection){port};
	}
	else
	{
		self->connections=malloc(sizeof(MDLRS_Connection)*8);
		*self->connections=(MDLRS_Connection){port};
		self->connections_size=8;
		self->connections_count=1;
	}
	if (port->connections)
	{
		if (port->connections_count==port->connections_size) port->connections=realloc(port->connections, sizeof(MDLRS_Connection)*(port->connections_size+=8));
		port->connections[port->connections_count++]=(MDLRS_Connection){self, self->connections_count};
	}
	else
	{
		port->connections=malloc(sizeof(MDLRS_Connection)*8);
		*port->connections=(MDLRS_Connection){self, self->connections_count};
		port->connections_size=8;
		port->connections_count=1;
	}
	self->connections[self->connections_count-1].index=port->connections_count;
	port->module->output_connections++;
	return 0;
}
int MDLRS_Port_connect_folder(MDLRS_Port *self, MDLRS_Ports_folder *folder)
{
	for (uint32_t a=0; a!=folder->ports_count; a++)
	{
		MDLRS_Port_base *port=folder->ports[a];
		((MDLRS_Port_base_f)port->f)->connect(port, (MDLRS_Port_base *)self);
	}
	return 0;
}
int MDLRS_Port_disconnect(MDLRS_Port *self)
{
	if (self->connections_count)
	{
		uint32_t count=self->connections_count;
		while (self->connections_count)
		{
			MDLRS_Port *port=self->connections[--self->connections_count].port;
			uint32_t count=port->connections_count--;
			for (uint32_t a=self->connections[self->connections_count].index; a!=count; a++)
			{
				port->connections[a].port->connections[port->connections[a].index-1].index--;
				port->connections[a-1]=port->connections[a];
			}
		}
		self->module->output_connections-=count;
		if (!self->module->output_connections)
		{
			MDLRS_Modularis *project=self->module->project;
			if (project->disconnected_modules_count==project->disconnected_modules_size) project->disconnected_modules=realloc(project->disconnected_modules, sizeof( MDLRS_Module *)*(project->disconnected_modules_size+=8));
			project->disconnected_modules[project->disconnected_modules_count++]=self->module;
			self->module->index=project->disconnected_modules_count;
		}
		return 0;
	}
	return 1;
}
int MDLRS_Port_disconnect_from_port(MDLRS_Port *self, MDLRS_Port_base *port)
{
	int result=((MDLRS_Port_base_f)port->f)->disconnect_port(port, self);
	if (!(self->module->index||self->module->output_connections))
	{
		MDLRS_Modularis *project=self->module->project;
		if (project->disconnected_modules_count==project->disconnected_modules_size) project->disconnected_modules=realloc(project->disconnected_modules, sizeof(MDLRS_Module *)*(project->disconnected_modules_size+=8));
		project->disconnected_modules[project->disconnected_modules_count++]=self->module;
		self->module->index=project->disconnected_modules_count;
	}
	return result;
}
int MDLRS_Port_disconnect_port(MDLRS_Port *self, MDLRS_Port *port)
{
	uint32_t index=MDLRS_Port_find_connection(self, port);
	if (index)
	{
		uint32_t count=port->connections_count--;
		for (uint32_t a=index; a!=count; a++)
		{
			port->connections[a].port->connections[port->connections[a].index-1].index--;
			port->connections[a-1]=port->connections[a];
		}
		index=self->connections[index-1].index;
		count=self->connections_count--;
		for (uint32_t a=index; a!=count; a++)
		{
			self->connections[a].port->connections[self->connections[a].index-1].index--;
			self->connections[a-1]=self->connections[a];
		}
		port->module->output_connections--;
		return 0;
	}
	return 1;
}
int MDLRS_Port_disconnect_folder(MDLRS_Port *self, MDLRS_Ports_folder *folder)
{
	for (uint32_t a=0; a!=folder->ports_count; a++)
	{
		MDLRS_Port_base *port=folder->ports[a];
		((MDLRS_Port_base_f)port->f)->disconnect_from_port(port, (MDLRS_Port_base *)self);
	}
	return 0;
}
int MDLRS_Port_disconnect_input(MDLRS_Port *self)
{
	if (self->connections_count)
	{
		while (self->connections_count) MDLRS_Port_disconnect_from_port(self->connections[self->connections_count-1].port, (MDLRS_Port_base *)self);
		return 0;
	}
	return 1;
}
void MDLRS_Port_update(MDLRS_Port *self)
{
	for (uint32_t a=0; a!=self->connections_count; a++) MDLRS_Module_update(self->connections[a].port->module);
	((MDLRS_Port_f)self->f)->process(self);
}
void MDLRS_Port_get_ready(MDLRS_Port *self)
{
	for (uint32_t a=0; a!=self->connections_count; a++) MDLRS_Module_get_ready(self->connections[a].port->module);
}