#include "word.h"

#include <iostream>

#include "config.h"

Word::Word(std::string word)
{
	this->word = word;
}

std::string Word::getWord()
{
	return word;
}

std::string Word::getEntry()
{
	return enteredWord;
}

/*
bool Word::getEntered()
{
	return entered;
}
*/

double Word::getScore()
{
	return score;
}

bool Word::getStarted()
{
	return started;
}

void Word::startTime()
{
	start = std::chrono::high_resolution_clock::now();
	started = true;
}

bool Word::enterWord(std::string enter)
{
	correct = word == enter;
	enteredWord = enter;
	time = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::high_resolution_clock::now() - start);
	//Calculate score
	score = word.length() * 1000 * (correct ? 1 : config.wordWrongWeight) / (double) time.count();
	return correct;
}

bool Word::getCorrect()
{
	return correct;
}

/*
int Word::charsCorrect()
{
	if (correct) {
		return word.size();
	}
	return 0;

	//int chars = 0;
	//for (unsigned long i = 0; i < word.size() && i < enteredWord.size(); ++i) {
	//	if (word[i] == enteredWord[i]) {
	//		chars++;
	//	}
	//}
	//return chars;
}
*/

std::chrono::milliseconds Word::getTime()
{
	return time;
}
