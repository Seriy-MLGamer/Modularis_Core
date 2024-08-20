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

#include <Modularis_Core/typedefs/modules/players/Sequencer.h>
#include <Modularis_Core/system/modules/Module.h>

#include <Modularis_Core/ports/controllers/Real_controller.h>
#include <Modularis_Core/ports/controllers/Integer_controller.h>
#include <Modularis_Core/ports/Note.h>
#include <Modularis_Core/typedefs/system/modules/players/Sequencer/Any_pattern.h>
#include <Modularis_Core/typedefs/system/modules/players/Sequencer/Patterns_sequence_data.h>
#include <Modularis_Core/typedefs/system/modules/players/Sequencer/Pattern_data.h>
#include <stdint.h>
#include <stdbool.h>
#include <Modularis_Core/typedefs/Modularis.h>
#include <stdlib.h>

struct MDLRS_Sequencer
{
	MDLRS_Module;

	MDLRS_Real_controller BPM;
	MDLRS_Integer_controller LPB;
	MDLRS_Real_controller cursor_position;
	MDLRS_Integer_controller loop;
	MDLRS_Integer_controller play;
	MDLRS_Note output;
	MDLRS_Any_pattern ***tracks;
	MDLRS_Patterns_sequence_data *tracks_data;
	MDLRS_Pattern_data *patterns_data;
	uint32_t tracks_count;
	uint32_t sequences_count;
	uint32_t time;
	bool is_playing;
	bool is_position_changed;
};
void MDLRS_Sequencer_new(MDLRS_Sequencer *self, MDLRS_Modularis *project);
void MDLRS_Sequencer_add(MDLRS_Sequencer *self, MDLRS_Any_pattern ***tracks, uint32_t tracks_count);
inline void MDLRS_Sequencer_set_position(MDLRS_Sequencer *self, float cursor_position)
{
	self->cursor_position.value=cursor_position;
	self->is_position_changed=true;
}
void MDLRS_Sequencer_on_update(MDLRS_Sequencer *self);
inline void MDLRS_Sequencer_remove(MDLRS_Sequencer *self)
{
	MDLRS_Module_disconnect((MDLRS_Module *)self);
	if (self->tracks_data) free(self->tracks_data);
	if (self->patterns_data) free(self->patterns_data);

	MDLRS_Port_remove((MDLRS_Port *)&self->BPM);
	MDLRS_Port_remove((MDLRS_Port *)&self->LPB);
	MDLRS_Port_remove((MDLRS_Port *)&self->cursor_position);
	MDLRS_Port_remove((MDLRS_Port *)&self->loop);
	MDLRS_Port_remove((MDLRS_Port *)&self->play);
	MDLRS_Note_remove(&self->output);
	MDLRS_Module_remove((MDLRS_Module *)self);
}