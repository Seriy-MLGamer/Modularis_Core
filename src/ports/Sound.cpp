#include <Modularis_Core/ports/Sound.hpp>

#include <cstdint>
#include <Modularis_Core/ports/system/Connection.hpp>

namespace MDLRS
{
	void Sound::process()
	{
		frame=0;
		for (uint32_t a=0; a!=connections_count; a++) frame+=((Sound *)connections[a].port)->frame;
	}
}