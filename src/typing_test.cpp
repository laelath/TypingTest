#include "typing_test.h"

#include <fstream>
#include <iostream>
#include <random>

TypingTest::TypingTest(Glib::RefPtr<Gtk::TextBuffer> textBuffer, Glib::RefPtr<Gtk::EntryBuffer> entryBuffer,
		Gtk::Label *label, size_t topWords, size_t minLength, size_t maxLength, std::chrono::seconds seconds,
		uint32_t seed)
{
	this->entryBuffer = entryBuffer;
	this->textBuffer = textBuffer;
	this->label = label;
	this->seconds = seconds;

	this->connection =
		entryBuffer->signal_inserted_text().connect(sigc::mem_fun(this, &TypingTest::textInsert));

	std::ifstream fileIn("words/google-10000-english-usa-no-swears.txt");
	if (!fileIn.is_open()) {
		std::exit(1);
	}

	label->set_text(this->getTime());

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
	this->enteredWords.reserve(seconds.count() * MAX_SPEED);
	for (int i = 0; i < seconds.count() * MAX_SPEED; ++i) {
		this->words.push_back(wordSelection[rand() % wordSelection.size()]);
	}

	textBuffer->set_text(this->getWords());
	textBuffer->apply_tag_by_name("current", textBuffer->get_iter_at_offset(0),
			textBuffer->get_iter_at_offset(this->words[0].length()));
}

TypingTest::~TypingTest()
{
	connection.disconnect();
}

std::string TypingTest::getWords()
{
	std::string text = words[0];
	for (unsigned long i = 1; i < words.size(); ++i) {
		text += " " + words[i];
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
			std::string word = entryBuffer->get_text().substr(0, pos);
			if (word == words[wordIndex]) {
				textBuffer->apply_tag_by_name("good",
						textBuffer->get_iter_at_offset(this->wordCharIndex),
						textBuffer->get_iter_at_offset(this->wordCharIndex +
							this->words[this->wordIndex].length()));
			} else {
				this->textBuffer->apply_tag_by_name("error",
						this->textBuffer->get_iter_at_offset(this->wordCharIndex),
						this->textBuffer->get_iter_at_offset(this->wordCharIndex +
							this->words[this->wordIndex].length()));
			}
			this->textBuffer->remove_tag_by_name("current",
					this->textBuffer->get_iter_at_offset(this->wordCharIndex),
					this->textBuffer->get_iter_at_offset(this->wordCharIndex +
						this->words[this->wordIndex].length()));
			this->wordCharIndex += this->words[this->wordIndex].length() + 1;
			this->wordIndex++;
			this->textBuffer->apply_tag_by_name("current",
					this->textBuffer->get_iter_at_offset(this->wordCharIndex),
					this->textBuffer->get_iter_at_offset(this->wordCharIndex +
						this->words[this->wordIndex].length()));
			this->enteredWords.push_back(word);
			this->entryBuffer->delete_text(0, pos + 1);
		}
	}
}
