#ifndef TYPING_TEST_H
#define TYPING_TEST_H

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
		//void run();

	private:
		std::string *words;
		//std::vector<std::string> words;
		int seconds;
		int numWords;
};

#endif
