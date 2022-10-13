/*
Copyright (C) 2022 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <typedefs/system/structures/linked list/List_node.h>

#include <system/structures/linked list/Linked_list.h>

struct List_node
{
	List_node *prev, *next;
};
inline void List_node_set(List_node *self, Linked_list *list)
{
	self->next=NULL;
	List_node *last=list->last;
	if (last)
	{
		last->next=self;
		self->prev=last;
	}
	else
	{
		list->first=self;
		self->prev=NULL;
	}
	list->last=self;
}
inline void List_node_unset(List_node *self, Linked_list *list)
{
	List_node
		*prev=self->prev,
		*next=self->next;
	if (prev)
	{
		if (next)
		{
			prev->next=next;
			next->prev=prev;
		}
		else
		{
			prev->next=NULL;
			list->last=prev;
		}
	}
	else if (next)
	{
		next->prev=NULL;
		list->first=next;
	}
	else *list=Linked_list_new();
}