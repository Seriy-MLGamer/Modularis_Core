#pragma once

namespace MDLRS
{
	struct Discrete_key
	{
		float value;
		float duration;

		inline Discrete_key(float value, float duration);
	};
	Discrete_key::Discrete_key(float value, float duration)
	{
		this->value=value;
		this->duration=duration;
	}
}