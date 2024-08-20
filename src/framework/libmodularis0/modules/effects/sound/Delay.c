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

#include <Modularis_Core/modules/effects/sound/Delay.h>

#include <stdlib.h>
#include <stdint.h>
#include <Modularis_Core/Modularis.h>
#include <Modularis_Core/system/types/Sound_value.h>
#include <string.h>

static struct MDLRS_Module_f f=
{
	(void (*)(void *))MDLRS_Delay_on_update
};

void MDLRS_Delay_new_body(MDLRS_Delay *self, float delay)
{
	self->f=&f;

	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->input);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->delay);
	MDLRS_Ports_folder_add(&self->outputs, (MDLRS_Any_port *)&self->output);
	uint32_t buffer_size=delay*self->project->sample_rate;
	if (buffer_size)
	{
		self->buffer=malloc(sizeof(MDLRS_Sound_value)*buffer_size);
		memset(self->buffer, 0, sizeof(MDLRS_Sound_value)*buffer_size);
		self->buffer_size=buffer_size;
	}
	else
	{
		self->buffer=malloc(1);
		self->buffer_size=0;
	}
	self->position=0;
}
void MDLRS_Delay_new(MDLRS_Delay *self, MDLRS_Modularis *project, float delay)
{
	MDLRS_Module_new((MDLRS_Module *)self, project);
	MDLRS_Sound_new(&self->input, (MDLRS_Module *)self);
	MDLRS_Real_controller_new(&self->delay, (MDLRS_Module *)self, delay);
	MDLRS_Sound_new(&self->output, (MDLRS_Module *)self);
	MDLRS_Delay_new_body(self, delay);
}
void MDLRS_Delay_on_update(MDLRS_Delay *self)
{
	uint32_t buffer_size=self->delay.value*self->project->sample_rate;
	if (buffer_size)
	{
		if (buffer_size>self->buffer_size)
		{
			self->buffer=realloc(self->buffer, sizeof(MDLRS_Sound_value)*buffer_size);
			self->buffer_size=buffer_size;
		}
		self->output.frame=self->buffer[self->position];
		self->buffer[self->position]=self->input.frame;
		self->position=(self->position+1)%buffer_size;
	}
}