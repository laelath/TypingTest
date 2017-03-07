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

#ifndef TYPING_TEST_TYPING_TEST_H
#define TYPING_TEST_TYPING_TEST_H

#include <chrono>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include <gtkmm.h>

#include "config.h"
#include "test_settings.h"
#include "word.h"

namespace typingtest {

// Represents a class that contains the words for a typing test. It is meant to
// be create and have its words retrieved with getWords().
class TypingTest {
public:
	// Constructor that uses the given settings and configuration. The parent
	// window is used to create transient dialogs.
	TypingTest(Gtk::Window *parent, const TestSettings &settings,
		const Config &config);
	// No-argument constructor.
	TypingTest();

	// Generates a word from the personal selection based on the personal
	// frequency.
	//
	// Returns a newly generated word.
	std::string genWord();
	// Returns words as a long string with each word separated by spaces.
	std::string getWordsAsString();

	const std::vector<std::shared_ptr<Word>> &getWords() const;
	const std::vector<std::string> &getWordSelection() const;
	const std::vector<std::string> &getPersonalSelection() const;

	const TestSettings &getSettings() const;
	void setSettings(const TestSettings &settings);
	const Config &getConfig() const;
	void setConfig(const Config &config);

private:
	// Settings and config for behavior.
	TestSettings settings;
	Config config;

	// Parent widget for creating transient dialogs.
	Gtk::Window *parent;

	// Random generator.
	std::default_random_engine rand;

	// Selection of words to choose from.
	std::vector<std::string> wordSelection;
	// The user's personal selection of words.
	std::vector<std::string> personalSelection;
	// The list of words generated.
	std::vector<std::shared_ptr<Word>> words;

	// The frequency for the user specifically.
	double personalFrequency;
	double capitalFrequency;
};
} // namespace typingtest

#endif // TYPING_TEST_TYPING_TEST_H
