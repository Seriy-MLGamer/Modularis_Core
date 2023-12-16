/*
(C) 2023 Серый MLGamer. All freedoms preserved.
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

#include <Modularis_Core/modules/players/Sequencer.hpp>

#include <cstdint>
#include <Modularis_Core/Modularis.hpp>
#include <Modularis_Core/modules/system/Sequence.hpp>
#include <Modularis_Core/modules/system/Sequence_note.hpp>

namespace MDLRS
{
	Sequencer::Sequencer(Modularis &project): Module(project), BPM(this, 120), LPB(this, 8), cursor_position(this, 0), play(this, 0), output(this)
	{
		inputs.add(BPM);
		inputs.add(LPB);
		inputs.add(cursor_position);
		inputs.add(play);
		outputs.add(output);
		tracks=NULL;
		tracks_count=0;
		length=0;
		time=0;
		is_playing=false;
		is_position_changed=false;
	}
	void Sequencer::set_position(uint32_t cursor_position)
	{
		this->cursor_position.value=cursor_position;
		is_playing=false;
		is_position_changed=true;
	}
	void Sequencer::process()
	{
		output.events_count=0;
		if (cursor_position.connections_count)
		{
			is_playing=false;
			is_position_changed=true;
		}
		else if (play.value)
		{
			if (is_position_changed)
			{
				time=60*project->sample_rate*cursor_position.value/(LPB.value*BPM.value);
				for (uint32_t a=0; a!=tracks_count; a++)
				{
					Sequence &track=tracks[a];
					track.note=0;
					track.time=0;
					while (cursor_position.value>=track.time+track.sequence[track.note].duration) track.time+=track.sequence[track.note++].duration;
				}
				is_position_changed=false;
			}
			if (is_playing) for (uint32_t a=0; a!=tracks_count; a++)
			{
				Sequence &track=tracks[a];
				if (time==(uint32_t)(60*project->sample_rate*track.time/(LPB.value*BPM.value)))
				{
					Sequence_note &note=track.sequence[track.note];
					output.add({note.type, a, note.pitch, note.velocity, note.phase});
				}
			}
			else
			{
				for (uint32_t a=0; a!=tracks_count; a++)
				{
					Sequence_note &note=tracks[a].sequence[tracks[a].note];
					output.add({note.type, a, note.pitch, note.velocity, note.phase});
				}
				is_playing=true;
			}
			if (++time!=(uint32_t)(60*project->sample_rate*length/(LPB.value*BPM.value))) for (uint32_t a=0; a!=tracks_count; a++)
			{
				Sequence &track=tracks[a];
				Sequence_note &note=track.sequence[track.note];
				if (time==(uint32_t)(60*project->sample_rate*(track.time+note.duration)/(LPB.value*BPM.value)))
				{
					track.note++;
					track.time+=note.duration;
				}
				cursor_position.value=(uint32_t)(LPB.value*BPM.value*time)/(60*project->sample_rate);
			}
			else
			{
				time=0;
				for (uint32_t a=0; a!=tracks_count; a++)
				{
					tracks[a].note=0;
					tracks[a].time=0;
				}
				cursor_position.value=0;
			}
		}
		else if (is_playing)
		{
			for (uint32_t a=0; a!=tracks_count; a++) output.add({NOTE_STOP, a});
			is_playing=false;
		}
	}
}