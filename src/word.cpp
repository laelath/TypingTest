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

#include <algorithm>
#include <iostream>
#include <vector>

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

unsigned int levenshtein_distance(const std::string& s1, const std::string& s2)
{
	const std::size_t len1 = s1.size(), len2 = s2.size();
	std::vector<unsigned int> col(len2 + 1), prevCol(len2 + 1);

	for (unsigned int i = 0; i < prevCol.size(); i++)
		prevCol[i] = i;
	for (unsigned int i = 0; i < len1; i++) {
		col[0] = i + 1;
		for (unsigned int j = 0; j < len2; j++)
			col[j + 1] = std::min({ prevCol[1 + j] + 1, col[j] + 1,
					prevCol[j] + (s1[i] == s2[j] ? 0 : 1) });
		col.swap(prevCol);
	}
	return prevCol[len2];
}

bool Word::enterWord(std::string enter)
{
	correct = word == enter;
	enteredWord = enter;
	time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);
	//Calculate score
	score = word.length() / (double) (time.count() *
			(levenshtein_distance(word, enteredWord) + 1));
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
