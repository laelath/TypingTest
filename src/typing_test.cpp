#include "typing_test.h"

#include <fstream>
#include <iostream>
#include <random>

TypingTest::TypingTest(Gtk::TextView *textView, Glib::RefPtr<Gtk::EntryBuffer> entryBuffer,
		Gtk::Label *label, size_t topWords, size_t minLength, size_t maxLength, std::chrono::seconds seconds,
		uint32_t seed)
{
	this->entryBuffer = entryBuffer;
	this->textView = textView;
	this->textBuffer = textView->get_buffer();
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
			entryBuffer->delete_text(0, pos + 1);
			enteredWords.push_back(word);

			if (word == words[wordIndex]) {
				textBuffer->apply_tag_by_name("good",
						textBuffer->get_iter_at_offset(wordCharIndex),
						textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].length()));
			} else {
				textBuffer->apply_tag_by_name("error",
						textBuffer->get_iter_at_offset(wordCharIndex),
						textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].length()));
			}

			textBuffer->remove_tag_by_name("current",
					textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].length()));

			wordCharIndex += words[wordIndex].length() + 1;
			wordIndex++;

			textBuffer->apply_tag_by_name("current",
					textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].length()));

			Gtk::TextBuffer::iterator itr = textBuffer->get_iter_at_offset(wordCharIndex);
			textView->scroll_to(itr, 0.25);
		}
	}
}
