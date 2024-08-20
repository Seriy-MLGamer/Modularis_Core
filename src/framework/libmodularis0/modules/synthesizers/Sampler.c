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

#include <Modularis_Core/modules/synthesizers/Sampler.h>

#include <stdio.h>
#include <stdint.h>
#include <Modularis_Core/system/types/Sound_value.h>
#include <stdlib.h>
#include <stddef.h>
#include <Modularis_Core/ports/Sound.h>
#include <stdbool.h>
#include <Modularis_Core/system/ports/Connection.h>
#include <Modularis_Core/system/modules/synthesizers/Sampler/Pressed_samples.h>
#include <string.h>
#include <Modularis_Core/typedefs/system/ports/Port.h>
#include <Modularis_Core/system/ports/Note/Note_events.h>
#include <Modularis_Core/system/ports/Note/Note_event.h>
#include <Modularis_Core/system/ports/Note/Note_type.h>
#include <Modularis_Core/system/modules/synthesizers/Sampler/Sample.h>
#include <Modularis_Core/Modularis.h>

static struct MDLRS_Module_f f=
{
	(void (*)(void *))MDLRS_Sampler_on_update
};

void MDLRS_Sampler_new_body(MDLRS_Sampler *self, const char *file)
{
	self->f=&f;

	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->input);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->volume);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->envelope);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->loop);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->loop_start);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->loop_length);
	FILE *sample_file=fopen(file, "rb");
	uint16_t format;
	uint16_t channels;
	uint32_t sample_rate;
	uint16_t bytes;
	uint16_t bits;
	uint32_t length;
	fseek(sample_file, 20, SEEK_CUR);
	fread(&format, 2, 1, sample_file);
	fread(&channels, 2, 1, sample_file);
	fread(&sample_rate, 4, 1, sample_file);
	fseek(sample_file, 4, SEEK_CUR);
	fread(&bytes, 2, 1, sample_file);
	fread(&bits, 2, 1, sample_file);
	fseek(sample_file, 4, SEEK_CUR);
	fread(&length, 4, 1, sample_file);
	length=channels*length/bytes;
	MDLRS_Sound_value *sample=malloc(sizeof(MDLRS_Sound_value)*length);
	switch (format)
	{
		case 1: //PCM
		{
			switch (bits)
			{
				case 8:
					for (uint32_t a=0; a!=length; a++)
					{
						uint8_t frame;
						fread(&frame, 1, 1, sample_file);
						sample[a]=(MDLRS_Sound_value)frame/(uint8_t)(1<<7)-1;
					}
					break;
				case 16:
					for (uint32_t a=0; a!=length; a++)
					{
						int16_t frame;
						fread(&frame, 2, 1, sample_file);
						sample[a]=(MDLRS_Sound_value)frame/(uint16_t)(1<<15);
					}
					break;
				case 24:
					for (uint32_t a=0; a!=length; a++)
					{
						int32_t frame=0;
						fread(&frame, 3, 1, sample_file);
						sample[a]=(MDLRS_Sound_value)frame/(uint32_t)(1<<23);
					}
					break;
				case 32:
					for (uint32_t a=0; a!=length; a++)
					{
						int32_t frame;
						fread(&frame, 4, 1, sample_file);
						sample[a]=(MDLRS_Sound_value)frame/(uint32_t)(1<<31);
					}
			}
			break;
		}
		case 3: //IEEE_FLOAT
			fread(sample, 4, length, sample_file);
	}
	fclose(sample_file);
	MDLRS_Sound *outputs=malloc(sizeof(MDLRS_Sound)*channels);
	for (uint16_t a=0; a!=channels; a++)
	{
		MDLRS_Sound_new(outputs+a, (MDLRS_Module *)self);
		MDLRS_Ports_folder_add(&self->outputs, (MDLRS_Any_port *)(outputs+a));
	}
	self->pressed=NULL;
	self->outputs_array=outputs;
	self->sample=sample;
	self->pressed_count=0;
	self->sample_rate=sample_rate;
	self->length=length/channels;
	self->channels=channels;
}
void MDLRS_Sampler_new(MDLRS_Sampler *self, MDLRS_Modularis *project, const char *file)
{
	MDLRS_Module_new((MDLRS_Module *)self, project);
	MDLRS_Note_new(&self->input, (MDLRS_Module *)self);
	MDLRS_Real_controller_new(&self->volume, (MDLRS_Module *)self, 1);
	MDLRS_ADSR_new(&self->envelope, (MDLRS_Module *)self, 0, 0, 1, 0);
	MDLRS_Integer_controller_new(&self->loop, (MDLRS_Module *)self, 0);
	MDLRS_Integer_controller_new(&self->loop_start, (MDLRS_Module *)self, 0);
	MDLRS_Integer_controller_new(&self->loop_length, (MDLRS_Module *)self, 0);
	MDLRS_Released_samples_new(&self->released);
	MDLRS_Sampler_new_body(self, file);
}
void MDLRS_Sampler_on_update(MDLRS_Sampler *self)
{
	bool reconnect=false;
	if (self->input.connections_count!=self->pressed_count) reconnect=true;
	else
	{
		uint32_t a; for (a=0; a!=self->input.connections_count; a++) if (self->input.connections[a].port!=self->pressed[a].connection) break;
		if (a!=self->input.connections_count) reconnect=true;
	}
	if (reconnect)
	{
		if (self->input.connections_count)
		{
			MDLRS_Pressed_samples *new_samples=malloc(sizeof(MDLRS_Pressed_samples)*self->input.connections_count);
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
						new_samples[a]=self->pressed[b];
						picked[b]=true;
					}
					else MDLRS_Pressed_samples_new(new_samples+a, port);
				}
				for (uint32_t a=0; a!=self->pressed_count; a++) if (!picked[a]) MDLRS_Released_samples_move(&self->released, self->pressed+a);
				free(picked);
				free(self->pressed);
			}
			else for (uint32_t a=0; a!=self->input.connections_count; a++) MDLRS_Pressed_samples_new(new_samples+a, self->input.connections[a].port);
			self->pressed=new_samples;
			self->pressed_count=self->input.connections_count;
		}
		else if (self->pressed)
		{
			for (uint32_t a=0; a!=self->pressed_count; a++) MDLRS_Released_samples_move(&self->released, self->pressed+a);
			free(self->pressed);
			self->pressed=NULL;
			self->pressed_count=0;
		}
	}
	for (uint32_t a=0; a!=self->input.connections_count; a++)
	{
		MDLRS_Note_events *events=self->input.events+a;
		MDLRS_Pressed_samples *pressed=self->pressed+a;
		for (uint32_t a=0; a!=events->events_count; a++)
		{
			MDLRS_Note_event *event=events->events+a;
			switch (event->type)
			{
				case NOTE_START:
					MDLRS_Pressed_samples_start(pressed, event->scancode, &self->released, self->sample_rate*event->pitch/(440*self->project->sample_rate), event->phase*self->length, event->velocity);
					break;
				case NOTE_CHANGE:
					MDLRS_Pressed_samples_change(pressed, event->scancode, self->sample_rate*event->pitch/(440*self->project->sample_rate), event->velocity);
					break;
				case NOTE_STOP:
					MDLRS_Pressed_samples_stop(pressed, event->scancode, &self->released);
			}
		}
	}
	for (unsigned a=0; a!=self->channels; a++) self->outputs_array[a].frame=0;
	for (uint32_t a=0; a!=self->input.connections_count; a++)
	{
		MDLRS_Pressed_samples *pressed=self->pressed+a;
		for (uint32_t a=0; a!=pressed->samples_count; a++)
		{
			MDLRS_Sample *sample=pressed->samples+a;
			if (sample->exists) for (unsigned a=0; a!=self->channels; a++) self->outputs_array[a].frame+=MDLRS_ADSR_pressed(&self->envelope, (float)sample->time/self->project->sample_rate)*sample->velocity*self->sample[self->channels*(uint32_t)sample->frame+a];
		}
	}
	for (uint32_t a=0; a!=self->released.samples_count; a++)
	{
		MDLRS_Sample *sample=self->released.samples+a;
		if (sample->exists) for (unsigned a=0; a!=self->channels; a++) self->outputs_array[a].frame+=MDLRS_ADSR_pressed(&self->envelope, (float)sample->time/self->project->sample_rate)*sample->velocity*self->sample[self->channels*(uint32_t)sample->frame+a];
		else for (unsigned a=0; a!=self->channels; a++) self->outputs_array[a].frame+=MDLRS_ADSR_released(&self->envelope, (float)sample->time/self->project->sample_rate)*sample->velocity*self->sample[self->channels*(uint32_t)sample->frame+a];
	}
	for (unsigned a=0; a!=self->channels; a++) self->outputs_array[a].frame*=self->volume.value;
	MDLRS_Released_samples_update(&self->released, self);
	for (uint32_t a=0; a!=self->pressed_count; a++) MDLRS_Pressed_samples_update(self->pressed+a, self);
}
void MDLRS_Sampler_remove_body(MDLRS_Sampler *self)
{
	MDLRS_Module_disconnect((MDLRS_Module *)self);
	if (self->pressed)
	{
		for (uint32_t a=0; a!=self->pressed_count; a++) MDLRS_Pressed_samples_remove(self->pressed+a);
		free(self->pressed);
	}
	for (unsigned a=0; a!=self->channels; a++) MDLRS_Port_remove((MDLRS_Port *)(self->outputs_array+a));
	free(self->outputs_array);
	free(self->sample);
}
void MDLRS_Sampler_remove(MDLRS_Sampler *self)
{
	MDLRS_Sampler_remove_body(self);
	MDLRS_Note_remove(&self->input);
	MDLRS_Port_remove((MDLRS_Port *)&self->volume);
	MDLRS_ADSR_remove(&self->envelope);
	MDLRS_Port_remove((MDLRS_Port *)&self->loop);
	MDLRS_Port_remove((MDLRS_Port *)&self->loop_start);
	MDLRS_Port_remove((MDLRS_Port *)&self->loop_length);
	MDLRS_Released_samples_remove(&self->released);
	MDLRS_Module_remove((MDLRS_Module *)self);
}