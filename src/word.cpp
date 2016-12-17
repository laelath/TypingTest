#include "word.h"

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

bool Word::getEntered()
{
	return entered;
}

bool Word::enterWord(std::string enter)
{
	correct = word == enter;
	entered = true;
	enteredWord = enter;
	return correct;
}

bool Word::getCorrect()
{
	return correct;
}

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
