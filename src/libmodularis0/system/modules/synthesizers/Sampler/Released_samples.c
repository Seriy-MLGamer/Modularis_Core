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

#include <Modularis_Core/system/modules/synthesizers/Sampler/Released_samples.h>

#include <stdlib.h>
#include <Modularis_Core/system/modules/synthesizers/Sampler/Sample.h>
#include <stdint.h>
#include <Modularis_Core/modules/synthesizers/Sampler.h>
#include <Modularis_Core/Modularis.h>
#include <stdbool.h>

void MDLRS_Released_samples_push(MDLRS_Released_samples *self, MDLRS_Sample *sample)
{
	if (self->samples)
	{
		if (self->samples_count==self->samples_size) self->samples=realloc(self->samples, sizeof(MDLRS_Sample)*(self->samples_size+=8));
		self->samples[self->samples_count++]=*sample;
	}
	else
	{
		self->samples=malloc(sizeof(MDLRS_Sample)*8);
		*self->samples=*sample;
		self->samples_size=8;
		self->samples_count=1;
	}
}
void MDLRS_Released_samples_update(MDLRS_Released_samples *self, MDLRS_Sampler *data)
{
	uint32_t sample_rate=data->project->sample_rate;
	uint32_t a=self->samples_count; while (a)
	{
		a--;
		MDLRS_Sample *sample=self->samples+a;
		MDLRS_Sample_update(sample, data);
		if (sample->frame>=data->length) MDLRS_Released_samples_disable(self, a);
		else if (sample->exists)
		{
			if ((float)sample->time/sample_rate>=data->envelope.attack.value+data->envelope.decay.value)
			{
				sample->exists=false;
				sample->time=0;
			}
		}
		else if (!MDLRS_ADSR_released(&data->envelope, (float)sample->time/sample_rate)) MDLRS_Released_samples_disable(self, a);
	}
}