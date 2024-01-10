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

#include <Modularis_Core/modules/synthesizers/Oscillator.h>

#include <stddef.h>
#include <stdint.h>
#include <Modularis_Core/ports/system/Note_event.h>
#include <Modularis_Core/ports/system/Note_type.h>
#include <stdlib.h>
#include <Modularis_Core/modules/system/Oscillation.h>
#include <string.h>
#include <Modularis_Core/Modularis.h>
#include <stdbool.h>
#define _USE_MATH_DEFINES
#include <math.h>

static struct MDLRS_Module_f f=
{
	(void (*)(void *))MDLRS_Oscillator_process
};

void MDLRS_Oscillator_new_body(MDLRS_Oscillator *self)
{
	self->f=&f;

	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Port_base *)&self->input);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Port_base *)&self->volume);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Port_base *)&self->waveform);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Port_base *)&self->envelope);
	MDLRS_Ports_folder_add(&self->outputs, (MDLRS_Port_base *)&self->output);
	self->oscillations=NULL;
	self->oscillations_size=0;
	self->oscillations_count=0;
}
void MDLRS_Oscillator_new(MDLRS_Oscillator *self, MDLRS_Modularis *project)
{
	MDLRS_Module_new((MDLRS_Module *)self, project);
	MDLRS_Note_new(&self->input, (MDLRS_Module *)self);
	MDLRS_Real_controller_new(&self->volume, (MDLRS_Module *)self, 1);
	MDLRS_Integer_controller_new(&self->waveform, (MDLRS_Module *)self, 0);
	MDLRS_ADSR_new(&self->envelope, (MDLRS_Module *)self, 0, 0, 1, 0);
	MDLRS_Sound_new(&self->output, (MDLRS_Module *)self);
	MDLRS_Oscillator_new_body(self);
}
void MDLRS_Oscillator_process(MDLRS_Oscillator *self)
{
	for (uint32_t a=0; a!=self->input.events_count; a++)
	{
		MDLRS_Note_event *event=self->input.events+a;
		switch (event->type)
		{
			case NOTE_START:
				if (event->scancode>=self->oscillations_count)
				{
					if (event->scancode>=self->oscillations_size)
					{
						if (self->oscillations)
						{
							self->oscillations=realloc(self->oscillations, sizeof(MDLRS_Oscillation)*(event->scancode+1));
							memset(self->oscillations+self->oscillations_size, 0, sizeof(MDLRS_Oscillation)*(event->scancode-self->oscillations_size));
						}
						else
						{
							self->oscillations=malloc(sizeof(MDLRS_Oscillation)*(event->scancode+1));
							memset(self->oscillations, 0, sizeof(MDLRS_Oscillation)*event->scancode);
						}
						self->oscillations_count=self->oscillations_size=event->scancode+1;
					}
					else self->oscillations_count=event->scancode+1;
				}
				MDLRS_Oscillation_new(self->oscillations+event->scancode, event->pitch/self->project->sample_rate, event->velocity/127.f, event->phase);
				break;
			case NOTE_CHANGE:
				if (event->scancode<self->oscillations_count) if (self->oscillations[event->scancode].exist) MDLRS_Oscillation_change(self->oscillations+event->scancode, event->pitch/self->project->sample_rate, event->velocity/127.f);
				break;
			case NOTE_STOP:
				if (event->scancode<self->oscillations_count) if (self->oscillations[event->scancode].exist)
				{
					self->oscillations[event->scancode].exist=false;
					if (event->scancode==self->oscillations_count-1) while (--self->oscillations_count) if (self->oscillations[self->oscillations_count-1].exist) break;
				}
		}
	}
	self->output.frame=0;
	switch (self->waveform.value)
	{
		case 0:
			for (uint32_t a=0; a!=self->oscillations_count; a++) if (self->oscillations[a].exist) self->output.frame+=self->volume.value*self->oscillations[a].velocity*sinf(self->oscillations[a].phase*2*(float)M_PI);
			break;
		case 1:
			for (uint32_t a=0; a!=self->oscillations_count; a++) if (self->oscillations[a].exist)
			{
				float phase=self->oscillations[a].phase;
				self->output.frame+=4*self->volume.value*self->oscillations[a].velocity*((phase>=.25f&&phase<.75f)*(.5f-phase)+(phase<.25f)*phase+(phase>=.75f)*(phase-1));
			}
			break;
		case 2:
			for (uint32_t a=0; a!=self->oscillations_count; a++) if (self->oscillations[a].exist) self->output.frame+=self->volume.value*self->oscillations[a].velocity*(1-2*self->oscillations[a].phase);
			break;
		case 3:
			for (uint32_t a=0; a!=self->oscillations_count; a++) if (self->oscillations[a].exist) self->output.frame+=self->volume.value*self->oscillations[a].velocity*((self->oscillations[a].phase<.5f)-(self->oscillations[a].phase>=.5f));
	}
	for (uint32_t a=0; a!=self->oscillations_count; a++) if (self->oscillations[a].exist) MDLRS_Oscillation_update(self->oscillations+a);
}