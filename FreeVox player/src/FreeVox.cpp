/*
(C) 2022-2023 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#include <FreeVox/player/FreeVox.hpp>

#include <exception>
#include <algorithm>
#include <cstring>

namespace FV
{
	using namespace std;

	FreeVox::FreeVox(uint32_t sample_rate, Channels channels)
	{
		if (channels<MONO||channels>STEREO) throw exception();
		this->sample_rate=sample_rate;
		this->channels=channels;
		output=Output(*this);
		auto_prepare=true;
		prepare_request=false;
	}
	void FreeVox::process(Sound_value *frame)
	{
		if (auto_prepare&&prepare_request)
		{
			prepare_to_processing();
			prepare_request=false;
		}
		for_each(processing_stages.rbegin(), processing_stages.rend(),
			[](Stage &stage)
			{
				for_each(stage.begin(), stage.end(),
					[](Module *module)
					{
						module->process();
					}
				);
			}
		);
		memcpy(frame, output.frame, sizeof(Sound_value)*channels);
	}
	void FreeVox::prepare_to_processing()
	{
		for_each(modules.begin(), modules.end(),
			[](Module *module)
			{

			}
		);
	}
	void FreeVox::play()
	{

	}
	void FreeVox::pause()
	{

	}
}