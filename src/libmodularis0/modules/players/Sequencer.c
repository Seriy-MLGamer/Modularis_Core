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

#include <Modularis_Core/modules/players/Sequencer.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <Modularis_Core/system/modules/players/Sequencer/Patterns_sequence_data.h>
#include <Modularis_Core/system/modules/players/Sequencer/Any_pattern.h>
#include <Modularis_Core/system/modules/players/Sequencer/Pattern_data.h>
#include <Modularis_Core/Modularis.h>
#include <Modularis_Core/system/ports/Note/Note_event.h>
#include <Modularis_Core/system/ports/Note/Note_type.h>

static struct MDLRS_Module_f f=
{
	(void (*)(void *))MDLRS_Sequencer_on_update
};

void MDLRS_Sequencer_new_body(MDLRS_Sequencer *self)
{
	self->f=&f;

	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->BPM);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->LPB);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->cursor_position);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->loop);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Any_port *)&self->play);
	MDLRS_Ports_folder_add(&self->outputs, (MDLRS_Any_port *)&self->output);
	self->tracks=NULL;
	self->tracks_data=NULL;
	self->patterns_data=NULL;
	self->tracks_count=0;
	self->sequences_count=0;
	self->time=0;
	self->is_playing=false;
	self->is_position_changed=false;
}
void MDLRS_Sequencer_new(MDLRS_Sequencer *self, MDLRS_Modularis *project)
{
	MDLRS_Module_new((MDLRS_Module *)self, project);
	MDLRS_Real_controller_new(&self->BPM, (MDLRS_Module *)self, 120);
	MDLRS_Integer_controller_new(&self->LPB, (MDLRS_Module *)self, 8);
	MDLRS_Real_controller_new(&self->cursor_position, (MDLRS_Module *)self, 0);
	MDLRS_Integer_controller_new(&self->loop, (MDLRS_Module *)self, 0);
	MDLRS_Integer_controller_new(&self->play, (MDLRS_Module *)self, 0);
	MDLRS_Note_new(&self->output, (MDLRS_Module *)self);
	MDLRS_Sequencer_new_body(self);
}
void MDLRS_Sequencer_add(MDLRS_Sequencer *self, MDLRS_Any_pattern ***tracks, uint32_t tracks_count)
{
	MDLRS_Patterns_sequence_data *tracks_data;
	if (self->tracks_data) tracks_data=realloc(self->tracks_data, sizeof(MDLRS_Patterns_sequence_data)*tracks_count);
	else tracks_data=malloc(sizeof(MDLRS_Patterns_sequence_data)*tracks_count);
	uint32_t max;
	tracks_data->scancode=0;
	for (uint32_t a=0; a!=tracks_count-1; a++)
	{
		max=0;
		for (uint32_t b=0; tracks[a][b]; b++)
		{
			uint32_t value=((MDLRS_Any_pattern_f)tracks[a][b]->f)->get_tracks_count(tracks[a][b]);
			if (value>max) max=value;
		}
		tracks_data[a+1].scancode=tracks_data[a].scancode+max;
	}
	max=0;
	for (uint32_t a=0; tracks[tracks_count-1][a]; a++)
	{
		uint32_t value=((MDLRS_Any_pattern_f)tracks[tracks_count-1][a]->f)->get_tracks_count(tracks[tracks_count-1][a]);
		if (value>max) max=value;
	}
	uint32_t sequences_count=tracks_data[tracks_count-1].scancode+max;
	MDLRS_Pattern_data *patterns_data;
	if (self->patterns_data) patterns_data=realloc(self->patterns_data, sizeof(MDLRS_Pattern_data)*sequences_count);
	else patterns_data=malloc(sizeof(MDLRS_Pattern_data)*sequences_count);
	self->tracks=tracks;
	self->tracks_data=tracks_data;
	self->patterns_data=patterns_data;
	self->tracks_count=tracks_count;
	self->sequences_count=sequences_count;
	self->is_position_changed=true;
}
void MDLRS_Sequencer_on_update(MDLRS_Sequencer *self)
{
	if (self->output.events) self->output.events->events_count=0;
	if (self->cursor_position.connections_count) self->is_position_changed=true;
	else if (self->play.value)
	{
		if (self->is_position_changed)
		{
			if (self->is_playing)
			{
				for (uint32_t a=0; a!=self->sequences_count; a++) MDLRS_Note_add(&self->output, (MDLRS_Note_event){NOTE_STOP, a});
				self->is_playing=false;
			}
			self->time=60*self->project->sample_rate*self->cursor_position.value/(self->LPB.value*self->BPM.value);
			for (uint32_t a=0; a!=self->tracks_count; a++)
			{
				MDLRS_Any_pattern **track=self->tracks[a];
				MDLRS_Patterns_sequence_data *track_data=self->tracks_data+a;
				track_data->pattern=0;
				track_data->time=0;
				while (track[track_data->pattern])
				{
					if (self->cursor_position.value<track_data->time+track[track_data->pattern]->length) break;
					track_data->time+=track[track_data->pattern++]->length;
				}
				MDLRS_Any_pattern *pattern=track[track_data->pattern];
				if (!pattern)
				{
					self->cursor_position.value=0;
					self->time=0;
					if (track_data->pattern&&self->loop.value)
					{
						loop:
						for (a=0; a!=self->tracks_count; a++)
						{
							pattern=*self->tracks[a];
							self->tracks_data[a].pattern=0;
							self->tracks_data[a].time=pattern->length;
							((MDLRS_Any_pattern_f)pattern->f)->null_position(pattern, self->patterns_data+self->tracks_data[a].scancode);
						}
						break;
					}
					self->play.value=0;
					return;
				}
				((MDLRS_Any_pattern_f)pattern->f)->change_position(pattern, self->patterns_data+track_data->scancode, self->cursor_position.value-track_data->time);
				track_data->time+=pattern->length;
			}
			self->is_position_changed=false;
		}
		if (self->is_playing) for (uint32_t a=0; a!=self->tracks_count; a++)
		{
			MDLRS_Any_pattern **track=self->tracks[a];
			MDLRS_Patterns_sequence_data *track_data=self->tracks_data+a;
			MDLRS_Any_pattern *pattern;
			while (self->cursor_position.value>=track_data->time)
			{
				if (!track[track_data->pattern+1])
				{
					if (self->output.events) self->output.events->events_count=0;
					for (uint32_t a=0; a!=self->sequences_count; a++) MDLRS_Note_add(&self->output, (MDLRS_Note_event){NOTE_STOP, a});
					self->is_playing=false;
					self->cursor_position.value=0;
					self->time=0;
					if (self->loop.value) goto loop;
					self->play.value=0;
					self->is_playing=false;
					self->is_position_changed=true;
					return;
				}
				pattern=track[track_data->pattern];
				uint32_t max_scancode=track_data->scancode+((MDLRS_Any_pattern_f)pattern->f)->get_tracks_count(pattern);
				for (uint32_t a=track_data->scancode; a!=max_scancode; a++) MDLRS_Note_add(&self->output, (MDLRS_Note_event){NOTE_STOP, a});
				pattern=track[++track_data->pattern];
				((MDLRS_Any_pattern_f)pattern->f)->null_position(pattern, self->patterns_data+track_data->scancode);
				((MDLRS_Any_pattern_f)pattern->f)->start(pattern, &self->output, self->patterns_data+track_data->scancode, track_data->scancode, 0);
				track_data->time+=pattern->length;
			}
			pattern=track[track_data->pattern];
			((MDLRS_Any_pattern_f)pattern->f)->play(pattern, &self->output, self->patterns_data+track_data->scancode, track_data->scancode, self->cursor_position.value+pattern->length-track_data->time);
		}
		else
		{
			for (uint32_t a=0; a!=self->tracks_count; a++)
			{
				MDLRS_Any_pattern **track=self->tracks[a];
				MDLRS_Patterns_sequence_data *track_data=self->tracks_data+a;
				MDLRS_Any_pattern *pattern;
				while (self->cursor_position.value>=track_data->time)
				{
					if (!track[track_data->pattern+1])
					{
						if (self->output.events) self->output.events->events_count=0;
						self->cursor_position.value=0;
						self->time=0;
						if (self->loop.value) goto loop;
						self->play.value=0;
						self->is_playing=false;
						self->is_position_changed=true;
						return;
					}
					pattern=track[++track_data->pattern];
					((MDLRS_Any_pattern_f)pattern->f)->null_position(pattern, self->patterns_data+track_data->scancode);
					track_data->time+=pattern->length;
				}
				pattern=track[track_data->pattern];
				((MDLRS_Any_pattern_f)pattern->f)->start(pattern, &self->output, self->patterns_data+track_data->scancode, track_data->scancode, self->cursor_position.value+pattern->length-track_data->time);
			}
			self->is_playing=true;
		}
		self->time++;
		self->cursor_position.value=self->LPB.value*self->BPM.value*self->time/(60*self->project->sample_rate);
	}
	else if (self->is_playing)
	{
		for (uint32_t a=0; a!=self->sequences_count; a++) MDLRS_Note_add(&self->output, (MDLRS_Note_event){NOTE_STOP, a});
		self->is_playing=false;
	}
}