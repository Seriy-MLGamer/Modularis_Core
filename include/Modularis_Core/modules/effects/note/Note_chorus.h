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

#include <Modularis_Core/typedefs/modules/effects/note/Note_chorus.h>
#include <Modularis_Core/system/modules/Module.h>

#include <Modularis_Core/ports/Note.h>
#include <Modularis_Core/ports/controllers/Real_controller.h>
#include <Modularis_Core/ports/controllers/Integer_controller.h>
#include <Modularis_Core/typedefs/Modularis.h>
#include <stdint.h>

struct MDLRS_Note_chorus
{
	MDLRS_Module;

	MDLRS_Note input;
	MDLRS_Real_controller spread;
	MDLRS_Integer_controller voices;
	MDLRS_Integer_controller random_phases;
	MDLRS_Note output;
};
void MDLRS_Note_chorus_new(MDLRS_Note_chorus *self, MDLRS_Modularis *project, float spread, uint32_t voices);
void MDLRS_Note_chorus_on_update(MDLRS_Note_chorus *self);
inline void MDLRS_Note_chorus_remove(MDLRS_Note_chorus *self)
{
	MDLRS_Module_disconnect((MDLRS_Module *)self);

	MDLRS_Note_remove(&self->input);
	MDLRS_Port_remove((MDLRS_Port *)&self->spread);
	MDLRS_Port_remove((MDLRS_Port *)&self->voices);
	MDLRS_Port_remove((MDLRS_Port *)&self->random_phases);
	MDLRS_Note_remove(&self->output);
	MDLRS_Module_remove((MDLRS_Module *)self);
}