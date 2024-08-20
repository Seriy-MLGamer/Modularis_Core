/*
(C) 2023-2024 Серый MLGamer. All freedoms preserved.
Дзен: <https://dzen.ru/seriy_mlgamer>
SoundCloud: <https://soundcloud.com/seriy_mlgamer>
YouTube: <https://www.youtube.com/@Seriy_MLGamer>
GitHub: <https://github.com/Seriy-MLGamer>
E-mail: <Seriy-MLGamer@yandex.ru>

This file is part of Modularis Core C++.
Modularis Core C++ is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Modularis Core C++ is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with Modularis Core C++. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <Modularis_Core_C++/system/ports/Any_port.hpp>

#include <cstdint>
#include <cstdlib>

namespace MDLRS
{
	struct Port;

	struct Ports_folder: Any_port
	{
		Any_port **ports;
		uint32_t ports_size;
		uint32_t ports_count;

		Ports_folder();
		int
			connect(Any_port *port),
			connect_port(Port *port),
			connect_folder(Ports_folder *folder),
			disconnect(),
			disconnect(Any_port *port),
			disconnect_port(Port *port),
			disconnect_folder(Ports_folder *folder),
			disconnect_input();
		void update();
		void get_ready();
		inline void operator+=(Any_port &port);
		inline ~Ports_folder();
	};
	void Ports_folder::operator+=(Any_port &port)
	{
		if (ports)
		{
			if (ports_count==ports_size) ports=(Any_port **)realloc(ports, sizeof(Any_port *)*(ports_size+=8));
			ports[ports_count++]=&port;
		}
		else
		{
			ports=(Any_port **)malloc(sizeof(Any_port *)*8);
			*ports=&port;
			ports_size=8;
			ports_count=1;
		}
	}
	Ports_folder::~Ports_folder()
	{
		if (ports) free(ports);
	}
}