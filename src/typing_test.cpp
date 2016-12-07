#include "typing_test.h"

#include <array>
#include <fstream>
#include <iostream>
#include <random>

TypingTest::TypingTest(TestType type, int topWords, int seconds, uint32_t seed) :
	words(new std::string[seconds * MAX_SPEED])
{
	this->seconds = seconds;
	this->numWords = seconds * MAX_SPEED;

	if (type != TestType::BASIC) {
		std::exit(1);
	}

	std::ifstream fileIn("words/google-10000-english-usa-no-swears.txt");
	if (!fileIn.is_open()) {
		std::exit(1);
	}

	std::string wordSelection[topWords];
	for (int i = 0; i < topWords && std::getline(fileIn, wordSelection[i]); ++i);

	std::minstd_rand rand;
	rand.seed(seed);

	for (int i = 0; i < this->numWords; ++i) {
		this->words.get()[i] = wordSelection[rand() % topWords];
	}
}

TypingTest::~TypingTest()
{
}

std::string TypingTest::getWords()
{
	std::string text = this->words.get()[0];
	for (int i = 1; i < this->numWords; ++i) {
		text += " " + this->words.get()[i];
	}
	return text;
}

std::string TypingTest::getTime()
{
	std::string secstr = (seconds % 60 < 10 ? "0" + std::to_string(seconds % 60) : std::to_string(seconds % 60));
	return std::to_string(seconds / 60) + ":" + secstr;
}

void TypingTest::textInsert(int pos, const char *text, int num)
{
	if (text[0] == ' ') {
		std::cout << this->currentWord << std::endl;
		this->currentWord = "";
	} else {
		this->currentWord.insert(pos, text, num);
	}
}

void TypingTest::textDelete(int pos, int num)
{
	if (!this->currentWord.empty()) {
		this->currentWord.erase(pos, num);
	}
}
