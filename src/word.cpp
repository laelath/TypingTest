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

#include "word.h"

#include <iostream>

#include "config.h"

namespace typingtest {

Word::Word(std::string word)
{
	this->word = word;
}

std::string Word::getWord()
{
	return word;
}

std::string Word::getEntry()
{
	return enteredWord;
}

double Word::getScore()
{
	return score;
}

bool Word::getStarted()
{
	return started;
}

void Word::startTime()
{
	start = std::chrono::high_resolution_clock::now();
	started = true;
}

bool Word::enterWord(std::string enter)
{
	correct = word == enter;
	enteredWord = enter;
	time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);
	//Calculate score
	score = word.length() * 1000 * (correct ? 1 : config.wordWrongWeight)
        / (double) time.count();
	return correct;
}

bool Word::getCorrect()
{
	return correct;
}

std::chrono::milliseconds Word::getTime()
{
	return time;
}
} // namespace typingtest
