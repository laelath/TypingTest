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

#ifndef TYPING_TEST_TEST_SETTINGS_H
#define TYPING_TEST_TEST_SETTINGS_H

#include <chrono>
#include <cstdlib>
#include <iostream>

namespace typingtest {

const char BASIC_TEST_STRING[] = "basic";
const char ADVANCED_TEST_STRING[] = "advanced";
const char ENDURANCE_TEST_STRING[] = "endurance";
const char CUSTOM_TEST_STRING[] = "custom";

class TestSettings {
public:
	enum TestType {
		BASIC = 0,
		ADVANCED,
		ENDURANCE,
		CUSTOM,
	};

	TestSettings();
	TestSettings(TestType type);
	TestSettings(TestType t, size_t tw, size_t minl, size_t maxl,
		std::chrono::seconds sec, uint32_t sd, double pf);

	void setType(TestType type);

	TestType type;
	size_t topWords;
	size_t minLength;
	size_t maxLength;
	std::chrono::seconds seconds;
	uint32_t seed;
	double personalFrequency;
	double capitalFrequency;
};

std::string toString(TestSettings::TestType type);
std::ostream& operator<<(std::ostream& os, TestSettings::TestType type);
std::istream& operator>>(std::istream& is, TestSettings::TestType& type);

} // namespace typingtest

#endif // TYPING_TEST_TEST_SETTINGS_H
