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

#include <Modularis_Core/ports/controllers/ADSR.h>

void MDLRS_ADSR_new(MDLRS_ADSR *self, MDLRS_Module *module, float attack, float decay, float sustain, float release)

{
	MDLRS_Ports_folder_new((MDLRS_Ports_folder *)self);

	MDLRS_Real_controller_new(&self->attack, module, attack);
	MDLRS_Real_controller_new(&self->decay, module, decay);
	MDLRS_Real_controller_new(&self->sustain, module, sustain);
	MDLRS_Real_controller_new(&self->release, module, release);
	MDLRS_Ports_folder_add((MDLRS_Ports_folder *)self, (MDLRS_Port_base *)&self->attack);
	MDLRS_Ports_folder_add((MDLRS_Ports_folder *)self, (MDLRS_Port_base *)&self->decay);
	MDLRS_Ports_folder_add((MDLRS_Ports_folder *)self, (MDLRS_Port_base *)&self->sustain);
	MDLRS_Ports_folder_add((MDLRS_Ports_folder *)self, (MDLRS_Port_base *)&self->release);
}