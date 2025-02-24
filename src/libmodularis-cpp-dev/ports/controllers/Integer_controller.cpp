/*
(C) 2023-2024 Серый MLGamer. All freedoms preserved.
Дзен: <https://dzen.ru/seriy_mlgamer>
SoundCloud: <https://soundcloud.com/seriy_mlgamer>
YouTube: <https://www.youtube.com/@Seriy_MLGamer>
GitHub: <https://github.com/Seriy-MLGamer>
E-mail: <Seriy-MLGamer@yandex.ru>

This file is part of Modularis Core C++.
Modularis Core C++ is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Modularis Core C++ is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with Modularis Core C++. If not, see <https://www.gnu.org/licenses/>.
*/

#include <Modularis_Core_C++/ports/controllers/Integer_controller.hpp>

#include <cstdint>
#include <Modularis_Core_C++/system/ports/Connection.hpp>

extern "C" void MDLRS_Integer_controller_new_body(MDLRS::Integer_controller *self, int32_t value);

namespace MDLRS
{
	Integer_controller::Integer_controller(Module *module, int32_t value): Port("Integer_controller", module)
	{
		MDLRS_Integer_controller_new_body(this, value);
	}
	void Integer_controller::on_update()
	{
		if (connections_count)
		{
			value=0;
			for (uint32_t a=0; a!=connections_count; a++) value+=((Integer_controller *)connections[a].port)->value;
			value/=connections_count;
		}
	}
}