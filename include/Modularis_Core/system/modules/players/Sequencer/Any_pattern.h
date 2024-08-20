/*
(C) 2024 Серый MLGamer. All freedoms preserved.
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

#pragma once

#include <Modularis_Core/typedefs/system/modules/players/Sequencer/Any_pattern.h>

#include <Modularis_Core/typedefs/system/modules/players/Sequencer/Pattern_data.h>
#include <Modularis_Core/typedefs/ports/Note.h>
#include <stdint.h>

struct MDLRS_Any_pattern
{
	void *f;

	float length;
};
typedef struct MDLRS_Any_pattern_f *MDLRS_Any_pattern_f;
struct MDLRS_Any_pattern_f
{
	uint32_t (*get_tracks_count)(void *self);
	void
		(*null_position)(void *self, MDLRS_Pattern_data *data),
		(*change_position)(void *self, MDLRS_Pattern_data *data, float time),
		(*start)(void *self, MDLRS_Note *output, MDLRS_Pattern_data *data, uint32_t scancode, float time),
		(*play)(void *self, MDLRS_Note *output, MDLRS_Pattern_data *data, uint32_t scancode, float time);
};