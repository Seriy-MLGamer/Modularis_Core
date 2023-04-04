/*
(C) 2023 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#include <FreeVox/player/modules/ports/inputs/Sounds_input.hpp>

#include <FreeVox/player/modules/ports/outputs/Sound_output.hpp>

namespace FV
{
	void Sounds_input::sum(Sound_value *output)
	{
		Channels channels=parent->project->channels;
		size_t connections_count=connections.size();
		for (Channels a=0; a!=channels; a++)
		{
			Sound_value &channel=output[a];
			channel=0;
			for (size_t b=0; b!=connections_count; b++) channel+=connections[b]->frame[a];
		}
	}
}