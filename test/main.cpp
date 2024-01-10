/*
(C) 2023-2024 Серый MLGamer. All freedoms preserved.
Дзен: <https://dzen.ru/seriy_mlgamer>
SoundCloud: <https://soundcloud.com/seriy_mlgamer>
YouTube: <https://www.youtube.com/@Seriy_MLGamer>
GitHub: <https://github.com/Seriy-MLGamer>
E-mail: <Seriy-MLGamer@yandex.ru>

This file is part of Modularis Core C++.
Modularis Core C++ is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Modularis Core C++ is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with Modularis Core C++. If not, see <https://www.gnu.org/licenses/>.
*/

#include <cstdint>
#include <Modularis_Core_C++/Modularis.hpp>
#include <Modularis_Core_C++/modules/players/Sequencer.hpp>
#include <Modularis_Core_C++/modules/synthesizers/Oscillator.hpp>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <Modularis_Core_C++/modules/system/Sequence.hpp>
#include <Modularis_Core_C++/modules/system/Sequence_note.hpp>
#include <fstream>
#include <SDL2/SDL.h>
#if defined(__linux__)
	#include <text GNU.hpp>
#elif defined(_WIN32)
	#include <text Windows.hpp>
	#include <windows.h>
#endif

using namespace std;
using namespace MDLRS;

#define tempo (uint32_t)140
#define song_length (uint32_t)512

char *output=NULL;
uint32_t sample_rate=44100;

struct Future_sound
{
	Modularis project;
	Sequencer bass_part, chords_part, melody_part;
	Oscillator bass, chords, melody;

	Future_sound(uint32_t sample_rate): project(sample_rate, 1), bass_part(project), chords_part(project), melody_part(project), bass(project), chords(project), melody(project){}
};

float notes[128];

inline int32_t clamp(int32_t value, int32_t min, int32_t max)
{
	return (value<min)*min+(value>max)*max+(value>=min&&value<=max)*value;
}
inline float note(const char *note)
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
		user_data->project.update();
		stream[a]=clamp(user_data->project.get(0)*32768, -32768, 32767);
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
						cout<<help;
						return 0;
					}
					else if (!strcmp(arguments[argument]+2, "output=")) output=arguments[argument]+9;
					else if (!strcmp(arguments[argument]+2, "sample-rate=")) sample_rate=atoi(arguments[argument]+14);
					else cerr<<warning_unknown_option<<arguments[argument]<<"\".\n";
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
					cerr<<warning_unknown_option<<arguments[argument]<<"\".\n";
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

	Future_sound song(sample_rate);
	song.bass_part.BPM.value=tempo;
	song.chords_part.BPM.value=tempo;
	song.melody_part.BPM.value=tempo;
	song.bass_part.length=song_length;
	song.chords_part.length=song_length;
	song.melody_part.length=song_length;
	Sequence_note bass_1[]=
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
	Sequence bass(bass_1);
	song.bass_part.tracks=&bass;
	song.bass_part.tracks_count=1;
	Sequence_note chords_1[]=
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
	Sequence_note chords_2[]=
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
	Sequence_note chords_3[]=
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
	Sequence chords[]=
	{
		Sequence(chords_1),
		Sequence(chords_2),
		Sequence(chords_3)
	};
	song.chords_part.tracks=chords;
	song.chords_part.tracks_count=sizeof(chords)/sizeof(Sequence);
	Sequence_note melody_1[]=
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
	Sequence melody(melody_1);
	song.melody_part.tracks=&melody;
	song.melody_part.tracks_count=1;
	song.bass_part.play.value=1;
	song.chords_part.play.value=1;
	song.melody_part.play.value=1;
	song.bass_part.outputs.ports[0]->connect(song.bass.inputs.ports[0]);
	song.chords_part.outputs.ports[0]->connect(song.chords.inputs.ports[0]);
	song.melody_part.outputs.ports[0]->connect(song.melody.inputs.ports[0]);
	song.bass.outputs.ports[0]->connect(song.project.output.inputs.ports[0]);
	song.chords.outputs.ports[0]->connect(song.project.output.inputs.ports[0]);
	song.melody.outputs.ports[0]->connect(song.project.output.inputs.ports[0]);

	if (output)
	{
		uint32_t song_size=60*sample_rate*song_length/(8*tempo)*2;
		uint32_t clipboard;
		ofstream file(output, ios::binary);
		file<<"RIFF"; //chunkId
		clipboard=36+song_size; file.write((char *)&clipboard, 4); //chunkSize
		file<<"WAVE"; //format
		file<<"fmt "; //subchunk1Id
		clipboard=16; file.write((char *)&clipboard, 4); //subchunk1Size
		clipboard=1; file.write((char *)&clipboard, 2); //audioFormat
		clipboard=1; file.write((char *)&clipboard, 2); //numChannels
		file.write((char *)&sample_rate, 4); //sampleRate
		clipboard=sample_rate*2; file.write((char *)&clipboard, 4); //byteRate
		clipboard=2; file.write((char *)&clipboard, 2); //blockAlign
		clipboard=16; file.write((char *)&clipboard, 2); //bitsPerSample
		file<<"data"; //subchunk2Id
		file.write((char *)&song_size, 4); //subchunk2Size
		//data
		int16_t buffer[1024];
		uint32_t a=0;
		while (true) if (a+sizeof buffer<song_size)
		{
			audi(&song, buffer, sizeof buffer);
			file.write((char *)buffer, sizeof buffer);
			a+=sizeof buffer;
		}
		else
		{
			audi(&song, buffer, song_size-a);
			file.write((char *)buffer, song_size-a);
			break;
		}
	}
	else
	{
		cout<<player_text;
		#ifdef _WIN32
			SetConsoleCP(old_codepage);
			SetConsoleOutputCP(old_output_codepage);
		#endif
		SDL_AudioSpec settings={(int)sample_rate, AUDIO_S16, 1, 0, 1024, 0, 0, (SDL_AudioCallback)audi, &song};
		SDL_OpenAudio(&settings, NULL);
		SDL_PauseAudio(0);
		string what;
		cin>>what;
		SDL_CloseAudio();
	}
	return 0;
}