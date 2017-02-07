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

#include "typing_test.h"

#include <fstream>
#include <map>

#include <glibmm/main.h>
#include <giomm/resource.h>
#include <giomm/datainputstream.h>
#include <gtkmm/messagedialog.h>

#include "config.h"

namespace typingtest {

TypingTest::TypingTest(Gtk::Window *parent, const TestSettings &settings,
	const Config &config)
	: settings(settings), config(config), parent(parent)
{
	personalFrequency = settings.personalFrequency;

	gsize size;
	auto data = Gio::Resource::lookup_data_global(
		"/us/laelath/typingtest/dictionary.txt");
	std::string resourceString(static_cast<const char *>(data->get_data(size)));
	std::istringstream dictStream(resourceString);

	try {
		wordSelection.reserve(settings.topWords);
	} catch (std::bad_alloc) {
		// Ignore the error, it's not a necessary call. This is in a try-catch
		// block because when testing on GCC this call produced a bad_alloc
		// exception. Clang did not have this issue.
	}
	for (size_t i = 0; i < settings.topWords; ) {
		std::string line;
		if (std::getline(dictStream, line)) {
			if (line.length() >= settings.minLength && line.length()
				<= settings.maxLength) {
				wordSelection.push_back(line);
				++i;
			}
		} else
			break;
	}

	if (settings.personalFrequency != 0) {
		std::ifstream trWords(config.dataDir + "troublewords.txt");
		if (!trWords.is_open()) {
			if (parent != nullptr) {
				Gtk::MessageDialog error(*parent,
					"No trouble words file found.", false, Gtk::MESSAGE_ERROR,
					Gtk::BUTTONS_CLOSE, true);
				error.set_secondary_text(
					"You need to finish a test before trouble words will be detected.");
				error.run();
			}
			return;
		}

		std::string line;
		while (std::getline(trWords, line)) {
			std::string word = line.substr(0, line.find(","));
			if (word.length() >=  settings.minLength && line.length() <= settings.maxLength) {
				int num = std::stoi(line.substr(line.find(",") + 1));
				for (int i = 0; i < num; ++i)
					personalSelection.push_back(word);
			}
		}

		if (personalSelection.size() == 0) {
			if (parent != nullptr) {
				Gtk::MessageDialog error(*parent, "Trouble words file empty.",
					false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE, true);
				error.set_secondary_text(
					"You need to finish a test before trouble words will be detected.");
				error.run();
			}
			return;
		}
	}

	if (settings.seed == 0)
		rand.seed(std::chrono::system_clock::now().time_since_epoch().count());
	else
		rand.seed(settings.seed);

	try {
		words.reserve(config.startWords);
	} catch (std::bad_alloc) {
		// Ignore the error, it's not a necessary call. This is in a try-catch
		// block because when testing on GCC this call produced a bad_alloc
		// exception. Clang did not have this issue.
	}
	for (int i = 0; i < config.startWords; ++i)
		words.push_back(std::shared_ptr<Word>(new Word(genWord())));
}

TypingTest::TypingTest() : TypingTest(nullptr, TestSettings(), Config())
{
}

std::string TypingTest::genWord()
{
	std::string word;
	if (rand() / (double) rand.max() < personalFrequency)
		word = personalSelection[rand() % personalSelection.size()];
	else
		word = wordSelection[rand() % wordSelection.size()];
	return word;
}

std::string TypingTest::getWordsAsString()
{
	std::string text = words[0]->getWord();
	for (unsigned long i = 1; i < words.size(); ++i)
		text += " " + words[i]->getWord();
	return text;
}

const std::vector<std::shared_ptr<Word>> &TypingTest::getWords() const
{
	return words;
}

const TestSettings &TypingTest::getSettings() const
{
	return settings;
}

void TypingTest::setSettings(const TestSettings &settings)
{
	this->settings = settings;
}

const Config &TypingTest::getConfig() const
{
	return config;
}

void TypingTest::setConfig(const Config &config)
{
	this->config = config;
}

const std::vector<std::string> &TypingTest::getWordSelection() const
{
	return wordSelection;
}

const std::vector<std::string> &TypingTest::getPersonalSelection() const
{
	return personalSelection;
}
} // namespace typingtest
