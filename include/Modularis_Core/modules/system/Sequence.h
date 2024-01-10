/*
(C) 2023-2024 Серый MLGamer. All freedoms preserved.
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

#include <Modularis_Core/typedefs/modules/system/Sequence.h>

#include <Modularis_Core/typedefs/modules/system/Sequence_note.h>
#include <stdint.h>

struct MDLRS_Sequence
{
	MDLRS_Sequence_note *sequence;
	uint32_t note;
	uint32_t time;
};
inline void MDLRS_Sequence_new(MDLRS_Sequence *self, MDLRS_Sequence_note *sequence)
{
	self->sequence=sequence;
	self->note=0;
	self->time=0;
}