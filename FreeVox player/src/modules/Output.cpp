/*
(C) 2023 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#include <FreeVox/player/modules/Output.hpp>

#include <cstring>
#include <FreeVox/player/FreeVox.hpp>

namespace FV
{
	Output::Output(FreeVox &project): Module(project)
	{
		input=Sounds_input(*this);
		volume=Real_controller(1);
		frame=new Sound_value[project.channels];
		memset(frame, 0, sizeof(Sound_value)*project.channels);
		inputs.push(&input);
	}
	void Output::process()
	{
		input.sum(frame);
		float controller=volume.get_value();
		Channels channels=project->channels;
		for (Channels a=0; a!=channels; a++) frame[a]*=controller;
	}
}