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
#include <random>
#include <string>
#include <vector>

#include <gtkmm/entry.h>
#include <gtkmm/entrybuffer.h>
#include <gtkmm/label.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>

#include "word.h"

namespace typingtest {

enum TestType {
	BASIC,
	ADVANCED,
	ENDURANCE,
	CUSTOM,
};

struct TestWidgets {
	Gtk::Window *parent;
	Gtk::TextView *textView;
	Gtk::Entry *entry;
	Gtk::Label *timer;
	Gtk::Label *wpm;
	Gtk::Label *wordNum;
	Gtk::Label *wordsCorrect;
	Gtk::Label *wordsWrong;
	Gtk::Label *charNum;
	Gtk::Label *charsCorrect;
	Gtk::Label *charsWrong;
	Gtk::Label *troubleWords;
};

struct TestSettings {
	TestType type;
	size_t topWords;
	size_t minLength;
	size_t maxLength;
	std::chrono::seconds seconds;
	uint32_t seed;
	double personalFrequency;

	TestSettings(TestType t, size_t tw, size_t minl, size_t maxl, std::chrono::seconds sec, uint32_t sd,
			double pf) :
		type(t), topWords(tw), minLength(minl), maxLength(maxl), seconds(sec), seed(sd), personalFrequency(pf)
	{}
};

const TestSettings basic_test = { BASIC, 200, 2, 100, std::chrono::seconds(60), 0, 0 };
const TestSettings advanced_test = { ADVANCED, 10000, 3, 100, std::chrono::seconds(60), 0, 0 };
const TestSettings endurance_test = { ENDURANCE, 500, 2, 100, std::chrono::seconds(300), 0, 0 };

TestType getTypeFromNumber(int num);
int getTypeNumber(TestType type);
TestSettings getTestTypeSettings(TestType type);

class TypingTest {
	public:
		TypingTest(Gtk::Window *parent, const TestWidgets &widgets, const TestSettings &settings); 
		TypingTest(Gtk::Window *parent, const TestWidgets &widgets) :
			TypingTest(parent, widgets, basic_test) {};
		~TypingTest();
		
	private:
		void disconnectSignals();
		std::string genWord();
		std::string getWords();
		std::string getTime();
		void textInsert(std::string text, int *pos);
		void textDelete(int pos, int num);
		bool updateTimer();
		void calculateScore();

		Gtk::Window *parent;
		Gtk::TextView *textView;
		Gtk::Entry *entry;
		Gtk::Label *timerLabel;
		Gtk::Label *wpmLabel;
		Gtk::Label *wordNumLabel;
		Gtk::Label *wordsCorrectLabel;
		Gtk::Label *wordsWrongLabel;
		Gtk::Label *charNumLabel;
		Gtk::Label *charsCorrectLabel;
		Gtk::Label *charsWrongLabel;
		Gtk::Label *troubleWordsLabel;

		//Glib::RefPtr<Gtk::EntryBuffer> entryBuffer;
		Glib::RefPtr<Gtk::TextBuffer> textBuffer;

		sigc::connection insertConnection;
		sigc::connection backspConnection;
		sigc::connection timerConnection;

		std::minstd_rand rand;

		std::vector<std::string> wordSelection;
		std::vector<std::string> personalSelection;
		std::vector<Word> words;

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
