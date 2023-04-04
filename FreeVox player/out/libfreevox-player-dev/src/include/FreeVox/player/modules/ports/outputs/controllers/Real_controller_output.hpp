/*
(C) 2023 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <FreeVox/player/modules/ports/outputs/controllers/core/Controller_output.hpp>

namespace FV
{
	struct Real_controller_output: Controller_output
	{
		float value;

		inline Real_controller_output(Module &parent);
		void get_value(Input_port &port, void &output);
		int
			connect(Input_port &port),
			disconnect(Input_port &port);
	}
	Real_controller_output::Real_controller_output(Module &parent): Output_port(parent)
	{
		value=0;
	}
}