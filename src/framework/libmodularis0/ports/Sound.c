/*
(C) 2024 Серый MLGamer. All freedoms preserved.
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

#include <Modularis_Core/ports/Sound.h>

#include <stdint.h>
#include <Modularis_Core/system/ports/Connection.h>

static struct MDLRS_Port_f f=
{
	(int (*)(void *, MDLRS_Any_port *))MDLRS_Port_connect,
	(int (*)(void *, MDLRS_Port *))MDLRS_Port_connect_port,
	(int (*)(void *, MDLRS_Ports_folder *))MDLRS_Port_connect_folder,
	(int (*)(void *))MDLRS_Port_disconnect,
	(int (*)(void *, MDLRS_Any_port *))MDLRS_Port_disconnect_from_port,
	(int (*)(void *, MDLRS_Port *))MDLRS_Port_disconnect_port,
	(int (*)(void *, MDLRS_Ports_folder *))MDLRS_Port_disconnect_folder,
	(int (*)(void *))MDLRS_Port_disconnect_input,
	(void (*)(void *))MDLRS_Port_update,
	(void (*)(void *))MDLRS_Port_get_ready,
	(void (*)(void *))MDLRS_Sound_on_update
};

void MDLRS_Sound_new_body(MDLRS_Sound *self)
{
	self->f=&f;

	self->frame=0;
}
void MDLRS_Sound_new(MDLRS_Sound *self, MDLRS_Module *module)
{
	MDLRS_Port_new((MDLRS_Port *)self, "Sound", module);
	self->f=&f;

	self->frame=0;
}
void MDLRS_Sound_on_update(MDLRS_Sound *self)
{
	self->frame=0;
	for (uint32_t a=0; a!=self->connections_count; a++) self->frame+=((MDLRS_Sound *)self->connections[a].port)->frame;
}