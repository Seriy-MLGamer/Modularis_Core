/*
(C) 2022-2024 Серый MLGamer. All freedoms preserved.
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

#include <Modularis_Core/typedefs/modules/system/Module.h>

#include <Modularis_Core/typedefs/Modularis.h>
#include <stdint.h>
#include <Modularis_Core/ports/system/Ports_folder.h>
#include <stdbool.h>

struct MDLRS_Module
{
	void *f;

	MDLRS_Modularis *project;
	uint32_t index;
	MDLRS_Ports_folder inputs, outputs;
	uint32_t output_connections;
	bool ready;
};
typedef struct MDLRS_Module_f *MDLRS_Module_f;
struct MDLRS_Module_f
{
	void (*process)(void *self);
};
void MDLRS_Module_new(MDLRS_Module *self, MDLRS_Modularis *project);
void MDLRS_Module_new_empty(MDLRS_Module *self);
inline void MDLRS_Module_update(MDLRS_Module *self)
{
	if (self->ready)
	{
		self->ready=false;
		MDLRS_Ports_folder_update(&self->inputs);
		((MDLRS_Module_f)self->f)->process(self);
	}
}
inline void MDLRS_Module_get_ready(MDLRS_Module *self)
{
	if (self->ready) return;
	self->ready=true;
	MDLRS_Ports_folder_get_ready(&self->inputs);
}
inline void MDLRS_Module_disconnect(MDLRS_Module *self)
{
	MDLRS_Ports_folder_disconnect_input(&self->inputs);
	MDLRS_Ports_folder_disconnect(&self->outputs);
}
void MDLRS_Module_remove(MDLRS_Module *self);