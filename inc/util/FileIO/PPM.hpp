#pragma once

#include "File.hpp"
#include <string>

namespace Aero::RT::Util {
	class PPMOutputFile : public File {
	public:
		PPMOutputFile(std::string name);
	};
}