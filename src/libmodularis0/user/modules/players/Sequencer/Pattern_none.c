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

#include <Modularis_Core/user/modules/players/Sequencer/Pattern_none.h>

static uint32_t get_tracks_count(MDLRS_Pattern_none *self);
static void null_position(MDLRS_Pattern_none *self, MDLRS_Pattern_data *data);
static void change_position(MDLRS_Pattern_none *self, MDLRS_Pattern_data *data, float time);
static void start(MDLRS_Pattern_none *self, MDLRS_Note *output, MDLRS_Pattern_data *data, uint32_t scancode, float time);
static struct MDLRS_Any_pattern_f f=
{
	(uint32_t (*)(void *))get_tracks_count,
	(void (*)(void *, MDLRS_Pattern_data *))null_position,
	(void (*)(void *, MDLRS_Pattern_data *, float))change_position,
	(void (*)(void *, MDLRS_Note *, MDLRS_Pattern_data *, uint32_t, float))start,
	(void (*)(void *, MDLRS_Note *, MDLRS_Pattern_data *, uint32_t, float))start
};

void MDLRS_Pattern_none_new(MDLRS_Pattern_none *self, float length)
{
	self->f=&f;

	self->length=length;
}
uint32_t get_tracks_count(MDLRS_Pattern_none *self)
{
	return 0;
}
void null_position(MDLRS_Pattern_none *self, MDLRS_Pattern_data *data){}
void change_position(MDLRS_Pattern_none *self, MDLRS_Pattern_data *data, float time){}
void start(MDLRS_Pattern_none *self, MDLRS_Note *output, MDLRS_Pattern_data *data, uint32_t scancode, float time){}