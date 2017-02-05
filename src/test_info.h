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
// TypingTest.  If not, see <http://www.gnu.org/licenses/>.

#ifndef TEST_INFO_H
#define TEST_INFO_H

#include <string>
#include <chrono>

#include "test_settings.h"

namespace typingtest {

// TestInfo represents the pertinent information for recording a test for
// statistics purposes. It records the wpm for things like finding the user's
// record speed and type to generate an informational string.
class TestInfo {
public:
	TestInfo();
	TestInfo(int wpm, std::chrono::seconds length, TestSettings::TestType type);
	TestInfo(int wpm, const TestSettings& settings);

	int getWpm() const;
	void setWpm(int wpm);
	std::chrono::seconds getLength() const;
	void setLength(std::chrono::seconds length);
	void setFromSettings(const TestSettings& settings);

private:
	// Average WPM for test.
	int wpm;
	// Length of the test in seconds.
	std::chrono::seconds length;
	// Type of test taken.
	TestSettings::TestType type;
};
} // namespace typingtest

#endif // TEST_INFO_H
