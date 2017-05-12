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

#ifndef TYPING_TEST_CONFIG_H
#define TYPING_TEST_CONFIG_H

#include <string>

namespace typingtest {

enum HighlightMode {
    ALL = 0,
    SOME,
    NONE,
};

HighlightMode getHighlight(std::string str);
std::string getString(HighlightMode hl);

class Config {
public:
    Config();

    int startWords = 500;
    double minZScore = -2.2;
    double maxZScore = 0.0;
    int startTroubleScore = 3;
    int troubleDec = 1;
    int troubleInc = 1;
    HighlightMode hlMode = HighlightMode::ALL;
    std::string configDir;
    std::string dataDir;

    void setPaths();
    void loadConfig();
    void saveConfig();
};
} // namespace typingtest

#endif // TYPING_TEST_CONFIG_H
