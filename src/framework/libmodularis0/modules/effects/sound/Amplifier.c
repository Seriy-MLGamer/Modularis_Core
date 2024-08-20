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

#include <Modularis_Core/modules/effects/sound/Amplifier.h>

static struct MDLRS_Module_f f=
{
	(void (*)(void *))MDLRS_Amplifier_on_update
};

void MDLRS_Amplifier_new_body(MDLRS_Amplifier *self)
{
	self->f=&f;

	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->input);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->volume);
	MDLRS_Ports_folder_add(&self->outputs, (MDLRS_Any_port *)&self->output);
}
void MDLRS_Amplifier_new(MDLRS_Amplifier *self, MDLRS_Modularis *project, float volume)
{
	MDLRS_Module_new((MDLRS_Module *)self, project);
	self->f=&f;

	MDLRS_Sound_new(&self->input, (MDLRS_Module *)self);
	MDLRS_Real_controller_new(&self->volume, (MDLRS_Module *)self, volume);
	MDLRS_Sound_new(&self->output, (MDLRS_Module *)self);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->input);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->volume);
	MDLRS_Ports_folder_add(&self->outputs, (MDLRS_Any_port *)&self->output);
}
void MDLRS_Amplifier_on_update(MDLRS_Amplifier *self)
{
	self->output.frame=self->volume.value*self->input.frame;
}