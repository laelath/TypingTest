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

#include "test_info.h"

#include <stdexcept>
#include <vector>
#include <string>
#include <sstream>

namespace typingtest {

TestInfo::TestInfo(int wpm, std::chrono::seconds length,
	TestSettings::TestType type)
	: wpm(wpm), length(length), type(type)
{
}

TestInfo::TestInfo(int wpm, const TestSettings& settings)
	: wpm(wpm), length(settings.seconds), type(settings.type)
{
}

TestInfo::TestInfo() : type(TestSettings::BASIC)
{
}

int TestInfo::getWpm() const
{
	return wpm;
}

void TestInfo::setWpm(int wpm)
{
	this->wpm = wpm;
}

std::chrono::seconds TestInfo::getLength() const
{
	return length;
}

void TestInfo::setLength(std::chrono::seconds length)
{
	this->length = length;
}

TestSettings::TestType TestInfo::getType() const
{
	return type;
}

void TestInfo::setType(TestSettings::TestType type)
{
	this->type = type;
}

bool TestInfo::getHasNote() const
{
	return hasNote;
}

void TestInfo::setHasNote(bool hasNote)
{
	this->hasNote = hasNote;
}

const std::string &TestInfo::getNote() const
{
	return note;
}

void TestInfo::setNote(const std::string &note)
{
	this->note = note;
}

void TestInfo::setFromSettings(const TestSettings& settings)
{
	this->length = settings.seconds;
	this->type = settings.type;
}

std::ostream& operator<<(std::ostream& os, const TestInfo& info)
{
	os << info.getWpm() << " " << info.getLength().count() << " "
		<< info.getType() << std::endl;
	if (info.getHasNote()) {
		os << "note" << std::endl;
		std::vector<std::string> lines;
		std::string line;
		std::istringstream noteStream{info.getNote()};
		while (std::getline(noteStream, line))
			lines.push_back(line);
		os << lines.size() << std::endl;
		for (const auto &line : lines)
			os << line << std::endl;
	} else
		os << "no_note" << std::endl;
	return os;
}

std::istream& operator>>(std::istream& is, TestInfo& info)
{
	int wpm;
	if (!(is >> wpm)) {
		is.setstate(std::ios_base::failbit);
		return is;
	}
	long length;
	if (!(is >> length)) {
		is.setstate(std::ios_base::failbit);
		return is;
	}
	TestSettings::TestType type;
	if (!(is >> type)) {
		is.setstate(std::ios_base::failbit);
		return is;
	}
	std::string hasNote;
	std::string note;
	is >> hasNote;
	if (hasNote == "note") {
		size_t lineCount;
		is >> lineCount;
		std::ostringstream contents;
		for (size_t i = 0; i < lineCount; ++i) {
			std::string line;
			std::getline(is, line);
			contents << line;
			if (i > 0)
				contents << std::endl;
		}
		note = contents.str();
	}

	info.setWpm(wpm);
	info.setLength(std::chrono::seconds(length));
	info.setType(type);
	info.setHasNote(hasNote == "note");
	info.setNote(note);

	return is;
}
} // namespace typingtest
