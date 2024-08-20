/*
(C) 2024 Серый MLGamer. All freedoms preserved.
Дзен: <https://dzen.ru/seriy_mlgamer>
SoundCloud: <https://soundcloud.com/seriy_mlgamer>
YouTube: <https://www.youtube.com/@Seriy_MLGamer>
GitHub: <https://github.com/Seriy-MLGamer>
E-mail: <Seriy-MLGamer@yandex.ru>

This file is part of Modularis Core C++.
Modularis Core C++ is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
Modularis Core C++ is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with Modularis Core C++. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <cstdint>

namespace MDLRS
{
	struct Sampler;

	struct Sample
	{
		bool exists;
		float speed, frame, velocity;
		uint32_t time; //It's needed for ADSR.

		inline Sample(float speed, float frame, float velocity);
		inline void change(float speed, float velocity);
		void update(Sampler *data);
	};
	Sample::Sample(float speed, float frame, float velocity)
	{
		exists=true;
		this->speed=speed;
		this->frame=frame;
		this->velocity=velocity;
		time=0;
	}
	void Sample::change(float speed, float velocity)
	{
		this->speed=speed;
		this->velocity=velocity;
	}
}