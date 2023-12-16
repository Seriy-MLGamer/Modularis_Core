/*
(C) 2023 Серый MLGamer. All freedoms preserved.
Дзен: <https://dzen.ru/seriy_mlgamer>
SoundCloud: <https://soundcloud.com/seriy_mlgamer>
YouTube: <https://www.youtube.com/@Seriy_MLGamer>
GitHub: <https://github.com/Seriy-MLGamer>
E-mail: <Seriy-MLGamer@yandex.ru>

This file is part of Modularis Core C.
Modularis Core C is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Modularis Core C is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with Modularis Core C. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

struct Modularis_Modularis
{
	uint32_t sample_rate; /**Sample rate of sound stream.*/
	bool lazy_update; /**Don't update disconnected modules.*/
	Module **disconnected_modules; /**Modules that aren't connected to any module.*/
	size_t disconnected_modules_count;
	Output output; /**Module for returning new frames of sound stream.*/
};