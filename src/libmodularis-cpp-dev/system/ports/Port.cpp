/*
(C) 2024 Серый MLGamer. All freedoms preserved.
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

#include <Modularis_Core_C++/system/ports/Port.hpp>

#include <cstdint>
#include <Modularis_Core_C++/ports/Ports_folder.hpp>
#include <Modularis_Core_C++/system/ports/Any_port.hpp>
#include <Modularis_Core_C++/system/ports/Connection.hpp>
#include <Modularis_Core_C++/system/modules/Module.hpp>

extern "C"
{
	using namespace MDLRS;

	void MDLRS_Port_new(Port *self, const char *type, Module *module);
	int
		MDLRS_Port_connect(Port *self, Any_port *port),
		MDLRS_Port_connect_port(Port *self, Port *port),
		MDLRS_Port_disconnect(Port *self),
		MDLRS_Port_disconnect_from_port(Port *self, Any_port *port),
		MDLRS_Port_disconnect_port(Port *self, Port *port);
}

namespace MDLRS
{
	Port::Port(const char *type, Module *module)
	{
		MDLRS_Port_new(this, type, module);
	}
	int Port::connect(Any_port *port)
	{
		return MDLRS_Port_connect(this, port);
	}
	int Port::connect_port(Port *port)
	{
		return MDLRS_Port_connect_port(this, port);
	}
	int Port::connect_folder(Ports_folder *folder)
	{
		for (uint32_t a=0; a!=folder->ports_count; a++) folder->ports[a]->connect(this);
		return 0;
	}
	int Port::disconnect()
	{
		return MDLRS_Port_disconnect(this);
	}
	int Port::disconnect(Any_port *port)
	{
		return MDLRS_Port_disconnect_from_port(this, port);
	}
	int Port::disconnect_port(Port *port)
	{
		return MDLRS_Port_disconnect_port(this, port);
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
			while (connections_count) MDLRS_Port_disconnect_from_port(connections[connections_count-1].port, this);
			return 0;
		}
		return 1;
	}
	void Port::update()
	{
		for (uint32_t a=0; a!=connections_count; a++) connections[a].port->module->update();
		on_update();
	}
	void Port::get_ready()
	{
		for (uint32_t a=0; a!=connections_count; a++) connections[a].port->module->get_ready();
	}
}