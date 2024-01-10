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
#include <Modularis_Core/Modularis.h>
#include <Modularis_Core/modules/system/Sequence.h>
#include <Modularis_Core/modules/system/Sequence_note.h>

static struct MDLRS_Module_f f=
{
	(void (*)(void *))MDLRS_Sequencer_process
};

void MDLRS_Sequencer_new_body(MDLRS_Sequencer *self)
{
	self->f=&f;

	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Port_base *)&self->BPM);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Port_base *)&self->LPB);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Port_base *)&self->cursor_position);
	MDLRS_Ports_folder_add(&self->inputs, (MDLRS_Port_base *)&self->play);
	MDLRS_Ports_folder_add(&self->outputs, (MDLRS_Port_base *)&self->output);
	self->tracks=NULL;
	self->tracks_count=0;
	self->length=0;
	self->time=0;
	self->is_playing=false;
	self->is_position_changed=false;
}
void MDLRS_Sequencer_new(MDLRS_Sequencer *self, MDLRS_Modularis *project)
{
	MDLRS_Module_new((MDLRS_Module *)self, project);
	MDLRS_Real_controller_new(&self->BPM, (MDLRS_Module *)self, 120);
	MDLRS_Integer_controller_new(&self->LPB, (MDLRS_Module *)self, 8);
	MDLRS_Integer_controller_new(&self->cursor_position, (MDLRS_Module *)self, 0);
	MDLRS_Integer_controller_new(&self->play, (MDLRS_Module *)self, 0);
	MDLRS_Note_new(&self->output, (MDLRS_Module *)self);
	MDLRS_Sequencer_new_body(self);
}
void MDLRS_Sequencer_process(MDLRS_Sequencer *self)
{
	self->output.events_count=0;
	if (self->cursor_position.connections_count)
	{
		self->is_playing=false;
		self->is_position_changed=true;
	}
	else if (self->play.value)
	{
		if (self->is_position_changed)
		{
			self->time=60*self->project->sample_rate*self->cursor_position.value/(self->LPB.value*self->BPM.value);
			for (uint32_t a=0; a!=self->tracks_count; a++)
			{
				MDLRS_Sequence *track=self->tracks+a;
				track->note=0;
				track->time=0;
				while (self->cursor_position.value>=track->time+track->sequence[track->note].duration) track->time+=track->sequence[track->note++].duration;
			}
			self->is_position_changed=false;
		}
		if (self->is_playing) for (uint32_t a=0; a!=self->tracks_count; a++)
		{
			MDLRS_Sequence *track=self->tracks+a;
			if (self->time==(uint32_t)(60*self->project->sample_rate*track->time/(self->LPB.value*self->BPM.value)))
			{
				MDLRS_Sequence_note *note=track->sequence+track->note;
				MDLRS_Note_add(&self->output, (MDLRS_Note_event){note->type, a, note->pitch, note->velocity, note->phase});
			}
		}
		else
		{
			for (uint32_t a=0; a!=self->tracks_count; a++)
			{
				MDLRS_Sequence_note *note=self->tracks[a].sequence+self->tracks[a].note;
				MDLRS_Note_add(&self->output, (MDLRS_Note_event){note->type, a, note->pitch, note->velocity, note->phase});
			}
			self->is_playing=true;
		}
		if (++self->time!=(uint32_t)(60*self->project->sample_rate*self->length/(self->LPB.value*self->BPM.value))) for (uint32_t a=0; a!=self->tracks_count; a++)
		{
			MDLRS_Sequence *track=self->tracks+a;
			MDLRS_Sequence_note *note=track->sequence+track->note;
			if (self->time==(uint32_t)(60*self->project->sample_rate*(track->time+note->duration)/(self->LPB.value*self->BPM.value)))
			{
				track->note++;
				track->time+=note->duration;
			}
			self->cursor_position.value=(uint32_t)(self->LPB.value*self->BPM.value*self->time)/(60*self->project->sample_rate);
		}
		else
		{
			self->time=0;
			for (uint32_t a=0; a!=self->tracks_count; a++)
			{
				self->tracks[a].note=0;
				self->tracks[a].time=0;
			}
			self->cursor_position.value=0;
		}
	}
	else if (self->is_playing)
	{
		for (uint32_t a=0; a!=self->tracks_count; a++) MDLRS_Note_add(&self->output, (MDLRS_Note_event){NOTE_STOP, a});
		self->is_playing=false;
	}
}