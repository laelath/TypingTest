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

#define START_WORDS 350

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

class TypingTest {
	public:
		TypingTest(const TestWidgets &widgets, size_t topWords, size_t minLength, size_t maxLength,
				std::chrono::seconds seconds, uint32_t seed);
		~TypingTest();

		std::string getWords();
		std::string getTime();
		
	private:
		void textInsert(int pos, const char *text, int num);
		void textDelete(int pos, int num);
		bool updateTimer();
		void calculateScore();

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

		Glib::RefPtr<Gtk::EntryBuffer> entryBuffer;
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
