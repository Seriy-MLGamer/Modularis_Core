/*
Copyright (C) 2022 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <typedefs/system/curves/Curve.h>
#include <system/structures/linked list/List_node.h>

#include <typedefs/patterns/keys/Key.h>

struct Curve
{
	List_node;
	const void *f;
	
	float value;
};
typedef const struct Curve_f
{
	float (*get_value)(void *self, Key *key, float cursor);
} *Curve_f;
inline Curve Curve_new(float value)
{
	Curve self;
	
	self.value=value;
	
	return self;
}