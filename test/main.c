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

#include <stdint.h>
#include <Modularis_Core/Modularis.h>
#include <Modularis_Core/modules/players/Sequencer.h>
#include <Modularis_Core/modules/synthesizers/Oscillator.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <Modularis_Core/modules/system/Sequence.h>
#include <Modularis_Core/modules/system/Sequence_note.h>
#include <SDL2/SDL.h>
#if defined(__linux__)
	#include <text GNU.h>
#elif defined(_WIN32)
	#include <text Windows.h>
	#include <windows.h>
#endif

#define tempo (uint32_t)140
#define song_length (uint32_t)512

char *output=NULL;
uint32_t sample_rate=44100;

typedef struct Future_sound Future_sound;
struct Future_sound
{
	MDLRS_Modularis project;
	MDLRS_Sequencer bass_part, chords_part, melody_part;
	MDLRS_Oscillator bass, chords, melody;
};
void Future_sound_new(Future_sound *self, uint32_t sample_rate)
{
	MDLRS_Modularis_new(&self->project, sample_rate, 1);
	MDLRS_Sequencer_new(&self->bass_part, &self->project);
	MDLRS_Sequencer_new(&self->chords_part, &self->project);
	MDLRS_Sequencer_new(&self->melody_part, &self->project);
	MDLRS_Oscillator_new(&self->bass, &self->project);
	MDLRS_Oscillator_new(&self->chords, &self->project);
	MDLRS_Oscillator_new(&self->melody, &self->project);
}
void Future_sound_remove(Future_sound *self)
{
	MDLRS_Sequencer_remove(&self->bass_part);
	MDLRS_Sequencer_remove(&self->chords_part);
	MDLRS_Sequencer_remove(&self->melody_part);
	MDLRS_Oscillator_remove(&self->bass);
	MDLRS_Oscillator_remove(&self->chords);
	MDLRS_Oscillator_remove(&self->melody);
	MDLRS_Modularis_remove(&self->project);
}

float notes[128];

inline int32_t clamp(int32_t value, int32_t min, int32_t max)
{
	return (value<min)*min+(value>max)*max+(value>=min&&value<=max)*value;
}
float note(const char *note)
{
	return 440*powf(2, notes[*note]+atoi(note+1)-4/*+(rand()/(float)RAND_MAX*2.f-1.f)*.001f*/);
}
void audi(Future_sound *user_data, int16_t *stream, int length)
{
	length/=sizeof(int16_t);
	for (int a=0; a!=length; a++)
	{
		if (!user_data->bass_part.time)
		{
			user_data->bass.volume.value=1.f/4.f;
			user_data->bass.waveform.value=0;
			user_data->chords.volume.value=1.f/4.f;
			user_data->chords.waveform.value=1;
			user_data->melody.volume.value=3.f/32.f;
			user_data->melody.waveform.value=3;
		}
		else if (user_data->bass_part.time==60*sample_rate*128/(8*tempo))
		{
			user_data->bass.volume.value=1.f/8.f;
			user_data->bass.waveform.value=3;
			user_data->chords.volume.value=1.f/8.f;
			user_data->chords.waveform.value=2;
			user_data->melody.volume.value=1.f/8.f;
		}
		else if (user_data->bass_part.time==60*sample_rate*240/(8*tempo))
		{
			user_data->melody.volume.value=3.f/16.f;
			user_data->melody.waveform.value=2;
		}
		MDLRS_Modularis_update(&user_data->project);
		stream[a]=clamp(MDLRS_Modularis_get(&user_data->project, 0)*32768, -32768, 32767);
	}
}

enum
{
	STATE_NONE,
	STATE_OUTPUT,
	STATE_SAMPLE_RATE
};

