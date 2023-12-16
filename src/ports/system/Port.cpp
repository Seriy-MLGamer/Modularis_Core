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

#include <Modularis_Core/ports/system/Port.hpp>

#include <Modularis_Core/ports/system/Port_base.hpp>
#include <Modularis_Core/modules/system/Module.hpp>
#include <Modularis_Core/Modularis.hpp>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <Modularis_Core/ports/system/Connection.hpp>
#include <Modularis_Core/ports/system/Ports_folder.hpp>

namespace MDLRS
{
	int Port::connect(Port_base *port)
	{
		int result=port->connect_port(this);
		if (module->index&&module->output_connections)
		{
			Module **modules=module->project->disconnected_modules;
			uint32_t count=module->project->disconnected_modules_count--;
			for (uint32_t a=module->index; a!=count; a++)
			{
				modules[a]->index--;
				modules[a-1]=modules[a];
			}
			module->index=0;
		}
		return result;
	}
	int Port::connect_port(Port *port)
	{
		if (strcmp(type, port->type)) return 1;
		if (find_connection(port)) return 2;
		if (connections)
		{
			if (connections_count==connections_size) connections=(Connection *)realloc(connections, sizeof(Connection)*(connections_size+=8));
			connections[connections_count++]={port};
		}
		else
		{
			connections=(Connection *)malloc(sizeof(Connection)*8);
			*connections={port};
			connections_size=8;
			connections_count=1;
		}
		if (port->connections)
		{
			if (port->connections_count==port->connections_size) port->connections=(Connection *)realloc(port->connections, sizeof(Connection)*(port->connections_size+=8));
			port->connections[port->connections_count++]={this, connections_count};
		}
		else
		{
			port->connections=(Connection *)malloc(sizeof(Connection)*8);
			*port->connections={this, connections_count};
			port->connections_size=8;
			port->connections_count=1;
		}
		connections[connections_count-1].index=port->connections_count;
		port->module->output_connections++;
		return 0;
	}
	int Port::connect_folder(Ports_folder *folder)
	{
		for (uint32_t a=0; a!=folder->ports_count; a++) folder->ports[a]->connect(this);
		return 0;
	}
	int Port::disconnect()
	{
		if (connections_count)
		{
			uint32_t count=connections_count;
			while (connections_count)
			{
				Port &port=*connections[--connections_count].port;
				uint32_t count=port.connections_count--;
				for (uint32_t a=connections[connections_count].index; a!=count; a++)
				{
					port.connections[a].port->connections[port.connections[a].index-1].index--;
					port.connections[a-1]=port.connections[a];
				}
			}
			module->output_connections-=count;
			if (!module->output_connections)
			{
				Modularis &project=*module->project;
				if (project.disconnected_modules_count==project.disconnected_modules_size) project.disconnected_modules=(Module **)realloc(project.disconnected_modules, sizeof(Module *)*(project.disconnected_modules_size+=8));
				project.disconnected_modules[project.disconnected_modules_count++]=module;
				module->index=project.disconnected_modules_count;
			}
			return 0;
		}
		return 1;
	}
	int Port::disconnect(Port_base *port)
	{
		int result=port->disconnect_port(this);
		if (!(module->index||module->output_connections))
		{
			Modularis &project=*module->project;
			if (project.disconnected_modules_count==project.disconnected_modules_size) project.disconnected_modules=(Module **)realloc(project.disconnected_modules, sizeof(Module *)*(project.disconnected_modules_size+=8));
			project.disconnected_modules[project.disconnected_modules_count++]=module;
			module->index=project.disconnected_modules_count;
		}
		return result;
	}
	int Port::disconnect_port(Port *port)
	{
		uint32_t index=find_connection(port);
		if (index)
		{
			uint32_t count=port->connections_count--;
			for (uint32_t a=index; a!=count; a++)
			{
				port->connections[a].port->connections[port->connections[a].index-1].index--;
				port->connections[a-1]=port->connections[a];
			}
			index=connections[index-1].index;
			count=connections_count--;
			for (uint32_t a=index; a!=count; a++)
			{
				connections[a].port->connections[connections[a].index-1].index--;
				connections[a-1]=connections[a];
			}
			port->module->output_connections--;
			return 0;
		}
		return 1;
	}
	int Port::disconnect_folder(Ports_folder *folder)
	{
		for (uint32_t a=0; a!=folder->ports_count; a++) folder->ports[a]->disconnect(this);
		return 0;
	}
	int Port::disconnect_input()
	{
		if (connections_count)
		{
			while (connections_count) connections[connections_count-1].port->disconnect(this);
			return 0;
		}
		return 1;
	}
	void Port::update()
	{
		for (uint32_t a=0; a!=connections_count; a++) connections[a].port->module->update();
		process();
	}
	void Port::get_ready()
	{
		for (uint32_t a=0; a!=connections_count; a++) connections[a].port->module->get_ready();
	}
}