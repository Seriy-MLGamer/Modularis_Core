/*
(C) 2023 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <FreeVox/player/modules/ports/system/Input_port.hpp>

#include <vector>
#include <FreeVox/player/core/types/Sound_value.hpp>

namespace FV
{
	struct Sound_output;

	using namespace std;

	struct Sounds_input: Input_port
	{
		vector<Sound_output *> connections;

		inline Sounds_input();
		void sum(Sound_value *output);
	};
	Sounds_input::Sounds_input(): Input_port("Sounds_input"){}
}