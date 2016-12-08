#ifndef TYPING_TEST_H
#define TYPING_TEST_H

#include <string>
#include <vector>

#include <gtkmm/entrybuffer.h>

//max speed in WPS, to determine number of words needed
#define MAX_SPEED 5 //300 WPM

enum TestType {
	BASIC,
	LONG,
	MEDIUM,
	SHORT,
	MIX
};

class TypingTest {
	public:
		TypingTest(Glib::RefPtr<Gtk::EntryBuffer> entryBuffer, TestType type, int topWords, int seconds,
				uint32_t seed);
		~TypingTest();

		std::string getWords();
		std::string getTime();
		//void run();
		
		void textInsert(int pos, const char *text, int num);

	private:
		Glib::RefPtr<Gtk::EntryBuffer> entryBuffer;
		sigc::connection connection;

		std::vector<std::string> words;

		int seconds;
		int wordIndex = 0;
		int charIndex = 0;
};

#endif
