/*
(C) 2023 Серый MLGamer. All freedoms preserved.
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

namespace MDLRS
{
	struct Port;
	struct Ports_folder;

	struct Port_base
	{
		virtual int
			connect(Port_base *port)=0,
			connect_port(Port *port)=0,
			connect_folder(Ports_folder *folder)=0,
			disconnect()=0,
			disconnect(Port_base *port)=0,
			disconnect_port(Port *port)=0,
			disconnect_folder(Ports_folder *folder)=0,
			disconnect_input()=0;
		virtual void update()=0;
		virtual void get_ready()=0;
	};
}