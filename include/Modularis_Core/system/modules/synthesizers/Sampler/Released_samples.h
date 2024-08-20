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

#include <Modularis_Core/typedefs/system/modules/synthesizers/Sampler/Released_samples.h>

#include <stdint.h>
#include <stddef.h>
#include <Modularis_Core/system/modules/synthesizers/Sampler/Pressed_samples.h>
#include <Modularis_Core/system/modules/synthesizers/Sampler/Sample.h>
#include <Modularis_Core/typedefs/modules/synthesizers/Sampler.h>
#include <stdlib.h>

struct MDLRS_Released_samples
{
	MDLRS_Sample *samples;
	uint32_t samples_size;
	uint32_t samples_count;
};
inline void MDLRS_Released_samples_new(MDLRS_Released_samples *self)
{
	self->samples=NULL;
	self->samples_size=0;
	self->samples_count=0;
}
void MDLRS_Released_samples_push(MDLRS_Released_samples *self, MDLRS_Sample *sample);
inline void MDLRS_Released_samples_move(MDLRS_Released_samples *self, MDLRS_Pressed_samples *pressed)
{
	for (uint32_t a=0; a!=pressed->samples_count; a++) if (pressed->samples[a].exists) MDLRS_Released_samples_push(self, pressed->samples+a);
	MDLRS_Pressed_samples_remove(pressed);
}
inline void MDLRS_Released_samples_disable(MDLRS_Released_samples *self, uint32_t sample)
{
	uint32_t count=--self->samples_count;
	for (; sample!=count; sample++) self->samples[sample]=self->samples[sample+1];
}
void MDLRS_Released_samples_update(MDLRS_Released_samples *self, MDLRS_Sampler *data);
inline void MDLRS_Released_samples_remove(MDLRS_Released_samples *self)
{
	if (self->samples) free(self->samples);
}