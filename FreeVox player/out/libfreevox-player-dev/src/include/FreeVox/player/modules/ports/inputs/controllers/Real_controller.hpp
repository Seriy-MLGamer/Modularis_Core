/*
(C) 2023 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <FreeVox/player/modules/ports/system/Input_port.hpp>

#include <FreeVox/player/modules/ports/outputs/controllers/core/Controller_output.hpp>

namespace FV
{
	struct Real_controller: Input_port
	{
		Controller_output *input;
		float value;

		inline Real_controller(float default_value);
		Real_controller()=default;
		inline float get_value();
	}
	Real_controller::Real_controller(float default_value): Input_port("Real_controller")
	{
		input=NULL;
		value=default_value;
	}
	float Real_controller::get_value()
	{
		if (input)
		{
			float result;
			input->get_value(*this, result);
			return result;
		}
		return value;
	}
}