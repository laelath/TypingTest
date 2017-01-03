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

#define START_WORDS 500

enum TestType {
	BASIC,
	ADVANCED,
	ENDURANCE,
	CUSTOM,
};

struct TestWidgets {
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
};

struct TestSettings {
	TestType type;
	size_t topWords;
	size_t minLength;
	size_t maxLength;
	std::chrono::seconds seconds;
	uint32_t seed;

	TestSettings(TestType t, size_t tw, size_t minl, size_t maxl, std::chrono::seconds sec, uint32_t sd) :
		type(t), topWords(tw), minLength(minl), maxLength(maxl), seconds(sec), seed(sd) {}
};

const TestSettings basic_test = { BASIC, 200, 2, 100, std::chrono::seconds(60), 0 };
const TestSettings advanced_test = { ADVANCED, 10000, 3, 100, std::chrono::seconds(60), 0 };
const TestSettings endurance_test = { ENDURANCE, 500, 2, 100, std::chrono::seconds(300), 0 };

TestSettings getTestTypeSettings(TestType type);

class TypingTest {
	public:
		TypingTest(const TestWidgets &widgets, const TestSettings &settings); 
		TypingTest(const TestWidgets &widgets) : TypingTest(widgets, basic_test) {};
		~TypingTest();

		std::string getWords();
		std::string getTime();
		
	private:
		void textInsert(std::string text, int *pos);
		void textDelete(int pos, int num);
		bool updateTimer();
		void calculateScore();

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

		//Glib::RefPtr<Gtk::EntryBuffer> entryBuffer;
		Glib::RefPtr<Gtk::TextBuffer> textBuffer;

		sigc::connection insertConnection;
		sigc::connection backspConnection;
		sigc::connection timerConnection;

		std::minstd_rand rand;

		std::vector<std::string> wordSelection;
		std::vector<Word> words;

		std::chrono::seconds seconds;
		std::chrono::seconds start;
		int wordIndex = 0;
		int wordCharIndex = 0;
		bool testStarted = false;
		bool testEnded = false;
		bool newWord = false;
};

#endif
