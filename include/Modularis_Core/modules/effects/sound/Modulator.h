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

#include <Modularis_Core/typedefs/modules/effects/sound/Modulator.h>
#include <Modularis_Core/system/modules/Module.h>

#include <Modularis_Core/ports/Sound.h>
#include <Modularis_Core/typedefs/Modularis.h>

struct MDLRS_Modulator
{
	MDLRS_Module;

	MDLRS_Sound carrier, modulator;
	MDLRS_Sound output;
};
void MDLRS_Modulator_new(MDLRS_Modulator *self, MDLRS_Modularis *project);
void MDLRS_Modulator_on_update(MDLRS_Modulator *self);
inline void MDLRS_Modulator_remove(MDLRS_Modulator *self)
{
	MDLRS_Module_disconnect((MDLRS_Module *)self);

	MDLRS_Port_remove((MDLRS_Port *)&self->carrier);
	MDLRS_Port_remove((MDLRS_Port *)&self->modulator);
	MDLRS_Port_remove((MDLRS_Port *)&self->output);
	MDLRS_Module_remove((MDLRS_Module *)self);
}