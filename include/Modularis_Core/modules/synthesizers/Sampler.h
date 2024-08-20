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

#include <Modularis_Core/typedefs/modules/synthesizers/Sampler.h>
#include <Modularis_Core/system/modules/Module.h>

#include <Modularis_Core/ports/Note.h>
#include <Modularis_Core/ports/controllers/Real_controller.h>
#include <Modularis_Core/ports/controllers/ADSR.h>
#include <Modularis_Core/ports/controllers/Integer_controller.h>
#include <Modularis_Core/system/modules/synthesizers/Sampler/Released_samples.h>
#include <Modularis_Core/typedefs/system/modules/synthesizers/Sampler/Pressed_samples.h>
#include <Modularis_Core/typedefs/ports/Sound.h>
#include <Modularis_Core/system/types/Sound_value.h>
#include <stdint.h>
#include <Modularis_Core/typedefs/Modularis.h>

struct MDLRS_Sampler
{
	MDLRS_Module;

	MDLRS_Note input;
	MDLRS_Real_controller volume;
	MDLRS_ADSR envelope;
	MDLRS_Integer_controller loop;
	MDLRS_Integer_controller loop_start;
	MDLRS_Integer_controller loop_length;
	MDLRS_Released_samples released;
	MDLRS_Pressed_samples *pressed;
	MDLRS_Sound *outputs_array;
	MDLRS_Sound_value *sample;
	uint32_t pressed_count;
	uint32_t sample_rate;
	uint32_t length;
	unsigned channels;
};
void MDLRS_Sampler_new(MDLRS_Sampler *self, MDLRS_Modularis *project, const char *file);
void MDLRS_Sampler_on_update(MDLRS_Sampler *self);
void MDLRS_Sampler_remove(MDLRS_Sampler *self);