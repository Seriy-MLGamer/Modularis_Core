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

#include <Modularis_Core/typedefs/system/modules/synthesizers/Sampler/Pressed_samples.h>

#include <Modularis_Core/typedefs/system/ports/Port.h>
#include <stdint.h>
#include <stddef.h>
#include <Modularis_Core/typedefs/system/modules/synthesizers/Sampler/Released_samples.h>
#include <Modularis_Core/system/modules/synthesizers/Sampler/Sample.h>
#include <Modularis_Core/typedefs/modules/synthesizers/Sampler.h>
#include <stdlib.h>

struct MDLRS_Pressed_samples
{
	MDLRS_Port *connection;
	MDLRS_Sample *samples;
	uint32_t samples_size;
	uint32_t samples_count;
};
inline void MDLRS_Pressed_samples_new(MDLRS_Pressed_samples *self, MDLRS_Port *connection)
{
	self->connection=connection;
	self->samples=NULL;
	self->samples_size=0;
	self->samples_count=0;
}
void MDLRS_Pressed_samples_start(MDLRS_Pressed_samples *self, uint32_t scancode, MDLRS_Released_samples *released, float speed, float frame, float velocity);
inline void MDLRS_Pressed_samples_change(MDLRS_Pressed_samples *self, uint32_t scancode, float speed, float velocity)
{
	if (scancode<self->samples_count) if (self->samples[scancode].exists) MDLRS_Sample_change(self->samples+scancode, speed, velocity);
}
void MDLRS_Pressed_samples_stop(MDLRS_Pressed_samples *self, uint32_t scancode, MDLRS_Released_samples *released);
void MDLRS_Pressed_samples_update(MDLRS_Pressed_samples *self, MDLRS_Sampler *data);
inline void MDLRS_Pressed_samples_remove(MDLRS_Pressed_samples *self)
{
	if (self->samples) free(self->samples);
}