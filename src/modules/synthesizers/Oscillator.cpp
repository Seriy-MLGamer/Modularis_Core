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

#include <Modularis_Core/modules/synthesizers/Oscillator.hpp>

#include <cstdint>
#include <Modularis_Core/ports/system/Note_event.hpp>
#include <Modularis_Core/ports/system/Note_type.hpp>
#include <cstdlib>
#include <Modularis_Core/modules/system/Oscillation.hpp>
#include <cstring>
#include <Modularis_Core/Modularis.hpp>
#define _USE_MATH_DEFINES
#include <cmath>

namespace MDLRS
{
	Oscillator::Oscillator(Modularis &project): Module(project), input(this), volume(this, 1), waveform(this, 0), envelope(this, 0, 0, 1, 0), output(this)
	{
		inputs.add(input);
		inputs.add(volume);
		inputs.add(waveform);
		inputs.add(envelope);
		outputs.add(output);
		oscillations=NULL;
		oscillations_size=0;
		oscillations_count=0;
	}
	void Oscillator::process()
	{
		for (uint32_t a=0; a!=input.events_count; a++)
		{
			Note_event &event=input.events[a];
			switch (event.type)
			{
				case NOTE_START:
					if (event.scancode>=oscillations_count)
					{
						if (event.scancode>=oscillations_size)
						{
							if (oscillations)
							{
								oscillations=(Oscillation *)realloc(oscillations, sizeof(Oscillation)*(event.scancode+1));
								memset(oscillations+oscillations_size, 0, sizeof(Oscillation)*(event.scancode-oscillations_size));
							}
							else
							{
								oscillations=(Oscillation *)malloc(sizeof(Oscillation)*(event.scancode+1));
								memset(oscillations, 0, sizeof(Oscillation)*event.scancode);
							}
							oscillations_count=oscillations_size=event.scancode+1;
						}
						else oscillations_count=event.scancode+1;
					}
					oscillations[event.scancode]=Oscillation(event.pitch/project->sample_rate, event.velocity/127.f, event.phase);
					break;
				case NOTE_CHANGE:
					if (event.scancode<oscillations_count) if (oscillations[event.scancode].exist) oscillations[event.scancode].change(event.pitch/project->sample_rate, event.velocity/127.f);
					break;
				case NOTE_STOP:
					if (event.scancode<oscillations_count) if (oscillations[event.scancode].exist)
					{
						oscillations[event.scancode].exist=false;
						if (event.scancode==oscillations_count-1) while (--oscillations_count) if (oscillations[oscillations_count-1].exist) break;
					}
			}
		}
		output.frame=0;
		switch (waveform.value)
		{
			case 0:
				for (uint32_t a=0; a!=oscillations_count; a++) if (oscillations[a].exist) output.frame+=volume.value*oscillations[a].velocity*std::sin(oscillations[a].phase*2*(float)M_PI);
				break;
			case 1:
				for (uint32_t a=0; a!=oscillations_count; a++) if (oscillations[a].exist)
				{
					float phase=oscillations[a].phase;
					output.frame+=4*volume.value*oscillations[a].velocity*((phase>=.25f&&phase<.75f)*(.5f-phase)+(phase<.25f)*phase+(phase>=.75f)*(phase-1));
				}
				break;
			case 2:
				for (uint32_t a=0; a!=oscillations_count; a++) if (oscillations[a].exist) output.frame+=volume.value*oscillations[a].velocity*(1-2*oscillations[a].phase);
				break;
			case 3:
				for (uint32_t a=0; a!=oscillations_count; a++) if (oscillations[a].exist) output.frame+=volume.value*oscillations[a].velocity*((oscillations[a].phase<.5f)-(oscillations[a].phase>=.5f));
		}
		for (uint32_t a=0; a!=oscillations_count; a++) if (oscillations[a].exist) oscillations[a].update();
	}
}