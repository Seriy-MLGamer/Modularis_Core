/*
(C) 2022-2024 Серый MLGamer. All freedoms preserved.
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

#pragma once

#include <cstdint>
#include <Modularis_Core_C++/ports/Ports_folder.hpp>

namespace MDLRS
{
	struct Modularis;

	struct Module
	{
		Modularis *project;
		uint32_t index;
		Ports_folder inputs, outputs;
		uint32_t output_connections;
		bool ready;

		Module(Modularis &project);
		Module();
		inline void
			update(),
			get_ready(),
			disconnect();
		virtual void on_update();
		~Module();
	};
	void Module::update()
	{
		if (ready)
		{
			ready=false;
			inputs.update();
			on_update();
		}
	}
	void Module::get_ready()
	{
		if (ready) return;
		ready=true;
		inputs.get_ready();
	}
	void Module::disconnect()
	{
		inputs.disconnect_input();
		outputs.disconnect();
	}
}