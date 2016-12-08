#include "typing_test.h"

#include <array>
#include <fstream>
#include <iostream>
#include <random>

TypingTest::TypingTest(Glib::RefPtr<Gtk::EntryBuffer> entryBuffer, TestType type, int topWords, int seconds,
		uint32_t seed)
{
	this->words.reserve(seconds * MAX_SPEED);
	this->entryBuffer = entryBuffer;
	this->seconds = seconds;

	this->connection =
		entryBuffer->signal_inserted_text().connect(sigc::mem_fun(this, &TypingTest::textInsert));

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

	for (int i = 0; i < seconds * MAX_SPEED; ++i) {
		this->words.push_back(wordSelection[rand() % topWords]);
	}
}

TypingTest::~TypingTest()
{
	this->connection.disconnect();
}

std::string TypingTest::getWords()
{
	std::string text = this->words[0];
	for (int i = 1; i < (int) this->words.size(); ++i) {
		text += " " + this->words[i];
	}
	return text;
}

std::string TypingTest::getTime()
{
	std::string secstr = (seconds % 60 < 10 ?
			"0" + std::to_string(seconds % 60) : std::to_string(seconds % 60));
	return std::to_string(seconds / 60) + ":" + secstr;
}

void TypingTest::textInsert(int pos, const char *text, int num)
{
	if (num == 1) {
		if (text[0] == ' ') {
			std::cout << "word submitted: " << this->entryBuffer->get_text().substr(0, pos) << std::endl;
			this->entryBuffer->delete_text(0, pos + 1);
		}
	}
}
