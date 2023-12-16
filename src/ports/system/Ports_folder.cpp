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

#include <Modularis_Core/ports/system/Ports_folder.hpp>

#include <Modularis_Core/ports/system/Port_base.hpp>
#include <cstdint>
#include <Modularis_Core/ports/system/Port.hpp>

namespace MDLRS
{
	int Ports_folder::connect(Port_base *port)
	{
		return port->connect_folder(this);
	}
	int Ports_folder::connect_port(Port *port)
	{
		for (uint32_t a=0; a!=ports_count; a++) port->connect(ports[a]);
		return 0;
	}
	int Ports_folder::connect_folder(Ports_folder *folder)
	{
		uint32_t minimum_count=(ports_count>folder->ports_count)*folder->ports_count+(ports_count<=folder->ports_count)*ports_count;
		for (uint32_t a=0; a!=minimum_count; a++) folder->ports[a]->connect(ports[a]);
		return 0;
	}
	int Ports_folder::disconnect()
	{
		for (uint32_t a=0; a!=ports_count; a++) ports[a]->disconnect();
		return 0;
	}
	int Ports_folder::disconnect(Port_base *port)
	{
		return port->disconnect_folder(this);
	}
	int Ports_folder::disconnect_port(Port *port)
	{
		for (uint32_t a=0; a!=ports_count; a++) port->disconnect(ports[a]);
		return 0;
	}
	int Ports_folder::disconnect_folder(Ports_folder *folder)
	{
		uint32_t minimum_count=(ports_count>folder->ports_count)*folder->ports_count+(ports_count<=folder->ports_count)*ports_count;
		for (uint32_t a=0; a!=minimum_count; a++) folder->ports[a]->disconnect(ports[a]);
		return 0;
	}
	int Ports_folder::disconnect_input()
	{
		for (uint32_t a=0; a!=ports_count; a++) ports[a]->disconnect_input();
		return 0;
	}
	void Ports_folder::update()
	{
		for (uint32_t a=0; a!=ports_count; a++) ports[a]->update();
	}
	void Ports_folder::get_ready()
	{
		for (uint32_t a=0; a!=ports_count; a++) ports[a]->get_ready();
	}
}