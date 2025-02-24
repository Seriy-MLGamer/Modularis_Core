#include <Modularis_Core_C++/system/modules/synthesizers/Sampler/Sample.hpp>

#include <Modularis_Core_C++/modules/synthesizers/Sampler.hpp>

namespace MDLRS
{
	void Sample::update(Sampler *data)
	{
		frame+=speed;
		if (data->loop.value) if (frame>=data->loop_start.value+data->loop_length.value) frame-=data->loop_length.value;
		time++;
	}
}