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

#include <Modularis_Core/system/modules/synthesizers/Sampler/Pressed_samples.h>

#include <stdint.h>
#include <stdlib.h>
#include <Modularis_Core/system/modules/synthesizers/Sampler/Sample.h>
#include <string.h>
#include <Modularis_Core/system/modules/synthesizers/Sampler/Released_samples.h>
#include <stdbool.h>
#include <Modularis_Core/modules/synthesizers/Sampler.h>

void MDLRS_Pressed_samples_start(MDLRS_Pressed_samples *self, uint32_t scancode, MDLRS_Released_samples *released, float speed, float frame, float velocity)
{
	if (scancode>=self->samples_count)
	{
		if (scancode>=self->samples_size)
		{
			if (self->samples)
			{
				self->samples=realloc(self->samples, sizeof(MDLRS_Sample)*(scancode+1));
				memset(self->samples+self->samples_size, 0, sizeof(MDLRS_Sample)*(scancode-self->samples_size));
			}
			else
			{
				self->samples=malloc(sizeof(MDLRS_Sample)*(scancode+1));
				memset(self->samples, 0, sizeof(MDLRS_Sample)*scancode);
			}
			self->samples_count=self->samples_size=scancode+1;
		}
		else self->samples_count=scancode+1;
	}
	else if (self->samples[scancode].exists) MDLRS_Released_samples_push(released, self->samples+scancode);
	MDLRS_Sample_new(self->samples+scancode, speed, frame, velocity);
}
void MDLRS_Pressed_samples_stop(MDLRS_Pressed_samples *self, uint32_t scancode, MDLRS_Released_samples *released)
{
	if (scancode<self->samples_count) if (self->samples[scancode].exists)
	{
		MDLRS_Released_samples_push(released, self->samples+scancode);
		self->samples[scancode].exists=false;
		if (scancode==self->samples_count-1) while (--self->samples_count) if (self->samples[self->samples_count-1].exists) break;
	}
}
void MDLRS_Pressed_samples_update(MDLRS_Pressed_samples *self, MDLRS_Sampler *data)
{
	uint32_t a=self->samples_count; while (a)
	{
		a--;
		if (self->samples[a].exists)
		{
			MDLRS_Sample_update(self->samples+a, data);
			if (self->samples[a].frame>=data->length)
			{
				self->samples[a].exists=false;
				if (a==self->samples_count-1) while (--self->samples_count) if (self->samples[self->samples_count-1].exists) break;
			}
		}
	}
}