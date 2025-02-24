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

#include <Modularis_Core/user/modules/players/Sequencer/Pattern.h>

#include <stdint.h>
#include <Modularis_Core/user/modules/players/Sequencer/Sequence.h>
#include <Modularis_Core/system/modules/players/Sequencer/Pattern_data.h>
#include <Modularis_Core/user/modules/players/Sequencer/Note_key.h>
#include <Modularis_Core/user/modules/players/Sequencer/Discrete_key.h>
#include <Modularis_Core/user/modules/players/Sequencer/Continuous_key.h>
#include <Modularis_Core/ports/Note.h>
#include <Modularis_Core/system/ports/Note/Note_event.h>
#include <Modularis_Core/system/ports/Note/Note_type.h>
#include <math.h>
#include <Modularis_Core/user/modules/players/Sequencer/Interpolation.h>

static struct MDLRS_Any_pattern_f f=
{
	(uint32_t (*)(void *))MDLRS_Pattern_get_tracks_count,
	(void (*)(void *, MDLRS_Pattern_data *))MDLRS_Pattern_null_position,
	(void (*)(void *, MDLRS_Pattern_data *, float))MDLRS_Pattern_change_position,
	(void (*)(void *, MDLRS_Note *, MDLRS_Pattern_data *, uint32_t, float))MDLRS_Pattern_start,
	(void (*)(void *, MDLRS_Note *, MDLRS_Pattern_data *, uint32_t, float))MDLRS_Pattern_play
};

