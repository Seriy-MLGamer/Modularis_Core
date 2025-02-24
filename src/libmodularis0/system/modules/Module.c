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

#include <Modularis_Core/system/modules/Module.h>

#include <Modularis_Core/Modularis.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

static void on_update(MDLRS_Module *self);
static struct MDLRS_Module_f f=
{
	(void (*)(void *))on_update
};

void MDLRS_Module_new_body(MDLRS_Module *self, MDLRS_Modularis *project)
{
	self->f=&f;

	self->project=project;
	if (project->disconnected_modules)
	{
		if (project->disconnected_modules_count==project->disconnected_modules_size) project->disconnected_modules=realloc(project->disconnected_modules, sizeof(MDLRS_Module *)*(project->disconnected_modules_size+=8));
		project->disconnected_modules[project->disconnected_modules_count++]=self;
		self->index=project->disconnected_modules_count;
	}
	else
	{
		project->disconnected_modules=malloc(sizeof(MDLRS_Module *)*8);
		*project->disconnected_modules=self;
		project->disconnected_modules_size=8;
		project->disconnected_modules_count=1;
		self->index=1;
	}
	self->output_connections=0;
	self->ready=true;
}
void MDLRS_Module_new(MDLRS_Module *self, MDLRS_Modularis *project)
{
	MDLRS_Ports_folder_new(&self->inputs);
	MDLRS_Ports_folder_new(&self->outputs);
	MDLRS_Module_new_body(self, project);
}
void MDLRS_Module_new_empty_body(MDLRS_Module *self)
{
	self->f=&f;
}
void MDLRS_Module_new_empty(MDLRS_Module *self)
{
	MDLRS_Ports_folder_new(&self->inputs);
	MDLRS_Ports_folder_new(&self->outputs);
	self->f=&f;
}
void on_update(MDLRS_Module *self){}
void MDLRS_Module_remove_body(MDLRS_Module *self)
{
	if (self->index)
	{
		MDLRS_Module **modules=self->project->disconnected_modules;
		uint32_t count=self->project->disconnected_modules_count--;
		for (uint32_t a=self->index; a!=count; a++)
		{
			modules[a]->index--;
			modules[a-1]=modules[a];
		}
	}
}
void MDLRS_Module_remove(MDLRS_Module *self)
{
	MDLRS_Module_remove_body(self);
	MDLRS_Ports_folder_remove(&self->inputs);
	MDLRS_Ports_folder_remove(&self->outputs);
}