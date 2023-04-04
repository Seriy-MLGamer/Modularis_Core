/*
(C) 2022-2023 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <list>
#include <vector>

namespace FV
{
	struct FreeVox;
	struct Input_port;
	struct Output_port;

	using namespace std;

	struct Module
	{
		FreeVox *project;
		list<Module *>::iterator index;
		vector<Input_port *> inputs;
		vector<Output_port *> outputs;

		Module(FreeVox &project);
		Module()=default;
		virtual void process();
		~Module();
	};
}