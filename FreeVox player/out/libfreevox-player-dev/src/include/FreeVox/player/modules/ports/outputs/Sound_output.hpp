/*
(C) 2023 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <FreeVox/player/modules/ports/system/Output_port.hpp>

#include <cstring>
#include <FreeVox/player/FreeVox.hpp>

namespace FV
{
	struct Sound_output: Output_port
	{
		Sound_value *frame;

		inline Sound_output(Module &parent);
		int
			connect(Input_port &port),
			disconnect(Input_port &port);
		inline ~Sound_output();
	};
	Sound_output::Sound_output(Module &parent): Output_port(parent)
	{
		frame=new Sound_value[parent.project->channels];
		memset(frame, 0, sizeof(Sound_value)*parent.project->channels);
	}
	Sound_output::~Sound_output()
	{
		delete frame;
	}
}