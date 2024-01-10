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

#include <Modularis_Core/Modularis.h>

#include <stdint.h>
#include <Modularis_Core/modules/system/Module.h>

void MDLRS_Modularis_update(MDLRS_Modularis *self)
{
	MDLRS_Module_update((MDLRS_Module *)&self->output);
	if (!self->lazy_update) for (uint32_t a=0; a!=self->disconnected_modules_count; a++) MDLRS_Module_update(self->disconnected_modules[a]);
	MDLRS_Module_get_ready((MDLRS_Module *)&self->output);
	if (!self->lazy_update) for (uint32_t a=0; a!=self->disconnected_modules_count; a++) MDLRS_Module_get_ready(self->disconnected_modules[a]);
}