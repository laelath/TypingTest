#ifndef WORD_H
#define WORD_H

#include <chrono>
#include <string>

class Word {
	public:
		Word(std::string word);

		std::string getWord();
		std::string getEntry();

		bool getEntered();

		bool enterWord(std::string enter);
		bool getCorrect();
		int charsCorrect();

	private:
		bool correct = false;
		bool entered = false;
		std::string word;
		std::string enteredWord;
		std::chrono::milliseconds time;
};

#endif
