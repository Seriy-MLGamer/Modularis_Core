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

#include <Modularis_Core/typedefs/modules/synthesizers/Oscillator.h>
#include <Modularis_Core/modules/system/Module.h>

#include <Modularis_Core/ports/Note.h>
#include <Modularis_Core/ports/controllers/Real_controller.h>
#include <Modularis_Core/ports/controllers/Integer_controller.h>
#include <Modularis_Core/ports/controllers/ADSR.h>
#include <Modularis_Core/ports/Sound.h>
#include <Modularis_Core/typedefs/modules/system/Oscillation.h>
#include <stdint.h>
#include <Modularis_Core/typedefs/Modularis.h>
#include <stdlib.h>

struct MDLRS_Oscillator
{
	MDLRS_Module;

	MDLRS_Note input;
	MDLRS_Real_controller volume;
	MDLRS_Integer_controller waveform;
	MDLRS_ADSR envelope;
	MDLRS_Sound output;
	MDLRS_Oscillation *oscillations;
	uint32_t oscillations_size;
	uint32_t oscillations_count;
};
void MDLRS_Oscillator_new(MDLRS_Oscillator *self, MDLRS_Modularis *project);
void MDLRS_Oscillator_process(MDLRS_Oscillator *self);
inline void MDLRS_Oscillator_remove(MDLRS_Oscillator *self)
{
	MDLRS_Module_disconnect((MDLRS_Module *)self);
	if (self->oscillations) free(self->oscillations);

	MDLRS_Note_remove(&self->input);
	MDLRS_Port_remove((MDLRS_Port *)&self->volume);
	MDLRS_Port_remove((MDLRS_Port *)&self->waveform);
	MDLRS_ADSR_remove(&self->envelope);
	MDLRS_Port_remove((MDLRS_Port *)&self->output);
	MDLRS_Module_remove((MDLRS_Module *)self);
}