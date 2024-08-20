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

#include <Modularis_Core/modules/effects/note/Transpose.h>

#include <Modularis_Core/system/ports/Note/Note_events.h>
#include <Modularis_Core/system/ports/Note/Note_event.h>
#include <math.h>

static struct MDLRS_Module_f f=
{
	(void (*)(void *))MDLRS_Transpose_on_update
};

void MDLRS_Transpose_new_body(MDLRS_Transpose *self)
{
	self->f=&f;

	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->input);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->transposition);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->velocity);
	MDLRS_Ports_folder_add(&self->outputs, (MDLRS_Any_port *)&self->output);
}
void MDLRS_Transpose_new(MDLRS_Transpose *self, MDLRS_Modularis *project, float transposition)
{
	MDLRS_Module_new((MDLRS_Module *)self, project);
	self->f=&f;

	MDLRS_Note_new(&self->input, (MDLRS_Module *)self);
	MDLRS_Real_controller_new(&self->transposition, (MDLRS_Module *)self, transposition);
	MDLRS_Real_controller_new(&self->velocity, (MDLRS_Module *)self, 1);
	MDLRS_Note_new(&self->output, (MDLRS_Module *)self);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->input);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->transposition);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->velocity);
	MDLRS_Ports_folder_add(&self->outputs, (MDLRS_Any_port *)&self->output);
}
void MDLRS_Transpose_on_update(MDLRS_Transpose *self)
{
	if (self->output.events) self->output.events->events_count=0;
	if (self->input.connections_count)
	{
		MDLRS_Note_events *events=self->input.events;
		for (uint32_t a=0; a!=events->events_count; a++)
		{
			MDLRS_Note_event *event=events->events+a;
			MDLRS_Note_add(&self->output, (MDLRS_Note_event){event->type, event->scancode, powf(2, self->transposition.value/12)*event->pitch, event->phase, self->velocity.value*event->velocity});
		}
	}
}