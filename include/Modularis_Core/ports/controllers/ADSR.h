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

#include <Modularis_Core/typedefs/ports/controllers/ADSR.h>
#include <Modularis_Core/ports/system/Ports_folder.h>

#include <Modularis_Core/ports/controllers/Real_controller.h>
#include <Modularis_Core/typedefs/modules/system/Module.h>

struct MDLRS_ADSR
{
	MDLRS_Ports_folder;

	MDLRS_Real_controller attack, decay, sustain, release;
};
void MDLRS_ADSR_new(MDLRS_ADSR *self, MDLRS_Module *module, float attack, float decay, float sustain, float release);
inline void MDLRS_ADSR_remove(MDLRS_ADSR *self)
{
	MDLRS_Port_remove((MDLRS_Port *)&self->attack);
	MDLRS_Port_remove((MDLRS_Port *)&self->decay);
	MDLRS_Port_remove((MDLRS_Port *)&self->sustain);
	MDLRS_Port_remove((MDLRS_Port *)&self->release);

	MDLRS_Ports_folder_remove((MDLRS_Ports_folder *)self);
}