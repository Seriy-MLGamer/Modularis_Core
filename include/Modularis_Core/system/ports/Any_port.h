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

#include <Modularis_Core/typedefs/system/ports/Any_port.h>

#include <Modularis_Core/typedefs/system/ports/Port.h>
#include <Modularis_Core/typedefs/ports/Ports_folder.h>

struct MDLRS_Any_port
{
	void *f;
};
typedef struct MDLRS_Any_port_f *MDLRS_Any_port_f;
struct MDLRS_Any_port_f
{
	int
		(*connect)(void *self, MDLRS_Any_port *port),
		(*connect_port)(void *self, MDLRS_Port *port),
		(*connect_folder)(void *self, MDLRS_Ports_folder *folder),
		(*disconnect)(void *self),
		(*disconnect_from_port)(void *self, MDLRS_Any_port *port),
		(*disconnect_port)(void *self, MDLRS_Port *port),
		(*disconnect_folder)(void *self, MDLRS_Ports_folder *folder),
		(*disconnect_input)(void *self);
	void (*update)(void *self);
	void (*get_ready)(void *self);
};