#include "word.h"

#include <iostream>

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

void Word::startTime()
{
	start = std::chrono::high_resolution_clock::now();
}

bool Word::enterWord(std::string enter)
{
	correct = word == enter;
	//entered = true;
	enteredWord = enter;
	time = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::high_resolution_clock::now() - start);
	//Calculate score
	score = (word.length() + WORD_SCORE_WEIGHT) * (correct ? 1 : WORD_WRONG_WEIGHT) / (double) time.count();
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
