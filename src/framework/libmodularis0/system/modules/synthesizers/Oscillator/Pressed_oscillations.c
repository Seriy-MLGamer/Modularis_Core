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

#include <Modularis_Core/system/modules/synthesizers/Oscillator/Pressed_oscillations.h>

#include <stdint.h>
#include <stdlib.h>
#include <Modularis_Core/system/modules/synthesizers/Oscillator/Oscillation.h>
#include <string.h>
#include <Modularis_Core/system/modules/synthesizers/Oscillator/Released_oscillations.h>
#include <stdbool.h>

void MDLRS_Pressed_oscillations_start(MDLRS_Pressed_oscillations *self, uint32_t scancode, MDLRS_Released_oscillations *released, float phase_speed, float phase, float velocity)
{
	if (scancode>=self->oscillations_count)
	{
		if (scancode>=self->oscillations_size)
		{
			if (self->oscillations)
			{
				self->oscillations=realloc(self->oscillations, sizeof(MDLRS_Oscillation)*(scancode+1));
				memset(self->oscillations+self->oscillations_size, 0, sizeof(MDLRS_Oscillation)*(scancode-self->oscillations_size));
			}
			else
			{
				self->oscillations=malloc(sizeof(MDLRS_Oscillation)*(scancode+1));
				memset(self->oscillations, 0, sizeof(MDLRS_Oscillation)*scancode);
			}
			self->oscillations_count=self->oscillations_size=scancode+1;
		}
		else self->oscillations_count=scancode+1;
	}
	else if (self->oscillations[scancode].exists) MDLRS_Released_oscillations_push(released, self->oscillations+scancode);
	MDLRS_Oscillation_new(self->oscillations+scancode, phase_speed, phase, velocity);
}
void MDLRS_Pressed_oscillations_stop(MDLRS_Pressed_oscillations *self, uint32_t scancode, MDLRS_Released_oscillations *released)
{
	if (scancode<self->oscillations_count) if (self->oscillations[scancode].exists)
	{
		MDLRS_Released_oscillations_push(released, self->oscillations+scancode);
		self->oscillations[scancode].exists=false;
		if (scancode==self->oscillations_count-1) while (--self->oscillations_count) if (self->oscillations[self->oscillations_count-1].exists) break;
	}
}