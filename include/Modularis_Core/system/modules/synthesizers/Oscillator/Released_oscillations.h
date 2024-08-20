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

#include <Modularis_Core/typedefs/system/modules/synthesizers/Oscillator/Released_oscillations.h>

#include <stdint.h>
#include <stddef.h>
#include <Modularis_Core/system/modules/synthesizers/Oscillator/Pressed_oscillations.h>
#include <Modularis_Core/system/modules/synthesizers/Oscillator/Oscillation.h>
#include <Modularis_Core/typedefs/ports/controllers/ADSR.h>
#include <stdlib.h>

struct MDLRS_Released_oscillations
{
	MDLRS_Oscillation *oscillations;
	uint32_t oscillations_size;
	uint32_t oscillations_count;
};
inline void MDLRS_Released_oscillations_new(MDLRS_Released_oscillations *self)
{
	self->oscillations=NULL;
	self->oscillations_size=0;
	self->oscillations_count=0;
}
void MDLRS_Released_oscillations_push(MDLRS_Released_oscillations *self, MDLRS_Oscillation *oscillation);
inline void MDLRS_Released_oscillations_move(MDLRS_Released_oscillations *self, MDLRS_Pressed_oscillations *pressed)
{
	for (uint32_t a=0; a!=pressed->oscillations_count; a++) if (pressed->oscillations[a].exists) MDLRS_Released_oscillations_push(self, pressed->oscillations+a);
	MDLRS_Pressed_oscillations_remove(pressed);
}
inline void MDLRS_Released_oscillations_disable(MDLRS_Released_oscillations *self, uint32_t oscillation)
{
	uint32_t count=--self->oscillations_count;
	for (; oscillation!=count; oscillation++) self->oscillations[oscillation]=self->oscillations[oscillation+1];
}
void MDLRS_Released_oscillations_update(MDLRS_Released_oscillations *self, MDLRS_ADSR *envelope);
inline void MDLRS_Released_oscillations_remove(MDLRS_Released_oscillations *self)
{
	if (self->oscillations) free(self->oscillations);
}