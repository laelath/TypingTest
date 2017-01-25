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

#ifndef WORD_H
#define WORD_H

#include <chrono>
#include <string>

namespace typingtest {

class Word {
public:
	Word(std::string word);

	std::string getWord();
	std::string getEntry();

	double getScore();
	bool getStarted();

	void startTime();
	bool enterWord(std::string enter);
	bool getCorrect();
	std::chrono::milliseconds getTime();

private:
	bool correct = false;
	double score;
	std::string word;
	std::string enteredWord;
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::milliseconds time;
	bool started = false;
};
} // namespace typingtest

#endif // WORD_H
