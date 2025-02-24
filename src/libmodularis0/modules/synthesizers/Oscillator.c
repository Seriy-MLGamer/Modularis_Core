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
#include <Modularis_Core/system/modules/synthesizers/Oscillator/Pressed_oscillations.h>
#include <Modularis_Core/system/modules/synthesizers/Oscillator/Oscillation.h>
#include <Modularis_Core/Modularis.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdbool.h>
#include <Modularis_Core/system/ports/Connection.h>
#include <stdlib.h>
#include <string.h>
#include <Modularis_Core/typedefs/system/ports/Port.h>
#include <Modularis_Core/system/ports/Note/Note_events.h>
#include <Modularis_Core/system/ports/Note/Note_event.h>
#include <Modularis_Core/system/ports/Note/Note_type.h>

static struct MDLRS_Module_f f=
{
	(void (*)(void *))MDLRS_Oscillator_on_update
};

void MDLRS_Oscillator_new_body(MDLRS_Oscillator *self)
{
	self->f=&f;

	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->input);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->volume);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->waveform);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->envelope);
	MDLRS_Ports_folder_add(&self->outputs, (MDLRS_Any_port *)&self->output);
	self->pressed=NULL;
	self->pressed_count=0;
}
void MDLRS_Oscillator_new(MDLRS_Oscillator *self, MDLRS_Modularis *project)
{
	MDLRS_Module_new((MDLRS_Module *)self, project);
	MDLRS_Note_new(&self->input, (MDLRS_Module *)self);
	MDLRS_Real_controller_new(&self->volume, (MDLRS_Module *)self, 1);
	MDLRS_Integer_controller_new(&self->waveform, (MDLRS_Module *)self, 0);
	MDLRS_ADSR_new(&self->envelope, (MDLRS_Module *)self, 0, 0, 1, 0);
	MDLRS_Sound_new(&self->output, (MDLRS_Module *)self);
	MDLRS_Released_oscillations_new(&self->released);
	MDLRS_Oscillator_new_body(self);
}
static void generate(MDLRS_Oscillator *self, float (*waveform)(float phase))
{
	for (uint32_t a=0; a!=self->input.connections_count; a++)
	{
		MDLRS_Pressed_oscillations *pressed=self->pressed+a;
		for (uint32_t a=0; a!=pressed->oscillations_count; a++)
		{
			MDLRS_Oscillation *oscillation=pressed->oscillations+a;
			if (oscillation->exists) self->output.frame+=MDLRS_ADSR_pressed(&self->envelope, (float)oscillation->time/self->project->sample_rate)*oscillation->velocity*waveform(oscillation->phase);
		}
	}
	for (uint32_t a=0; a!=self->released.oscillations_count; a++)
	{
		MDLRS_Oscillation *oscillation=self->released.oscillations+a;
		if (oscillation->exists) self->output.frame+=MDLRS_ADSR_pressed(&self->envelope, (float)oscillation->time/self->project->sample_rate)*oscillation->velocity*waveform(oscillation->phase);
		else self->output.frame+=MDLRS_ADSR_released(&self->envelope, (float)oscillation->time/self->project->sample_rate)*oscillation->velocity*waveform(oscillation->phase);
	}
}
static float sine(float phase)
{
	return sinf(phase*2*(float)M_PI);
}
static float triangle(float phase)
{
	return 4*((phase>=.25f&&phase<.75f)*(.5f-phase)+(phase<.25f)*phase+(phase>=.75f)*(phase-1));
}
static float saw(float phase)
{
	return 1-2*phase;
}
static float square(float phase)
{
	return (phase<.5f)-(phase>=.5f);
}
void MDLRS_Oscillator_on_update(MDLRS_Oscillator *self)
{
	bool reconnect=false;
	if (self->input.connections_count!=self->pressed_count) reconnect=true;
	else
	{
		uint32_t a;
		for (a=0; a!=self->input.connections_count; a++) if (self->input.connections[a].port!=self->pressed[a].connection) break;
		if (a!=self->input.connections_count) reconnect=true;
	}
	if (reconnect)
	{
		if (self->input.connections_count)
		{
			MDLRS_Pressed_oscillations *new_oscillations=malloc(sizeof(MDLRS_Pressed_oscillations)*self->input.connections_count);
			if (self->pressed)
			{
				bool *picked=malloc(sizeof(bool)*self->pressed_count);
				memset(picked, 0, sizeof(bool)*self->pressed_count);
				for (uint32_t a=0; a!=self->input.connections_count; a++)
				{
					MDLRS_Port *port=self->input.connections[a].port;
					uint32_t b;
					for (b=0; b!=self->pressed_count; b++) if (self->pressed[b].connection==port) break;
					if (b!=self->pressed_count)
					{
						new_oscillations[a]=self->pressed[b];
						picked[b]=true;
					}
					else MDLRS_Pressed_oscillations_new(new_oscillations+a, port);
				}
				for (uint32_t a=0; a!=self->pressed_count; a++) if (!picked[a]) MDLRS_Released_oscillations_move(&self->released, self->pressed+a);
				free(picked);
				free(self->pressed);
			}
			else for (uint32_t a=0; a!=self->input.connections_count; a++) MDLRS_Pressed_oscillations_new(new_oscillations+a, self->input.connections[a].port);
			self->pressed=new_oscillations;
			self->pressed_count=self->input.connections_count;
		}
		else if (self->pressed)
		{
			for (uint32_t a=0; a!=self->pressed_count; a++) MDLRS_Released_oscillations_move(&self->released, self->pressed+a);
			free(self->pressed);
			self->pressed=NULL;
			self->pressed_count=0;
		}
	}
	for (uint32_t a=0; a!=self->input.connections_count; a++)
	{
		MDLRS_Note_events *events=self->input.events+a;
		MDLRS_Pressed_oscillations *pressed=self->pressed+a;
		for (uint32_t a=0; a!=events->events_count; a++)
		{
			MDLRS_Note_event *event=events->events+a;
			switch (event->type)
			{
				case NOTE_START:
					MDLRS_Pressed_oscillations_start(pressed, event->scancode, &self->released, event->pitch/self->project->sample_rate, event->phase, event->velocity);
					break;
				case NOTE_CHANGE:
					MDLRS_Pressed_oscillations_change(pressed, event->scancode, event->pitch/self->project->sample_rate, event->velocity);
					break;
				case NOTE_STOP:
					MDLRS_Pressed_oscillations_stop(pressed, event->scancode, &self->released);
			}
		}
	}
	self->output.frame=0;
	switch (self->waveform.value)
	{
		case 0:
			generate(self, sine);
			break;
		case 1:
			generate(self, triangle);
			break;
		case 2:
			generate(self, saw);
			break;
		case 3:
			generate(self, square);
	}
	self->output.frame*=self->volume.value;
	MDLRS_Released_oscillations_update(&self->released, &self->envelope);
	for (uint32_t a=0; a!=self->pressed_count; a++) MDLRS_Pressed_oscillations_update(self->pressed+a);
}
void MDLRS_Oscillator_remove(MDLRS_Oscillator *self)
{
	MDLRS_Module_disconnect((MDLRS_Module *)self);
	if (self->pressed)
	{
		for (uint32_t a=0; a!=self->pressed_count; a++) MDLRS_Pressed_oscillations_remove(self->pressed+a);
		free(self->pressed);
	}

	MDLRS_Note_remove(&self->input);
	MDLRS_Port_remove((MDLRS_Port *)&self->volume);
	MDLRS_Port_remove((MDLRS_Port *)&self->waveform);
	MDLRS_ADSR_remove(&self->envelope);
	MDLRS_Port_remove((MDLRS_Port *)&self->output);
	MDLRS_Released_oscillations_remove(&self->released);
	MDLRS_Module_remove((MDLRS_Module *)self);
}