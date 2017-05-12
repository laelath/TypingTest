
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

#include "test_settings.h"

#include <string>

namespace typingtest {

TestSettings::TestSettings() : TestSettings(TestSettings::BASIC)
{
}

TestSettings::TestSettings(TestType type)
{
    setType(type);
}

TestSettings::TestSettings(TestType t, size_t tw, size_t minl, size_t maxl,
    std::chrono::seconds sec, uint32_t sd, double pf)
    : type(t),
      topWords(tw),
      minLength(minl),
      maxLength(maxl),
      seconds(sec),
      seed(sd),
      personalFrequency(pf)
{
}

std::ostream& operator<<(std::ostream& os, TestSettings::TestType type)
{
    switch (type) {
    case TestSettings::BASIC:
        os << BASIC_TEST_STRING;
        break;
    case TestSettings::ADVANCED:
        os << ADVANCED_TEST_STRING;
        break;
    case TestSettings::ENDURANCE:
        os << ENDURANCE_TEST_STRING;
        break;
    case TestSettings::CUSTOM:
        os << CUSTOM_TEST_STRING;
        break;
    }
    return os;
}

std::istream& operator>>(std::istream& is, TestSettings::TestType& type)
{
    std::string typeString;
    if (!(is >> typeString)) {
        is.setstate(std::ios_base::failbit);
        return is;
    }

    if (typeString == BASIC_TEST_STRING)
        type = TestSettings::BASIC;
    else if (typeString == ADVANCED_TEST_STRING)
        type = TestSettings::ADVANCED;
    else if (typeString == ENDURANCE_TEST_STRING)
        type = TestSettings::ENDURANCE;
    else if (typeString == CUSTOM_TEST_STRING)
        type = TestSettings::CUSTOM;
    else {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

std::string toString(TestSettings::TestType type)
{
    switch (type) {
    case TestSettings::BASIC:
        return BASIC_TEST_STRING;
    case TestSettings::ADVANCED:
        return ADVANCED_TEST_STRING;
    case TestSettings::ENDURANCE:
        return ENDURANCE_TEST_STRING;
    case TestSettings::CUSTOM:
        return CUSTOM_TEST_STRING;
    }
}

void TestSettings::setType(TestType type)
{
    this->type = type;
    switch (type) {
    case BASIC:
        topWords = 200;
        minLength = 2;
        maxLength = 25;
        seconds = std::chrono::seconds(60);
        capitalFrequency = 0;
        break;
    case ADVANCED:
        topWords = 1000;
        minLength = 3;
        maxLength = 25;
        seconds = std::chrono::seconds(60);
        capitalFrequency = 0.1;
        break;
    case ENDURANCE:
        topWords = 500;
        minLength = 2;
        maxLength = 25;
        seconds = std::chrono::seconds(300);
        capitalFrequency = 0.05;
        break;
    default:
        break;
    }
    seed = 0;
    personalFrequency = 0;
}
} // namespace typingtest
