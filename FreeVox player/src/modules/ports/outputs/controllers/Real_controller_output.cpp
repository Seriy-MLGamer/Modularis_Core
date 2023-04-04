/*
(C) 2023 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#include <FreeVox/player/modules/ports/outputs/controllers/Real_controller_output.hpp>

#include <cstring>
#include <FreeVox/player/FreeVox.hpp>

namespace FV
{
	void Real_controller_output::get_value(Input_port &port, void &output)
	{
		if (!strcmp(input.type, "Real_controller")) (float &)output=value;
	}
	int Real_controller_output::connect(Input_port &port)
	{
		if (!strcmp(port.type, "Real_controller"))
		{
			((Real_controller &)port).input=this;
			parent->project->prepare_request=true;
			return 0;
		}
		return 1;
	}
	int Real_controller_output::disconnect(Input_port &port)
	{
		if (!strcmp(port.type, "Real_controller"))
		{
			((Real_controller &)port).input=NULL;
			parent->project->prepare_request=true;
			return 0;
		}
		return 1;
	}
}