int main(int arguments_count, char **arguments)
{
	#ifdef _WIN32
		UINT old_codepage=GetConsoleCP();
		UINT old_output_codepage=GetConsoleOutputCP();
		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);
	#endif

	int state=STATE_NONE;
	for (int argument=1; argument!=arguments_count; argument++) switch (state)
	{
		case STATE_NONE:
			if (*arguments[argument]=='-') switch (arguments[argument][1])
			{
				case '-':
					if (!strcmp(arguments[argument]+2, "help"))
					{
						printf(help);
						return 0;
					}
					else if (!strcmp(arguments[argument]+2, "output=")) output=arguments[argument]+9;
					else if (!strcmp(arguments[argument]+2, "sample-rate=")) sample_rate=atoi(arguments[argument]+14);
					else fprintf(stderr, warning_unknown_option, arguments[argument]);
					break;
				case 'o':
					if (arguments[argument][2]) output=arguments[argument]+2;
					else state=STATE_OUTPUT;
					break;
				case 'r':
					if (arguments[argument][2]) sample_rate=atoi(arguments[argument]+2);
					else state=STATE_SAMPLE_RATE;
					break;
				default:
					fprintf(stderr, warning_unknown_option, arguments[argument]);
			}
			break;
		case STATE_OUTPUT:
			output=arguments[argument];
			state=STATE_NONE;
			break;
		case STATE_SAMPLE_RATE:
			sample_rate=atoi(arguments[argument]);
			state=STATE_NONE;
	}

	notes['A']=0;
	notes['a']=1.f/12.f;
	notes['B']=1.f/6.f;
	notes['C']=-3.f/4.f;
	notes['c']=-2.f/3.f;
	notes['D']=-7.f/12.f;
	notes['d']=-1.f/2.f;
	notes['E']=-5.f/12.f;
	notes['F']=-1.f/3.f;
	notes['f']=-1.f/4.f;
	notes['G']=-1.f/6.f;
	notes['g']=-1.f/12.f;

	Future_sound song; Future_sound_new(&song, sample_rate);
	song.bass_part.BPM.value=tempo;
	song.chords_part.BPM.value=tempo;
	song.melody_part.BPM.value=tempo;
	song.bass_part.length=song_length;
	song.chords_part.length=song_length;
	song.melody_part.length=song_length;
	MDLRS_Sequence_note bass_1[]=
	{
		{NOTE_START, note("F2"), 127, 0, 48},
		{NOTE_START, note("D2"), 127, 0, 16},
		{NOTE_START, note("C2"), 127, 0, 48},
		{NOTE_START, note("G2"), 127, 0, 16},
		{NOTE_START, note("D2"), 127, 0, 48},
		{NOTE_START, note("E2"), 127, 0, 16},
		{NOTE_START, note("F2"), 127, 0, 32},
		{NOTE_START, note("G2"), 127, 0, 32},
		{NOTE_START, note("A1"), 127, 0, 4},
		{NOTE_STOP, .duration=2},
		{NOTE_START, note("A1"), 127, 0, 6},
		{NOTE_START, note("E1"), 127, 0, 4},
		{NOTE_START, note("A1"), 127, 0, 4},
		{NOTE_STOP, .duration=2},
		{NOTE_START, note("A1"), 127, 0, 6},
		{NOTE_START, note("C2"), 127, 0, 4},
		{NOTE_START, note("F1"), 127, 0, 4},
		{NOTE_STOP, .duration=2},
		{NOTE_START, note("F1"), 127, 0, 6},
		{NOTE_START, note("F2"), 127, 0, 4},
		{NOTE_START, note("G1"), 127, 0, 4},
		{NOTE_STOP, .duration=2},
		{NOTE_START, note("G2"), 127, 0, 6},
		{NOTE_START, note("E2"), 127, 0, 4},
		{NOTE_START, note("A1"), 127, 0, 4},
		{NOTE_STOP, .duration=2},
		{NOTE_START, note("A1"), 127, 0, 6},
		{NOTE_START, note("E1"), 127, 0, 4},
		{NOTE_START, note("A1"), 127, 0, 4},
		{NOTE_STOP, .duration=2},
		{NOTE_START, note("A1"), 127, 0, 6},
		{NOTE_START, note("C2"), 127, 0, 4},
		{NOTE_START, note("F1"), 127, 0, 4},
		{NOTE_STOP, .duration=2},
		{NOTE_START, note("F1"), 127, 0, 6},
		{NOTE_START, note("F2"), 127, 0, 4},
		{NOTE_START, note("D2"), 127, 0, 4},
		{NOTE_STOP, .duration=2},
		{NOTE_START, note("D2"), 127, 0, 6},
		{NOTE_START, note("D3"), 127, 0, 4},
		{NOTE_START, note("F1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G1"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C2"), 127, 0, 4},
		{NOTE_CHANGE, note("C2"), 112, 0, 4},
		{NOTE_CHANGE, note("C2"), 96, 0, 4},
		{NOTE_CHANGE, note("C2"), 80, 0, 4},
		{NOTE_CHANGE, note("C2"), 64, 0, 4},
		{NOTE_CHANGE, note("C2"), 48, 0, 4},
		{NOTE_CHANGE, note("C2"), 32, 0, 4},
		{NOTE_CHANGE, note("C2"), 16, 0, 4}
	};
	MDLRS_Sequence bass; MDLRS_Sequence_new(&bass, bass_1);
	song.bass_part.tracks=&bass;
	song.bass_part.tracks_count=1;
	MDLRS_Sequence_note chords_1[]=
	{
		{NOTE_STOP, .duration=4},
		{NOTE_START, note("C4"), 32, 0, 4},
		{NOTE_CHANGE, note("C4"), 64, 0, 4},
		{NOTE_CHANGE, note("C4"), 96, 0, 4},
		{NOTE_CHANGE, note("C4"), 127, 0, 48},
		{NOTE_START, note("C4"), 127, 0, 64},
		{NOTE_START, note("D4"), 127, 0, 64},
		{NOTE_START, note("F4"), 127, 0, 32},
		{NOTE_START, note("A4"), 127, 0, 32},
		{NOTE_START, note("C5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("E5"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("E5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D5"), 127, 0, 6},
		{NOTE_START, note("C5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("E5"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("E5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F5"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F5"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D5"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D5"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D5"), 127, 0, 8},
		{NOTE_START, note("A4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("B4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("B4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C5"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C5"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("B4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("B4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("B4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("B4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C5"), 127, 0, 4},
		{NOTE_CHANGE, note("C5"), 112, 0, 4},
		{NOTE_CHANGE, note("C5"), 96, 0, 4},
		{NOTE_CHANGE, note("C5"), 80, 0, 4},
		{NOTE_CHANGE, note("C5"), 64, 0, 4},
		{NOTE_CHANGE, note("C5"), 48, 0, 4},
		{NOTE_CHANGE, note("C5"), 32, 0, 4},
		{NOTE_CHANGE, note("C5"), 16, 0, 4}
	};
	MDLRS_Sequence_note chords_2[]=
	{
		{NOTE_STOP, .duration=4},
		{NOTE_START, note("A3"), 32, 0, 4},
		{NOTE_CHANGE, note("A3"), 64, 0, 4},
		{NOTE_CHANGE, note("A3"), 96, 0, 4},
		{NOTE_CHANGE, note("A3"), 127, 0, 48},
		{NOTE_START, note("G3"), 127, 0, 64},
		{NOTE_START, note("A3"), 127, 0, 64},
		{NOTE_START, note("C4"), 127, 0, 32},
		{NOTE_START, note("G4"), 127, 0, 32},
		{NOTE_START, note("A4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C5"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("B4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("B4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("B4"), 127, 0, 6},
		{NOTE_START, note("A4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C5"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D5"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D5"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D5"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 8},
		{NOTE_START, note("F4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G4"), 127, 0, 4},
		{NOTE_CHANGE, note("G4"), 112, 0, 4},
		{NOTE_CHANGE, note("G4"), 96, 0, 4},
		{NOTE_CHANGE, note("G4"), 80, 0, 4},
		{NOTE_CHANGE, note("G4"), 64, 0, 4},
		{NOTE_CHANGE, note("G4"), 48, 0, 4},
		{NOTE_CHANGE, note("G4"), 32, 0, 4},
		{NOTE_CHANGE, note("G4"), 16, 0, 4}
	};
	MDLRS_Sequence_note chords_3[]=
	{
		{NOTE_STOP, .duration=4},
		{NOTE_START, note("F3"), 32, 0, 4},
		{NOTE_CHANGE, note("F3"), 64, 0, 4},
		{NOTE_CHANGE, note("F3"), 96, 0, 4},
		{NOTE_CHANGE, note("F3"), 127, 0, 48},
		{NOTE_START, note("E3"), 127, 0, 64},
		{NOTE_START, note("F3"), 127, 0, 64},
		{NOTE_START, note("A3"), 127, 0, 32},
		{NOTE_START, note("D4"), 127, 0, 32},
		{NOTE_START, note("E4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("E4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("E4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("E4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("G4"), 127, 0, 6},
		{NOTE_START, note("E4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("E4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("E4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("E4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 5},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("A4"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F4"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F4"), 127, 0, 3},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("F4"), 127, 0, 8},
		{NOTE_START, note("C4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("E4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("E4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("C4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("D4"), 127, 0, 7},
		{NOTE_STOP, .duration=1},
		{NOTE_START, note("E4"), 127, 0, 4},
		{NOTE_CHANGE, note("E4"), 112, 0, 4},
		{NOTE_CHANGE, note("E4"), 96, 0, 4},
		{NOTE_CHANGE, note("E4"), 80, 0, 4},
		{NOTE_CHANGE, note("E4"), 64, 0, 4},
		{NOTE_CHANGE, note("E4"), 48, 0, 4},
		{NOTE_CHANGE, note("E4"), 32, 0, 4},
		{NOTE_CHANGE, note("E4"), 16, 0, 4}
	};
	MDLRS_Sequence chords[3];
	MDLRS_Sequence_new(chords, chords_1);
	MDLRS_Sequence_new(chords+1, chords_2);
	MDLRS_Sequence_new(chords+2, chords_3);
	song.chords_part.tracks=chords;
	song.chords_part.tracks_count=sizeof(chords)/sizeof(MDLRS_Sequence);
	MDLRS_Sequence_note melody_1[]=
	{
		{NOTE_STOP, .duration=4},
		{NOTE_START, note("F3"), 127, 0, 4},
		{NOTE_START, note("A3"), 127, 0, 4},
		{NOTE_START, note("C4"), 127, 0, 4},
		{NOTE_START, note("E4"), 127, 0, 16},
		{NOTE_START, note("F4"), 127, 0, 16},
		{NOTE_START, note("G4"), 127, 0, 16},
		{NOTE_STOP, .duration=4},
		{NOTE_START, note("G3"), 127, 0, 4},
		{NOTE_START, note("B3"), 127, 0, 4},
		{NOTE_START, note("C4"), 127, 0, 4},
		{NOTE_START, note("E4"), 127, 0, 16},
		{NOTE_START, note("D4"), 127, 0, 16},
		{NOTE_START, note("C4"), 127, 0, 8},
		{NOTE_START, note("B3"), 127, 0, 4},
		{NOTE_START, note("C4"), 127, 0, 4},
		{NOTE_STOP, .duration=4},
		{NOTE_START, note("A3"), 127, 0, 4},
		{NOTE_START, note("B3"), 127, 0, 4},
		{NOTE_START, note("C4"), 127, 0, 4},
		{NOTE_START, note("D4"), 127, 0, 8},
		{NOTE_START, note("C4"), 127, 0, 8},
		{NOTE_START, note("E4"), 127, 0, 8},
		{NOTE_START, note("D4"), 127, 0, 8},
		{NOTE_START, note("C4"), 127, 0, 4},
		{NOTE_START, note("D4"), 127, 0, 4},
		{NOTE_START, note("C4"), 127, 0, 8},
		{NOTE_STOP, .duration=4},
		{NOTE_START, note("A3"), 127, 0, 4},
		{NOTE_START, note("B3"), 127, 0, 4},
		{NOTE_START, note("C4"), 127, 0, 4},
		{NOTE_START, note("D4"), 127, 0, 8},
		{NOTE_START, note("C4"), 127, 0, 8},
		{NOTE_START, note("G4"), 127, 0, 8},
		{NOTE_START, note("A4"), 127, 0, 8},
		{NOTE_START, note("C5"), 127, 0, 4},
		{NOTE_START, note("D5"), 127, 0, 4},
		{NOTE_START, note("E5"), 127, 0, 8},
		{NOTE_START, note("G5"), 127, 0, 4},
		{NOTE_START, note("A5"), 127, 0, 4},
		{NOTE_START, note("G5"), 127, 0, 4},
		{NOTE_START, note("E5"), 127, 0, 8},
		{NOTE_START, note("C5"), 127, 0, 8},
		{NOTE_START, note("D5"), 127, 0, 8},
		{NOTE_START, note("E5"), 127, 0, 8},
		{NOTE_START, note("D5"), 127, 0, 8},
		{NOTE_START, note("G5"), 127, 0, 4},
		{NOTE_START, note("E5"), 127, 0, 8},
		{NOTE_START, note("G5"), 127, 0, 4},
		{NOTE_START, note("C5"), 127, 0, 4},
		{NOTE_START, note("G5"), 127, 0, 4},
		{NOTE_START, note("E5"), 127, 0, 8},
		{NOTE_START, note("C5"), 127, 0, 8},
		{NOTE_START, note("D5"), 127, 0, 8},
		{NOTE_START, note("E5"), 127, 0, 8},
		{NOTE_START, note("F5"), 127, 0, 8},
		{NOTE_START, note("D5"), 127, 0, 4},
		{NOTE_START, note("A5"), 127, 0, 4},
		{NOTE_START, note("E5"), 127, 0, 4},
		{NOTE_START, note("F5"), 127, 0, 4},
		{NOTE_START, note("E5"), 127, 0, 4},
		{NOTE_START, note("D5"), 127, 0, 4},
		{NOTE_START, note("G5"), 127, 0, 4},
		{NOTE_START, note("E5"), 127, 0, 4},
		{NOTE_START, note("D5"), 127, 0, 4},
		{NOTE_START, note("G5"), 127, 0, 2},
		{NOTE_START, note("E5"), 127, 0, 2},
		{NOTE_START, note("G5"), 127, 0, 2},
		{NOTE_START, note("E5"), 127, 0, 2},
		{NOTE_START, note("A5"), 127, 0, 4},
		{NOTE_START, note("G5"), 127, 0, 4},
		{NOTE_START, note("E5"), 127, 0, 4},
		{NOTE_START, note("G5"), 127, 0, 4},
		{NOTE_START, note("E5"), 127, 0, 4},
		{NOTE_START, note("D5"), 127, 0, 4},
		{NOTE_START, note("E5"), 127, 0, 2},
		{NOTE_START, note("D5"), 127, 0, 2},
		{NOTE_START, note("E5"), 127, 0, 2},
		{NOTE_START, note("D5"), 127, 0, 2},
		{NOTE_START, note("F5"), 127, 0, 4},
		{NOTE_START, note("C5"), 127, 0, 4},
		{NOTE_START, note("G5"), 127, 0, 4},
		{NOTE_START, note("C5"), 127, 0, 4},
		{NOTE_START, note("A5"), 127, 0, 4},
		{NOTE_START, note("C5"), 127, 0, 4},
		{NOTE_START, note("B5"), 127, 0, 4},
		{NOTE_START, note("C5"), 127, 0, 4},
		{NOTE_START, note("C6"), 127, 0, 4},
		{NOTE_CHANGE, note("C6"), 112, 0, 4},
		{NOTE_CHANGE, note("C6"), 96, 0, 4},
		{NOTE_CHANGE, note("C6"), 80, 0, 4},
		{NOTE_CHANGE, note("C6"), 64, 0, 4},
		{NOTE_CHANGE, note("C6"), 48, 0, 4},
		{NOTE_CHANGE, note("C6"), 32, 0, 4},
		{NOTE_CHANGE, note("C6"), 16, 0, 4}
	};
	MDLRS_Sequence melody; MDLRS_Sequence_new(&melody, melody_1);
	song.melody_part.tracks=&melody;
	song.melody_part.tracks_count=1;
	song.bass_part.play.value=1;
	song.chords_part.play.value=1;
	song.melody_part.play.value=1;
	((MDLRS_Port_base_f)song.bass_part.outputs.ports[0]->f)->connect(song.bass_part.outputs.ports[0], song.bass.inputs.ports[0]);
	((MDLRS_Port_base_f)song.chords_part.outputs.ports[0]->f)->connect(song.chords_part.outputs.ports[0], song.chords.inputs.ports[0]);
	((MDLRS_Port_base_f)song.melody_part.outputs.ports[0]->f)->connect(song.melody_part.outputs.ports[0], song.melody.inputs.ports[0]);
	((MDLRS_Port_base_f)song.bass.outputs.ports[0]->f)->connect(song.bass.outputs.ports[0], song.project.output.inputs.ports[0]);
	((MDLRS_Port_base_f)song.chords.outputs.ports[0]->f)->connect(song.chords.outputs.ports[0], song.project.output.inputs.ports[0]);
	((MDLRS_Port_base_f)song.melody.outputs.ports[0]->f)->connect(song.melody.outputs.ports[0], song.project.output.inputs.ports[0]);

	if (output)
	{
		uint32_t song_size=60*sample_rate*song_length/(8*tempo)*2;
		uint32_t clipboard;
		FILE *file=fopen(output, "wb");
		fputs("RIFF", file); //chunkId
		clipboard=36+song_size; fwrite(&clipboard, 4, 1, file); //chunkSize
		fputs("WAVE", file); //format
		fputs("fmt ", file); //subchunk1Id
		clipboard=16; fwrite(&clipboard, 4, 1, file); //subchunk1Size
		clipboard=1; fwrite(&clipboard, 2, 1, file); //audioFormat
		clipboard=1; fwrite(&clipboard, 2, 1, file); //numChannels
		fwrite(&sample_rate, 4, 1, file); //sampleRate
		clipboard=sample_rate*2; fwrite(&clipboard, 4, 1, file); //byteRate
		clipboard=2; fwrite(&clipboard, 2, 1, file); //blockAlign
		clipboard=16; fwrite(&clipboard, 2, 1, file); //bitsPerSample
		fputs("data", file); //subchunk2Id
		fwrite(&song_size, 4, 1, file); //subchunk2Size
		//data
		int16_t buffer[1024];
		uint32_t a=0;
		while (true) if (a+sizeof buffer<song_size)
		{
			audi(&song, buffer, sizeof buffer);
			fwrite(buffer, 1, sizeof buffer, file);
			a+=sizeof buffer;
		}
		else
		{
			audi(&song, buffer, song_size-a);
			fwrite(buffer, 1, song_size-a, file);
			break;
		}
		fclose(file);
	}
	else
	{
		printf(player_text);
		#ifdef _WIN32
			SetConsoleCP(old_codepage);
			SetConsoleOutputCP(old_output_codepage);
		#endif
		SDL_AudioSpec settings={(int)sample_rate, AUDIO_S16, 1, 0, 1024, 0, 0, (SDL_AudioCallback)audi, &song};
		SDL_OpenAudio(&settings, NULL);
		SDL_PauseAudio(0);
		char what[256];
		scanf("%s", what);
		SDL_CloseAudio();
	}
	Future_sound_remove(&song);
	return 0;
}