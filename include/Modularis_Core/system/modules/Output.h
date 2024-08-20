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

#include <Modularis_Core/typedefs/system/modules/Output.h>
#include <Modularis_Core/system/modules/Module.h>

#include <stdint.h>
#include <Modularis_Core/ports/Sound.h>
#include <Modularis_Core/typedefs/Modularis.h>
#include <stdlib.h>

struct MDLRS_Output
{
	MDLRS_Module;

	MDLRS_Sound *channels;
	uint32_t channels_count;
};
void MDLRS_Output_new(MDLRS_Output *self, MDLRS_Modularis *project, uint32_t channels);
inline void MDLRS_Output_remove(MDLRS_Output *self)
{
	MDLRS_Ports_folder_disconnect_input(&self->inputs);
	for (uint32_t a=0; a!=self->channels_count; a++) MDLRS_Port_remove((MDLRS_Port *)(self->channels+a));
	free(self->channels);
	
	MDLRS_Ports_folder_remove(&self->inputs);
}