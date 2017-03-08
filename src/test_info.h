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

#ifndef TYPING_TEST_TEST_INFO_H
#define TYPING_TEST_TEST_INFO_H

#include <chrono>
#include <iostream>
#include <string>

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
	TestSettings::TestType getType() const;
	void setType(TestSettings::TestType type);

	bool getHasNote() const;
	void setHasNote(bool hasNote);
	const std::string &getNote() const;
	void setNote(const std::string &note);

	void setFromSettings(const TestSettings& settings);

	void saveToFile(const std::string &name);
	TestInfo readFromFile(const std::string &name);

private:
	// Average WPM for test.
	int wpm;
	// Length of the test in seconds.
	std::chrono::seconds length;
	// Type of test taken.
	TestSettings::TestType type;
	// Whether or not the note variable matters
	bool hasNote = false;
	std::string note;
};

// Output operator for writing to files.
std::ostream& operator<<(std::ostream& os, const TestInfo& info);
// Input operator for reading from file.
std::istream& operator>>(std::istream& is, TestInfo& info);

} // namespace typingtest

#endif // TYPING_TEST_TEST_INFO_H
