#ifndef TYPING_TEST_H
#define TYPING_TEST_H

#include <chrono>
#include <string>
#include <vector>

#include <gtkmm/entrybuffer.h>
#include <gtkmm/label.h>
#include <gtkmm/textbuffer.h>

//max speed in WPS, to determine number of words needed
#define MAX_SPEED 5 //300 WPM

class TypingTest {
	public:
		TypingTest(Glib::RefPtr<Gtk::TextBuffer> textBuffer, Glib::RefPtr<Gtk::EntryBuffer> entryBuffer,
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
		Gtk::Label *label;
		sigc::connection connection;

		std::vector<std::string> words;
		std::vector<std::string> enteredWords;

		std::chrono::seconds seconds;
		int wordIndex = 0;
		int wordCharIndex = 0;
};

#endif
