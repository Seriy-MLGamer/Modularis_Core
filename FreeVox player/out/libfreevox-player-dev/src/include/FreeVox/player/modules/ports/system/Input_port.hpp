/*
(C) 2023 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

namespace FV
{
	struct Input_port
	{
		const char *type; //У портов, считай, динамическая типизация, так что для ускоренной проверки допустимости соединения выходного и входного портов я решил явно указывать тип порта.

		inline Input_port(const char *type);
		Input_port()=default;
	};
	Input_port::Input_port(const char *type)
	{
		this->type=type;
	}
}