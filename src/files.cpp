// Copyright (C) 2017 Justin Frank, Jason Waataja
//
// This file is part of TypingTest.
//
// TypingTest is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// TypingTest is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// TypingTest.  If not, see <http:// www.gnu.org/licenses/>.

#include "files.h"

#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>

namespace typingtest {

std::string getSwapPath(const std::string &path)
{
	std::string swapPathTest = path + ".swp";
	if (access(swapPathTest.c_str(), F_OK) != 0)
		return swapPathTest;
	for (int i = 0; i < 20; ++i) {
		swapPathTest = path + ".swp" + std::to_string(i);
		if (access(swapPathTest.c_str(), F_OK) != 0)
			return swapPathTest;
	}
	throw std::runtime_error("Failed to find suitable swap file");
}

void save(const std::string &path, const std::string &swapPath)
{
	std::remove(path.c_str());
	if (std::rename(swapPath.c_str(), path.c_str()) != 0)
		throw std::runtime_error(std::strerror(errno));
}
} // namespace typingtest
