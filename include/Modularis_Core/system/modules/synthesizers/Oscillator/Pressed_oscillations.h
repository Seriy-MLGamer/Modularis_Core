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

#include <Modularis_Core/typedefs/system/modules/synthesizers/Oscillator/Pressed_oscillations.h>

#include <Modularis_Core/typedefs/system/ports/Port.h>
#include <stdint.h>
#include <stddef.h>
#include <Modularis_Core/typedefs/system/modules/synthesizers/Oscillator/Released_oscillations.h>
#include <Modularis_Core/system/modules/synthesizers/Oscillator/Oscillation.h>
#include <stdlib.h>

struct MDLRS_Pressed_oscillations
{
	MDLRS_Port *connection;
	MDLRS_Oscillation *oscillations;
	uint32_t oscillations_size;
	uint32_t oscillations_count;
};
inline void MDLRS_Pressed_oscillations_new(MDLRS_Pressed_oscillations *self, MDLRS_Port *connection)
{
	self->connection=connection;
	self->oscillations=NULL;
	self->oscillations_size=0;
	self->oscillations_count=0;
}
void MDLRS_Pressed_oscillations_start(MDLRS_Pressed_oscillations *self, uint32_t scancode, MDLRS_Released_oscillations *released, float phase_speed, float phase, float velocity);
inline void MDLRS_Pressed_oscillations_change(MDLRS_Pressed_oscillations *self, uint32_t scancode, float phase_speed, float velocity)
{
	if (scancode<self->oscillations_count) if (self->oscillations[scancode].exists) MDLRS_Oscillation_change(self->oscillations+scancode, phase_speed, velocity);
}
void MDLRS_Pressed_oscillations_stop(MDLRS_Pressed_oscillations *self, uint32_t scancode, MDLRS_Released_oscillations *released);
inline void MDLRS_Pressed_oscillations_update(MDLRS_Pressed_oscillations *self)
{
	for (uint32_t a=0; a!=self->oscillations_count; a++) if (self->oscillations[a].exists) MDLRS_Oscillation_update(self->oscillations+a);
}
inline void MDLRS_Pressed_oscillations_remove(MDLRS_Pressed_oscillations *self)
{
	if (self->oscillations) free(self->oscillations);
}