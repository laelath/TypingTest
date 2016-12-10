#ifndef TYPING_TEST_H
#define TYPING_TEST_H

#include <chrono>
#include <random>
#include <string>
#include <vector>

#include <gtkmm/entrybuffer.h>
#include <gtkmm/label.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>

#define START_WORDS 400

class TypingTest {
	public:
		TypingTest(Gtk::TextView *textView, Glib::RefPtr<Gtk::EntryBuffer> entryBuffer,
				Gtk::Label *label, size_t topWords, size_t minLength, size_t maxLength,
				std::chrono::seconds seconds, uint32_t seed);
		~TypingTest();

		std::string getWords();
		std::string getTime();
		//void run();
		
		void textInsert(int pos, const char *text, int num);

	private:
		Glib::RefPtr<Gtk::EntryBuffer> entryBuffer;
		Glib::RefPtr<Gtk::TextBuffer> textBuffer;
		Gtk::TextView *textView;
		Gtk::Label *label;
		sigc::connection connection;

		std::minstd_rand rand;

		std::vector<std::string> wordSelection;
		std::vector<std::string> words;
		std::vector<std::string> enteredWords;

		std::chrono::seconds seconds;
		int wordIndex = 0;
		int wordCharIndex = 0;
};

#endif
