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

#include <Modularis_Core/modules/system/Output.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <Modularis_Core/ports/Sound.h>

void MDLRS_Output_new_body(MDLRS_Output *self, MDLRS_Modularis *project, uint32_t channels)
{
	self->project=project;
	self->index=0;
	self->ready=true;
	self->channels=malloc(sizeof(MDLRS_Sound)*channels);
	self->channels_count=channels;
	for (uint32_t a=0; a!=channels; a++)
	{
		MDLRS_Sound_new(self->channels+a, (MDLRS_Module *)self);
		MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Port_base *)self->channels+a);
	}
}
void MDLRS_Output_new(MDLRS_Output *self, MDLRS_Modularis *project, uint32_t channels)
{
	MDLRS_Module_new_empty((MDLRS_Module *)self);
	MDLRS_Output_new_body(self, project, channels);
}