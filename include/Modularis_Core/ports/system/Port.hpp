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

#pragma once

#include <Modularis_Core/ports/system/Port_base.hpp>

#include <cstdint>
#include <Modularis_Core/ports/system/Connection.hpp>
#include <cstdlib>

namespace MDLRS
{
	struct Module;
	struct Ports_folder;

	struct Port: Port_base
	{
		const char *type;
		Module *module;
		Connection *connections;
		uint32_t connections_size;
		uint32_t connections_count;

		inline Port(const char *type, Module *module);
		int
			connect(Port_base *port),
			connect_port(Port *port),
			connect_folder(Ports_folder *folder),
			disconnect(),
			disconnect(Port_base *port),
			disconnect_port(Port *port),
			disconnect_folder(Ports_folder *folder),
			disconnect_input();
		inline uint32_t find_connection(Port *port);
		void update();
		virtual void process()=0;
		void get_ready();
		inline ~Port();
	};
	Port::Port(const char *type, Module *module)
	{
		this->type=type;
		this->module=module;
		connections=NULL;
		connections_size=0;
		connections_count=0;
	}
	uint32_t Port::find_connection(Port *port)
	{
		for (uint32_t a=0; a!=connections_count; a++) if (connections[a].port==port) return a+1;
		return 0;
	}
	Port::~Port()
	{
		if (connections) free(connections);
	}
}