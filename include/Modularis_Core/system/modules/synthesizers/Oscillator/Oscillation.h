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

#include <Modularis_Core/typedefs/system/modules/synthesizers/Oscillator/Oscillation.h>

#include <stdbool.h>
#include <stdint.h>

struct MDLRS_Oscillation
{
	bool exists;
	float phase_speed, phase, velocity;
	uint32_t time; //It's needed for ADSR.
};
inline void MDLRS_Oscillation_new(MDLRS_Oscillation *self, float phase_speed, float phase, float velocity)
{
	self->exists=true;
	self->phase_speed=phase_speed;
	self->phase=phase;
	self->velocity=velocity;
	self->time=0;
}
inline void MDLRS_Oscillation_change(MDLRS_Oscillation *self, float phase_speed, float velocity)
{
	self->phase_speed=phase_speed;
	self->velocity=velocity;
}
inline void MDLRS_Oscillation_update(MDLRS_Oscillation *self)
{
	self->phase+=self->phase_speed;
	if (self->phase>=1) self->phase--;
	self->time++;
}