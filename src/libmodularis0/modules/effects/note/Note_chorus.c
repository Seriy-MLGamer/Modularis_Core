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

#include <Modularis_Core/modules/effects/note/Note_chorus.h>

#include <Modularis_Core/system/ports/Note/Note_events.h>
#include <Modularis_Core/system/ports/Note/Note_event.h>
#include <Modularis_Core/system/ports/Note/Note_type.h>
#include <math.h>
#include <stdlib.h>

static struct MDLRS_Module_f f=
{
	(void (*)(void *))MDLRS_Note_chorus_on_update
};

void MDLRS_Note_chorus_new_body(MDLRS_Note_chorus *self)
{
	self->f=&f;

	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->input);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->spread);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->voices);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->random_phases);
	MDLRS_Ports_folder_add(&self->outputs, (MDLRS_Any_port *)&self->output);
}
void MDLRS_Note_chorus_new(MDLRS_Note_chorus *self, MDLRS_Modularis *project, float spread, uint32_t voices)
{
	MDLRS_Module_new((MDLRS_Module *)self, project);
	self->f=&f;

	MDLRS_Note_new(&self->input, (MDLRS_Module *)self);
	MDLRS_Real_controller_new(&self->spread, (MDLRS_Module *)self, spread);
	MDLRS_Integer_controller_new(&self->voices, (MDLRS_Module *)self, voices);
	MDLRS_Integer_controller_new(&self->random_phases, (MDLRS_Module *)self, 1);
	MDLRS_Note_new(&self->output, (MDLRS_Module *)self);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->input);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->spread);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->voices);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->random_phases);
	MDLRS_Ports_folder_add(&self->outputs, (MDLRS_Any_port *)&self->output);
}
void MDLRS_Note_chorus_on_update(MDLRS_Note_chorus *self)
{
	if (self->output.events) self->output.events->events_count=0;
	if (self->input.connections_count)
	{
		MDLRS_Note_events *events=self->input.events;
		if (self->random_phases.value) for (uint32_t a=0; a!=events->events_count; a++)
		{
			MDLRS_Note_event *event=events->events+a;
			switch (event->type)
			{
				case NOTE_START:
					for (int32_t a=0; a!=self->voices.value; a++) MDLRS_Note_add(&self->output, (MDLRS_Note_event){NOTE_START, self->voices.value*event->scancode+a, powf(2, self->spread.value*(2*a+1-self->voices.value)/(24*(self->voices.value-1)))*event->pitch, rand()/(float)((uint64_t)RAND_MAX+1), event->velocity});
					break;
				case NOTE_CHANGE:
					for (int32_t a=0; a!=self->voices.value; a++) MDLRS_Note_add(&self->output, (MDLRS_Note_event){NOTE_CHANGE, self->voices.value*event->scancode+a, powf(2, self->spread.value*(2*a+1-self->voices.value)/(24*(self->voices.value-1)))*event->pitch, .velocity=event->velocity});
					break;
				case NOTE_STOP:
					for (int32_t a=0; a!=self->voices.value; a++) MDLRS_Note_add(&self->output, (MDLRS_Note_event){NOTE_STOP, self->voices.value*event->scancode+a});
			}
		}
		else for (uint32_t a=0; a!=events->events_count; a++)
		{
			MDLRS_Note_event *event=events->events+a;
			switch (event->type)
			{
				case NOTE_START:
					for (int32_t a=0; a!=self->voices.value; a++) MDLRS_Note_add(&self->output, (MDLRS_Note_event){NOTE_START, self->voices.value*event->scancode+a, powf(2, self->spread.value*(2*a+1-self->voices.value)/(24*(self->voices.value-1)))*event->pitch, event->phase, event->velocity});
					break;
				case NOTE_CHANGE:
					for (int32_t a=0; a!=self->voices.value; a++) MDLRS_Note_add(&self->output, (MDLRS_Note_event){NOTE_CHANGE, self->voices.value*event->scancode+a, powf(2, self->spread.value*(2*a+1-self->voices.value)/(24*(self->voices.value-1)))*event->pitch, .velocity=event->velocity});
					break;
				case NOTE_STOP:
					for (int32_t a=0; a!=self->voices.value; a++) MDLRS_Note_add(&self->output, (MDLRS_Note_event){NOTE_STOP, self->voices.value*event->scancode+a});
			}
		}
	}
}