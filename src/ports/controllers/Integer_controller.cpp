/*
(C) 2023 Серый MLGamer. All freedoms preserved.
Дзен: <https://dzen.ru/seriy_mlgamer>
SoundCloud: <https://soundcloud.com/seriy_mlgamer>
YouTube: <https://www.youtube.com/@Seriy_MLGamer>
GitHub: <https://github.com/Seriy-MLGamer>
E-mail: <Seriy-MLGamer@yandex.ru>

This file is part of Modularis Core.
Modularis Core is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Modularis Core is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with Modularis Core. If not, see <https://www.gnu.org/licenses/>.
*/

#include <Modularis_Core/ports/controllers/Integer_controller.hpp>

#include <cstdint>
#include <Modularis_Core/ports/system/Connection.hpp>

namespace MDLRS
{
	void Integer_controller::process()
	{
		if (connections_count)
		{
			value=0;
			for (uint32_t a=0; a!=connections_count; a++) value+=((Integer_controller *)connections[a].port)->value;
			value/=connections_count;
		}
	}
}