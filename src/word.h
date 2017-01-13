#ifndef WORD_H
#define WORD_H

#include <chrono>
#include <string>

class Word {
	public:
		Word(std::string word);

		std::string getWord();
		std::string getEntry();

		//bool getEntered();
		double getScore();
		bool getStarted();

		void startTime();
		bool enterWord(std::string enter);
		bool getCorrect();
		//int charsCorrect();
		std::chrono::milliseconds getTime();

	private:
		bool correct = false;
		double score;
		//bool entered = false;
		std::string word;
		std::string enteredWord;
		std::chrono::high_resolution_clock::time_point start;
		std::chrono::milliseconds time;
		bool started = false;
};

#endif
