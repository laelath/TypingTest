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

#ifndef TYPING_TEST_FILES_H
#define TYPING_TEST_FILES_H

#include <iostream>
#include <fstream>
#include <string>

namespace typingtest {

// Checks for a file with the given path with ".swp" appended. If such a file
// already exists then it tries appending the number 1 to the end.  Continues
// to do this until a file that doesn't already exists is found.
//
// Tries opening 20 files. If a suitable file could not be found then it throws
// a runtime_exception.
std::string getSwapPath(const std::string &path);

// Renames the file at swapPath to the file at path. Throws a runtime_error on
// failure.
void save(const std::string &path, const std::string &swapPath);
} // namespace typingtest

#endif // TYPING_TEST_FILES_H
