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

#include <Modularis_Core/typedefs/modules/effects/sound/Delay.h>
#include <Modularis_Core/system/modules/Module.h>

#include <Modularis_Core/ports/Sound.h>
#include <Modularis_Core/ports/controllers/Real_controller.h>
#include <Modularis_Core/system/types/Sound_value.h>
#include <stdint.h>
#include <Modularis_Core/typedefs/Modularis.h>
#include <stdlib.h>

struct MDLRS_Delay
{
	MDLRS_Module;

	MDLRS_Sound input;
	MDLRS_Real_controller delay;
	MDLRS_Sound output;
	MDLRS_Sound_value *buffer;
	uint32_t buffer_size;
	uint32_t position;
};
void MDLRS_Delay_new(MDLRS_Delay *self, MDLRS_Modularis *project, float delay);
void MDLRS_Delay_on_update(MDLRS_Delay *self);
inline void MDLRS_Delay_remove(MDLRS_Delay *self)
{
	MDLRS_Module_disconnect((MDLRS_Module *)self);
	free(self->buffer);

	MDLRS_Port_remove((MDLRS_Port *)&self->input);
	MDLRS_Port_remove((MDLRS_Port *)&self->delay);
	MDLRS_Port_remove((MDLRS_Port *)&self->output);
	MDLRS_Module_remove((MDLRS_Module *)self);
}