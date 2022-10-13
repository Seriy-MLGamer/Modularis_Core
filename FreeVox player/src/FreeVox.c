/*
Copyright (C) 2022 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#include <FreeVox.h>

FV_FreeVox FV_FreeVox_new()
{
	FV_FreeVox self;

	self;

	return self;
}
static void play(FV_FreeVox *self);
static void pause(FV_FreeVox *self);
static void stop(FV_FreeVox *self);
static void set_cursor(FV_FreeVox *self, float cursor);
static float get_cursor(FV_FreeVox *self);

void
	(*FV_FreeVox_play)(FV_FreeVox *self)=play,
	(*FV_FreeVox_pause)(FV_FreeVox *self)=pause,
	(*FV_FreeVox_stop)(FV_FreeVox *self)=stop,
	(*FV_FreeVox_set_cursor)(FV_FreeVox *self, float cursor)=set_cursor;
float (*FV_FreeVox_get_cursor)(FV_FreeVox *self)=get_cursor;