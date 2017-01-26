
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

namespace typingtest {

TestSettings::TestSettings()
{
}

TestSettings::TestSettings(TestType type)
{
	switch (type) {
	case ADVANCED:
		*this = ADVANCED_TEST;
		break;
	case ENDURANCE:
		*this = ENDURANCE_TEST;
		break;
	default:
		*this = BASIC_TEST;
		break;
	}
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
} /* namespace typingtest */
