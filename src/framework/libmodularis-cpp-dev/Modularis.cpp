#include <Modularis_Core_C++/Modularis.hpp>

extern "C" void MDLRS_Modularis_update(MDLRS::Modularis *self);

namespace MDLRS
{
	void Modularis::update()
	{
		MDLRS_Modularis_update(this);
	}
}