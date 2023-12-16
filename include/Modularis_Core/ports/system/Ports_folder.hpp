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
#include <cstdlib>

namespace MDLRS
{
	struct Port;

	struct Ports_folder: Port_base
	{
		Port_base **ports;
		uint32_t ports_size;
		uint32_t ports_count;

		inline Ports_folder();
		int
			connect(Port_base *port),
			connect_port(Port *port),
			connect_folder(Ports_folder *folder),
			disconnect(),
			disconnect(Port_base *port),
			disconnect_port(Port *port),
			disconnect_folder(Ports_folder *folder),
			disconnect_input();
		inline void add(Port_base &port);
		void update();
		void get_ready();
		inline ~Ports_folder();
	};
	Ports_folder::Ports_folder()
	{
		ports=NULL;
		ports_size=0;
		ports_count=0;
	}
	void Ports_folder::add(Port_base &port)
	{
		if (ports)
		{
			if (ports_count==ports_size) ports=(Port_base **)realloc(ports, sizeof(Port_base *)*(ports_size+=8));
			ports[ports_count++]=&port;
		}
		else
		{
			ports=(Port_base **)malloc(sizeof(Port_base *)*8);
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