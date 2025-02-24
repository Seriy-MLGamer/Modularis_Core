#include <Modularis_Core/system/modules/synthesizers/Sampler/Sample.h>

#include <Modularis_Core/modules/synthesizers/Sampler.h>

void MDLRS_Sample_update(MDLRS_Sample *self, MDLRS_Sampler *data)
{
	self->frame+=self->speed;
	if (data->loop.value) if (self->frame>=data->loop_start.value+data->loop_length.value) self->frame-=data->loop_length.value;
	self->time++;
}