void MDLRS_Pattern_new(MDLRS_Pattern *self, float length, MDLRS_Sequence *tracks, uint32_t tracks_count)
{
	self->f=&f;

	self->length=length;
	self->tracks=tracks;
	self->tracks_count=tracks_count;
}
uint32_t MDLRS_Pattern_get_tracks_count(MDLRS_Pattern *self)
{
	return self->tracks_count;
}
void MDLRS_Pattern_null_position(MDLRS_Pattern *self, MDLRS_Pattern_data *data)
{
	for (uint32_t a=0; a!=self->tracks_count; a++)
	{
		MDLRS_Sequence *track=self->tracks+a;
		MDLRS_Pattern_data *track_data=data+a;
		track_data->note=0;
		track_data->phase=0;
		track_data->velocity=0;
		track_data->note_time=track->notes->duration;
		track_data->phase_time=track->phases->duration;
		track_data->velocity_time=track->velocities->duration;
	}
}
void MDLRS_Pattern_change_position(MDLRS_Pattern *self, MDLRS_Pattern_data *data, float time)
{
	for (uint32_t a=0; a!=self->tracks_count; a++)
	{
		MDLRS_Sequence *track=self->tracks+a;
		MDLRS_Pattern_data *track_data=data+a;
		track_data->note=0;
		track_data->phase=0;
		track_data->velocity=0;
		track_data->note_time=0;
		track_data->phase_time=0;
		track_data->velocity_time=0;
		while (time>=track_data->note_time) track_data->note_time+=track->notes[track_data->note++].duration;
		while (time>=track_data->phase_time) track_data->phase_time+=track->phases[track_data->phase++].duration;
		while (time>=track_data->velocity_time) track_data->velocity_time+=track->velocities[track_data->velocity++].duration;
		track_data->note--;
		track_data->phase--;
		track_data->velocity--;
	}
}
void MDLRS_Pattern_start(MDLRS_Pattern *self, MDLRS_Note *output, MDLRS_Pattern_data *data, uint32_t scancode, float time)
{
	for (uint32_t a=0; a!=self->tracks_count; a++)
	{
		MDLRS_Sequence *track=self->tracks+a;
		MDLRS_Pattern_data *track_data=data+a;
		while (time>=track_data->note_time) track_data->note_time+=track->notes[++track_data->note].duration;
		while (time>=track_data->phase_time) track_data->phase_time+=track->phases[++track_data->phase].duration;
		while (time>=track_data->velocity_time) track_data->velocity_time+=track->velocities[++track_data->velocity].duration;
		if (track->notes[track_data->note].pressed) MDLRS_Note_add(output, (MDLRS_Note_event){NOTE_START, scancode+a, 440*powf(2, MDLRS_Note_key_get_value(track->notes+track_data->note, time+track->notes[track_data->note].duration-track_data->note_time)/12), track->phases[track_data->phase].value, MDLRS_Continuous_key_get_value(track->velocities+track_data->velocity, time+track->velocities[track_data->velocity].duration-track_data->velocity_time)});
	}
}
void MDLRS_Pattern_play(MDLRS_Pattern *self, MDLRS_Note *output, MDLRS_Pattern_data *data, uint32_t scancode, float time)
{
	for (uint32_t a=0; a!=self->tracks_count; a++)
	{
		MDLRS_Sequence *track=self->tracks+a;
		MDLRS_Pattern_data *track_data=data+a;
		while (time>=track_data->note_time||time>=track_data->phase_time||time>=track_data->velocity_time)
		{
			if (track_data->note_time<track_data->phase_time)
			{
				if (track_data->note_time<track_data->velocity_time)
				{
					if (track->notes[track_data->note].pressed)
					{
						if (track->notes[++track_data->note].pressed) MDLRS_Note_add(output, (MDLRS_Note_event){NOTE_CHANGE, scancode+a, 440*powf(2, track->notes[track_data->note].value/12), track->phases[track_data->phase].value, MDLRS_Continuous_key_get_value(track->velocities+track_data->velocity, track_data->note_time+track->velocities[track_data->velocity].duration-track_data->velocity_time)});
						else MDLRS_Note_add(output, (MDLRS_Note_event){NOTE_STOP, scancode+a});
					}
					else if (track->notes[++track_data->note].pressed) MDLRS_Note_add(output, (MDLRS_Note_event){NOTE_START, scancode+a, 440*powf(2, track->notes[track_data->note].value/12), track->phases[track_data->phase].value, MDLRS_Continuous_key_get_value(track->velocities+track_data->velocity, track_data->note_time+track->velocities[track_data->velocity].duration-track_data->velocity_time)});
					track_data->note_time+=track->notes[track_data->note].duration;
				}
				else if (track_data->velocity_time<track_data->note_time)
				{
					if (track->notes[track_data->note].pressed) MDLRS_Note_add(output, (MDLRS_Note_event){NOTE_CHANGE, scancode+a, 440*powf(2, MDLRS_Note_key_get_value(track->notes+track_data->note, track_data->velocity_time+track->notes[track_data->note].duration-track_data->note_time)/12), track->phases[track_data->phase].value, track->velocities[track_data->velocity+1].value});
					track_data->velocity_time+=track->velocities[++track_data->velocity].duration;
				}
				else
				{
					if (track->notes[track_data->note].pressed)
					{
						if (!track->notes[++track_data->note].pressed) MDLRS_Note_add(output, (MDLRS_Note_event){NOTE_STOP, scancode+a});
					}
					else if (track->notes[++track_data->note].pressed) MDLRS_Note_add(output, (MDLRS_Note_event){NOTE_START, scancode+a, 440*powf(2, track->notes[track_data->note].value/12), track->phases[track_data->phase].value, track->velocities[track_data->velocity+1].value});
					track_data->note_time+=track->notes[track_data->note].duration;
					track_data->velocity_time+=track->velocities[++track_data->velocity].duration;
				}
			}
			else if (track_data->phase_time<track_data->note_time)
			{
				if (track_data->phase_time<track_data->velocity_time)
				{
					if (track->notes[track_data->note].pressed) MDLRS_Note_add(output, (MDLRS_Note_event){NOTE_START, scancode+a, 440*powf(2, MDLRS_Note_key_get_value(track->notes+track_data->note, track_data->phase_time+track->notes[track_data->note].duration-track_data->note_time)/12), track->phases[track_data->phase+1].value, MDLRS_Continuous_key_get_value(track->velocities+track_data->velocity, track_data->phase_time+track->velocities[track_data->velocity].duration-track_data->velocity_time)});
					track_data->phase_time+=track->phases[++track_data->phase].duration;
				}
				else if (track_data->velocity_time<track_data->phase_time) track_data->velocity_time+=track->velocities[++track_data->velocity].duration;
				else
				{
					if (track->notes[track_data->note].pressed) MDLRS_Note_add(output, (MDLRS_Note_event){NOTE_START, scancode+a, 440*powf(2, MDLRS_Note_key_get_value(track->notes+track_data->note, track_data->phase_time+track->notes[track_data->note].duration-track_data->note_time)/12), track->phases[track_data->phase+1].value, track->velocities[track_data->velocity+1].value});
					track_data->phase_time+=track->phases[++track_data->phase].duration;
					track_data->velocity_time+=track->velocities[++track_data->velocity].duration;
				}
			}
			else if (track_data->note_time<track_data->velocity_time)
			{
				if (track->notes[++track_data->note].pressed) MDLRS_Note_add(output, (MDLRS_Note_event){NOTE_START, scancode+a, 440*powf(2, track->notes[track_data->note].value/12), track->phases[track_data->phase+1].value, MDLRS_Continuous_key_get_value(track->velocities+track_data->velocity, track_data->note_time+track->velocities[track_data->velocity].duration-track_data->velocity_time)});
				else if (track->notes[track_data->note-1].pressed) MDLRS_Note_add(output, (MDLRS_Note_event){NOTE_STOP, scancode+a});
				track_data->note_time+=track->notes[track_data->note].duration;
				track_data->phase_time+=track->phases[++track_data->phase].duration;
			}
			else if (track_data->velocity_time<track_data->note_time) track_data->velocity_time+=track->velocities[++track_data->velocity].duration;
			else
			{
				if (track->notes[++track_data->note].pressed) MDLRS_Note_add(output, (MDLRS_Note_event){NOTE_START, scancode+a, 440*powf(2, track->notes[track_data->note].value/12), track->phases[track_data->phase+1].value, track->velocities[track_data->velocity+1].value});
				else if (track->notes[track_data->note-1].pressed) MDLRS_Note_add(output, (MDLRS_Note_event){NOTE_STOP, scancode+a});
				track_data->note_time+=track->notes[track_data->note].duration;
				track_data->phase_time+=track->phases[++track_data->phase].duration;
				track_data->velocity_time+=track->velocities[++track_data->velocity].duration;
			}
		}
		if (track->notes[track_data->note].pressed) if (track->notes[track_data->note].curve!=INTERPOLATION_NONE||track->velocities[track_data->velocity].curve!=INTERPOLATION_NONE)
		{
			MDLRS_Note_add(output, (MDLRS_Note_event){NOTE_CHANGE, scancode+a, 440*powf(2, MDLRS_Note_key_get_value(track->notes+track_data->note, time+track->notes[track_data->note].duration-track_data->note_time)/12), .velocity=MDLRS_Continuous_key_get_value(track->velocities+track_data->velocity, time+track->velocities[track_data->velocity].duration-track_data->velocity_time)});
		}
	}
}