/*
(C) 2023 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#include <FreeVox/player/modules/Module.hpp>

#include <FreeVox/player/FreeVox.hpp>

namespace FV
{
	Module::Module(FreeVox &project)
	{
		this->project=&project;
		project.modules.push_back(this);
		index=project.modules.end();
		inputs=vector();
		outputs=vector();
		project.prepare_request=true;
	}
	Module::~Module()
	{
		project->modules.erase(index);
		project->prepare_request=true;
	}
}