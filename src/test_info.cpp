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

namespace typingtest {

TestInfo::TestInfo(int wpm, std::chrono::seconds length, TestSettings::TestType type)
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

void TestInfo::setFromSettings(const TestSettings& settings)
{
	this->length = settings.seconds;
	this->type = settings.type;
}

std::ostream& operator<<(std::ostream& os, const TestInfo& info)
{
	os << info.getWpm() << " " << info.getLength().count() << info.getType();
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

	info.setWpm(wpm);
	info.setLength(std::chrono::seconds(length));
	info.setType(type);
	return is;
}
} // namespace typingtest
