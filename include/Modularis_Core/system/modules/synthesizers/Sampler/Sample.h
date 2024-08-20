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

#include <Modularis_Core/typedefs/system/modules/synthesizers/Sampler/Sample.h>

#include <stdbool.h>
#include <stdint.h>
#include <Modularis_Core/typedefs/modules/synthesizers/Sampler.h>

struct MDLRS_Sample
{
	bool exists;
	float speed, frame, velocity;
	uint32_t time; //It's needed for ADSR.
};
inline void MDLRS_Sample_new(MDLRS_Sample *self, float speed, float frame, float velocity)
{
	self->exists=true;
	self->speed=speed;
	self->frame=frame;
	self->velocity=velocity;
	self->time=0;
}
inline void MDLRS_Sample_change(MDLRS_Sample *self, float speed, float velocity)
{
	self->speed=speed;
	self->velocity=velocity;
}
void MDLRS_Sample_update(MDLRS_Sample *self, MDLRS_Sampler *data);