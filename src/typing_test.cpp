#include "typing_test.h"

#include <array>
#include <fstream>
#include <iostream>
#include <random>

TypingTest::TypingTest(TestType type, int topWords, int seconds, uint32_t seed)
{
	this->seconds = seconds;
	
	this->numWords = seconds * MAX_SPEED;
	//words.reserve(numWords);
	this->words = new std::string[numWords];

	if (type != TestType::BASIC) {
		std::exit(1);
	}

	std::ifstream fileIn("words/google-10000-english-usa-no-swears.txt");
	if (!fileIn.is_open()) {
		std::exit(1);
	}

	//std::vector<std::string> wordSelection (topWords);
	std::string wordSelection[topWords];

	for (int i = 0; i < topWords; ++i) {
		std::string line;
		if (std::getline(fileIn, line)) {
			wordSelection[i] = line;
		} else {
			std::exit(1);
		}
	}

	std::minstd_rand rand;
	rand.seed(seed);

	for (int i = 0; i < this->numWords; ++i) {
		this->words[i] = wordSelection[rand() % topWords];
	}
}

TypingTest::~TypingTest()
{
	delete words;
}

std::string TypingTest::getWords()
{
	std::string text = this->words[0];
	for (int i = 1; i < this->numWords; ++i) {
		text += " " + this->words[i];
	}
	return text;
}
