/*
Copyright (C) 2022 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <FreeVox/player/typedefs/FreeVox.h>

#include <system/structures/linked list/Linked_list.h>
#include <modules/system/render/Modules_stages.h>
#include <patterns/render/Keys_chains.h>

struct FV_FreeVox
{
	float cursor;
	Linked_list modules, patterns;
	Modules_stages modules_render;
	Keys_chains patterns_render;
};
FV_FreeVox FV_FreeVox_new();