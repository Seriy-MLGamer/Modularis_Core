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

#include <Modularis_Core/modules/system/Module.hpp>

#include <Modularis_Core/Modularis.hpp>
#include <cstdlib>
#include <cstdint>

namespace MDLRS
{
	Module::Module(Modularis &project)
	{
		this->project=&project;
		if (project.disconnected_modules)
		{
			if (project.disconnected_modules_count==project.disconnected_modules_size) project.disconnected_modules=(Module **)realloc(project.disconnected_modules, sizeof(Module *)*(project.disconnected_modules_size+=8));
			project.disconnected_modules[project.disconnected_modules_count++]=this;
			index=project.disconnected_modules_count;
		}
		else
		{
			project.disconnected_modules=(Module **)malloc(sizeof(Module *)*8);
			*project.disconnected_modules=this;
			project.disconnected_modules_size=8;
			project.disconnected_modules_count=1;
			index=1;
		}
		output_connections=0;
		ready=true;
	}
	Module::~Module()
	{
		if (index)
		{
			Module **modules=project->disconnected_modules;
			uint32_t count=project->disconnected_modules_count--;
			for (uint32_t a=index; a!=count; a++)
			{
				modules[a]->index--;
				modules[a-1]=modules[a];
			}
		}
	}
}