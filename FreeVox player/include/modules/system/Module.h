/*
Copyright (C) 2022 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <typedefs/modules/system/Module.h>
#include <system/structures/linked list/List_node.h>

#include <system/structures/Color.h>

struct Module
{
	List_node;
	const void *f;

	const char *name;
	float x, y;
	Color color;
	Module **inputs, **outputs;
};
typedef const struct Module_f
{
	void (*update)(void *self);
} *Module_f;