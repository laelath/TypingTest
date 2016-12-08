#include "typing_test.h"

#include <fstream>
#include <iostream>
#include <random>

TypingTest::TypingTest(Glib::RefPtr<Gtk::TextBuffer> textBuffer, Glib::RefPtr<Gtk::EntryBuffer> entryBuffer,
		size_t topWords, size_t minLength, size_t maxLength, std::chrono::seconds seconds, uint32_t seed)
{
	this->entryBuffer = entryBuffer;
	this->seconds = seconds;

	this->connection =
		entryBuffer->signal_inserted_text().connect(sigc::mem_fun(this, &TypingTest::textInsert));

	std::ifstream fileIn("words/google-10000-english-usa-no-swears.txt");
	if (!fileIn.is_open()) {
		std::exit(1);
	}

	std::vector<std::string> wordSelection;
	wordSelection.reserve(topWords);
	for (unsigned long i = 0; i < topWords; ) {
		std::string line;
		if (std::getline(fileIn, line)) {
			if (line.length() >= minLength && line.length() <= maxLength) {
				wordSelection.push_back(line);
				i++;
			}
		} else {
			break;
		}
	}

	std::minstd_rand rand;
	rand.seed(seed);

	this->words.reserve(seconds.count() * MAX_SPEED);
	for (int i = 0; i < seconds.count() * MAX_SPEED; ++i) {
		this->words.push_back(wordSelection[rand() % wordSelection.size()]);
	}

	textBuffer->set_text(this->getWords());
}

TypingTest::~TypingTest()
{
	this->connection.disconnect();
}

std::string TypingTest::getWords()
{
	std::string text = this->words[0];
	for (unsigned long i = 1; i < this->words.size(); ++i) {
		text += " " + this->words[i];
	}
	return text;
}

std::string TypingTest::getTime()
{
	std::string secstr = (seconds.count() % 60 < 10 ?
			"0" + std::to_string(seconds.count() % 60) : std::to_string(seconds.count() % 60));
	return std::to_string(seconds.count() / 60) + ":" + secstr;
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
