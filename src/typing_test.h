#ifndef TYPING_TEST_H
#define TYPING_TEST_H

#include <memory>
#include <string>
#include <vector>

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
		TypingTest(TestType type, int topWords, int seconds, uint32_t seed);
		~TypingTest();

		std::string getWords();
		std::string getTime();
		//void run();

	private:
		std::unique_ptr<std::string> words;
		std::string currentWord;
		int seconds;
		int numWords;
		int index = 0;
		int charIndex = 0;
};

#endif
