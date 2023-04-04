/*
(C) 2022-2023 Серый MLGamer <Seriy-MLGamer@yandex.ru>

This file is part of FreeVox player.
FreeVox player is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
FreeVox player is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with FreeVox player. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <FreeVox/player/modules/Output.hpp>

namespace FV
{
	using namespace std;

	typedef vector<Module *> Stage;
	enum Channels
	{
		MONO=1,
		STEREO=2
	};

	/**
	 * The class for creating instances of the FreeVox program that contains FreeVox project and generates the sound stream. This class encapsulates environment variables of the instance and separates them from the library global variables.
	 * The main framework class for the FreeVox-based app.
	 */
	struct FreeVox
	{
		/**
		 * The sample rate of the sound stream.
		 */
		uint32_t sample_rate;
		/**
		 * The number of channels in the stream. Can be only MONO or STEREO.
		 */
		Channels channels;
		/**
		 * The modules of the FreeVox project.
		 */
		list<Module *> modules;
		/**
		 * The module for returning new frames of the sound stream.
		 */
		Output output;
		/**
		 * The data of the optimized modules processing algorithm.
		 */
		vector<Stage> processing_stages;
		/**
		 * Allow responding to requests of prepare_to_processing() at some events. Disabling is helpful at loading of the FreeVox project from the file.
		 */
		bool auto_prepare;
		/**
		 * If true, prepare_to_processing() will be called at process() call.
		 */
		bool prepare_request;

		/**
		 * Creates an instance of the FreeVox program that generates the sound stream.
		 * @param sample_rate The sample rate of the sound stream.
		 * @param channels The number of channels in the stream. Can be only MONO or STEREO.
		 */
		FreeVox(uint32_t sample_rate, Channels channels);
		/**
		 * Processes all the modules in the FreeVox project and returns the generated frame of the sound stream into the output argument.
		 * Modules are processed by the optimized algorithm, supporting multithreading, but its data must be updated via prepare_to_processing() method. But it often happens automatically.
		 * @param output The output for the new frame of the sound stream.
		 */
		void process(Sound_value *output);
		/**
		 * Updates the data of the optimized multithreaded modules processing algorithm. It's often called automatically at some events, but the manual call may be useful at some programmer-defined situations.
		 */
		void prepare_to_processing();
		void play();
		void pause();
	};
}