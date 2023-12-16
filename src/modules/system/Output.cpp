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

#include <Modularis_Core/modules/system/Output.hpp>

#include <cstdint>
#include <cstdlib>
#include <Modularis_Core/ports/Sound.hpp>
#include <cstring>

namespace MDLRS
{
	Output::Output(Modularis *project, uint32_t channels)
	{
		this->project=project;
		index=0;
		ready=true;
		this->channels=(Sound *)malloc(sizeof(Sound)*channels);
		channels_count=channels;
		Sound sound(this);
		for (uint32_t a=0; a!=channels; a++)
		{
			memcpy(this->channels+a, &sound, sizeof(Sound));
			inputs.add(this->channels[a]);
		}
	}
	void Output::process(){}
}