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

#include <Modularis_Core/system/modules/synthesizers/Oscillator/Released_oscillations.h>

#include <stdlib.h>
#include <Modularis_Core/system/modules/synthesizers/Oscillator/Oscillation.h>
#include <stdint.h>
#include <Modularis_Core/ports/controllers/ADSR.h>
#include <Modularis_Core/system/modules/Module.h>
#include <Modularis_Core/Modularis.h>
#include <stdbool.h>

void MDLRS_Released_oscillations_push(MDLRS_Released_oscillations *self, MDLRS_Oscillation *oscillation)
{
	if (self->oscillations)
	{
		if (self->oscillations_count==self->oscillations_size) self->oscillations=realloc(self->oscillations, sizeof(MDLRS_Oscillation)*(self->oscillations_size+=8));
		self->oscillations[self->oscillations_count++]=*oscillation;
	}
	else
	{
		self->oscillations=malloc(sizeof(MDLRS_Oscillation)*8);
		*self->oscillations=*oscillation;
		self->oscillations_size=8;
		self->oscillations_count=1;
	}
}
void MDLRS_Released_oscillations_update(MDLRS_Released_oscillations *self, MDLRS_ADSR *envelope)
{
	uint32_t sample_rate=envelope->attack.module->project->sample_rate;
	uint32_t a=self->oscillations_count; while (a)
	{
		a--;
		MDLRS_Oscillation *oscillation=self->oscillations+a;
		MDLRS_Oscillation_update(oscillation);
		if (oscillation->exists)
		{
			if ((float)oscillation->time/sample_rate>=envelope->attack.value+envelope->decay.value)
			{
				oscillation->exists=false;
				oscillation->time=0;
			}
		}
		else if (!MDLRS_ADSR_released(envelope, (float)oscillation->time/sample_rate)) MDLRS_Released_oscillations_disable(self, a);
	}
}