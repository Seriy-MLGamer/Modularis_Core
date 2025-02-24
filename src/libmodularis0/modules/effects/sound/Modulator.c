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

#include <Modularis_Core/modules/effects/sound/Modulator.h>

static struct MDLRS_Module_f f=
{
	(void (*)(void *))MDLRS_Modulator_on_update
};

void MDLRS_Modulator_new_body(MDLRS_Modulator *self)
{
	self->f=&f;

	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->carrier);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->modulator);
	MDLRS_Ports_folder_add(&self->outputs, (MDLRS_Any_port *)&self->output);
}
void MDLRS_Modulator_new(MDLRS_Modulator *self, MDLRS_Modularis *project)
{
	MDLRS_Module_new((MDLRS_Module *)self, project);
	self->f=&f;

	MDLRS_Sound_new(&self->carrier, (MDLRS_Module *)self);
	MDLRS_Sound_new(&self->modulator, (MDLRS_Module *)self);
	MDLRS_Sound_new(&self->output, (MDLRS_Module *)self);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->carrier);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->modulator);
	MDLRS_Ports_folder_add(&self->outputs, (MDLRS_Any_port *)&self->output);
}
void MDLRS_Modulator_on_update(MDLRS_Modulator *self)
{
	if (self->modulator.connections_count) self->output.frame=self->modulator.frame*self->carrier.frame;
	else self->output.frame=self->carrier.frame;
}