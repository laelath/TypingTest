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

#ifndef TYPING_TEST_H
#define TYPING_TEST_H

#include <chrono>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include <gtkmm/entry.h>
#include <gtkmm/entrybuffer.h>
#include <gtkmm/label.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>

#include "test_settings.h"
#include "word.h"

namespace typingtest {

class TypingTest {
public:
	TypingTest(Gtk::Window *parent, const TestSettings &settings);
	TypingTest(Gtk::Window *parent);

private:
	std::string genWord();
	std::string getWordsAsString();
	std::string getTime();
	bool updateTimer();
	void calculateScore();

	Gtk::Window *parent;

	std::minstd_rand rand;

	std::vector<std::string> wordSelection;
	std::vector<std::string> personalSelection;
	std::vector<std::shared_ptr<Word>> words;

	double personalFrequency;

	std::chrono::seconds seconds;
	std::chrono::seconds start;

	int wordIndex = 0;
	int wordCharIndex = 0;
	bool testStarted = false;
	bool testEnded = false;
};
} // namespace typingtest

#endif // TYPING_TEST_H
