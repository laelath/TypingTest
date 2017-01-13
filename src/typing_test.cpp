#include "typing_test.h"

#include <fstream>
#include <iostream>

#include <glibmm/main.h>

#include <gtkmm/messagedialog.h>

#include "config.h"

TestType getTypeFromNumber(int num)
{
	switch (num) {
		case 1 : return ADVANCED;
		case 2 : return ENDURANCE;
		case 3 : return CUSTOM;
	}
	return BASIC;
}

int getTypeNumber(TestType type)
{
	switch (type) {
		case BASIC : return 0;
		case ADVANCED : return 1;
		case ENDURANCE : return 2;
		case CUSTOM : return 3;
	}
}

TestSettings getTestTypeSettings(TestType type)
{
	if (type == ADVANCED) {
		return advanced_test;
	} else if (type == ENDURANCE) {
		return endurance_test;
	}

	return basic_test;
}

TypingTest::TypingTest(Gtk::Window *parent, const TestWidgets &widgets, const TestSettings &settings)
{
	this->parent = parent;

	textView = widgets.textView;
	entry = widgets.entry;
	timerLabel = widgets.timer;
	wpmLabel = widgets.wpm;
	wordNumLabel = widgets.wordNum;
	wordsCorrectLabel = widgets.wordsCorrect;
	wordsWrongLabel = widgets.wordsWrong;
	charNumLabel = widgets.charNum;
	charsCorrectLabel = widgets.charsCorrect;
	charsWrongLabel = widgets.charsWrong;
	troubleWordsLabel = widgets.troubleWords;

	textBuffer = textView->get_buffer();

	seconds = settings.seconds;
	start = settings.seconds;

	personalFrequency = settings.personalFrequency;

	entry->set_text("");
	entry->grab_focus();

	insertConnection = entry->signal_insert_text().connect(sigc::mem_fun(this, &TypingTest::textInsert));
	backspConnection = entry->signal_delete_text().connect(sigc::mem_fun(this, &TypingTest::textDelete));

	timerLabel->set_text(getTime());

	std::ifstream fileIn("words/dictionary.txt");
	std::ifstream trWords("words/troublewords.txt");

	if (!fileIn.is_open()) {
		std::exit(1);
	}
	
	if (settings.personalFrequency != 0) {
		if (!trWords.is_open()) {
			Gtk::MessageDialog error(*parent, "No trouble words file found.", false, Gtk::MESSAGE_ERROR,
					Gtk::BUTTONS_CLOSE, true);
			error.set_secondary_text("You need to finish a test before trouble words will be detected.");
			error.run();
			disconnectSignals();
			return;
		} else if (trWords.peek() == std::ifstream::traits_type::eof()) {
			Gtk::MessageDialog error(*parent, "Trouble words file empty.", false, Gtk::MESSAGE_ERROR,
					Gtk::BUTTONS_CLOSE, true);
			error.set_secondary_text("You need to finish a test before trouble words will be detected.");
			error.run();
			disconnectSignals();
			return;
		}
	}

	wordSelection.reserve(settings.topWords);
	for (size_t i = 0; i < settings.topWords; ) {
		std::string line;
		if (std::getline(fileIn, line)) {
			if (line.length() >= settings.minLength && line.length() <= settings.maxLength) {
				wordSelection.push_back(line);
				i++;
			}
		} else {
			break;
		}
	}

	std::string line;
	while (std::getline(trWords, line)) {
		std::string word = line.substr(0, line.find(","));
		if (word.length() >=  settings.minLength && line.length() <= settings.maxLength) {
			int num = std::stoi(line.substr(line.find(",") + 1));
			for (int i = 0; i < num; ++i) {
				personalSelection.push_back(word);
			}
		}
	}

	if (settings.seed == 0) {
		rand.seed(std::chrono::system_clock::now().time_since_epoch().count());
	} else {
		rand.seed(settings.seed);
	}

	words.reserve(config.startWords);
	for (int i = 0; i < config.startWords; ++i) {
		words.push_back(genWord());
	}

	textBuffer->set_text(getWords());
	textBuffer->apply_tag_by_name("current", textBuffer->get_iter_at_offset(0),
			textBuffer->get_iter_at_offset(words[0].getWord().length()));
	textBuffer->apply_tag_by_name("uglyhack", textBuffer->get_iter_at_offset(words[0].getWord().length() + 1),
			textBuffer->end());
}

TypingTest::~TypingTest()
{
	disconnectSignals();
}

void TypingTest::disconnectSignals()
{
	insertConnection.disconnect();
	backspConnection.disconnect();
	timerConnection.disconnect();
}

std::string TypingTest::genWord()
{
	std::string word;
	if (rand() / (double) rand.max() < personalFrequency) {
		word = personalSelection[rand() % personalSelection.size()];
	} else {
		word = wordSelection[rand() % wordSelection.size()];
	}
	return word;
}

std::string TypingTest::getWords()
{
	std::string text = words[0].getWord();
	for (unsigned long i = 1; i < words.size(); ++i) {
		text += " " + words[i].getWord();
	}
	return text;
}

std::string TypingTest::getTime()
{
	std::string secstr = (seconds.count() % 60 < 10 ?
			"0" + std::to_string(seconds.count() % 60) : std::to_string(seconds.count() % 60));
	return std::to_string(seconds.count() / 60) + ":" + secstr;
}

void TypingTest::textInsert(std::string text, int *)
{
	if (!testStarted) {
		testStarted = true;
		timerConnection =
			Glib::signal_timeout().connect(sigc::mem_fun(*this, &TypingTest::updateTimer), 1000);
		words[0].startTime();
	}

	if (!testEnded) {
		if (text[0] == ' ') {
			std::string word = entry->get_text();
			word.erase(std::remove(word.begin(), word.end(), ' '), word.end());
			entry->set_text("");

			textBuffer->remove_tag_by_name("current", textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
			textBuffer->remove_tag_by_name("currenterror", textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));

			if (words[wordIndex].enterWord(word)) {
				textBuffer->apply_tag_by_name("good", textBuffer->get_iter_at_offset(wordCharIndex),
						textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
			} else {
				textBuffer->apply_tag_by_name("error", textBuffer->get_iter_at_offset(wordCharIndex),
						textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
			}

			wordCharIndex += words[wordIndex].getWord().length() + 1;
			wordIndex++;

			textBuffer->remove_tag_by_name("uglyhack", textBuffer->get_iter_at_offset(0), textBuffer->end());

			std::string newWord = genWord();
			words.push_back(newWord);
			textBuffer->insert(textBuffer->end(), " " + newWord);

			textBuffer->apply_tag_by_name("uglyhack",
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length() + 1),
					textBuffer->end());

			Gtk::TextBuffer::iterator itr = textBuffer->get_iter_at_offset(wordCharIndex);
			textView->scroll_to(itr, 0.2);
		} else {
			if (!words[wordIndex].getStarted()) {
				words[wordIndex].startTime();
			}
		}

		std::string text = entry->get_text();
		std::string word = words[wordIndex].getWord();
		if (text.length() <= word.length() && text == word.substr(0, text.length())) {
			textBuffer->remove_tag_by_name("currenterror", textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
			textBuffer->apply_tag_by_name("current", textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
		} else {
			textBuffer->remove_tag_by_name("current", textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
			textBuffer->apply_tag_by_name("currenterror", textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
		}
	}
}

void TypingTest::textDelete(int, int)
{
	if (!testEnded) {
		std::string text = entry->get_text();
		std::string word = words[wordIndex].getWord();
		if (text.length() <= word.length() && text == word.substr(0, text.length())) {
			textBuffer->remove_tag_by_name("currenterror", textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
			textBuffer->apply_tag_by_name("current", textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
		} else {
			textBuffer->remove_tag_by_name("current", textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
			textBuffer->apply_tag_by_name("currenterror", textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
		}
	}
}

bool TypingTest::updateTimer()
{
	seconds--;
	timerLabel->set_text(getTime());
	if (seconds != std::chrono::seconds::duration::zero()) {
		return true;
	} else {
		textBuffer->erase(textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()),
				textBuffer->end());
		calculateScore();
		testEnded = true;
		return false;
	}
}

void TypingTest::calculateScore()
{
	//Test information
	int wordNum = 0;
	int wordsCorrect = 0;
	int charNum = 0;
	int charsCorrect = 0;
	for (int i = 0; i < wordIndex; ++i) {
		wordNum++;
		charNum += words[i].getWord().length() + 1;
		charsCorrect++;
		if (words[i].getCorrect()) {
			wordsCorrect++;
			charsCorrect += words[i].getWord().length();
		}
	}

	//Trouble words
	std::vector<std::tuple<std::string, double>> wordScores;
	for (int i = 0; i < wordIndex; ++i) {
		bool found = false;
		for (std::tuple<std::string, double> wordScore : wordScores) {
			if (words[i].getWord() == std::get<0>(wordScore)) {
				std::get<1>(wordScore) = std::min(std::get<1>(wordScore), words[i].getScore());
				found = true;
				break;
			}
		}
		if (!found) {
			wordScores.push_back(std::make_tuple(words[i].getWord(), words[i].getScore()));
		}
	}

	//Calculate averages and total
	double total = 0;
	for (std::tuple<std::string, double> wordScore : wordScores) {
		total += std::get<1>(wordScore);
	}
	double mean = total / wordScores.size();

	//Standard deviation
	double sum = 0;
	for (std::tuple<std::string, double> wordScore : wordScores) {
		sum += std::pow(std::get<1>(wordScore) - mean, 2);
	}

	double stdDev = std::sqrt(sum / wordScores.size());
	
	//Sort scores
	std::sort(wordScores.begin(), wordScores.end(),
			[](std::tuple<std::string, double> i, std::tuple<std::string, double> j) -> bool
			{
				return std::get<1>(i) < std::get<1>(j);
			});

	std::vector<std::string> troubleWords;
	std::vector<std::string> goodWords;

	std::string troubleWordsStr;

	//Print out scores
	for (std::tuple<std::string, double> wordScore : wordScores) {
		if (std::get<1>(wordScore) - mean <= config.minZScore * stdDev) {
			troubleWords.push_back(std::get<0>(wordScore));
			troubleWordsStr += std::get<0>(wordScore) + "\n";
		} else if (std::get<1>(wordScore) - mean > config.maxZScore * stdDev) {
			goodWords.push_back(std::get<0>(wordScore));
		}
	}

	std::ifstream file("words/troublewords.txt");
	std::ofstream temp("words/.troublewords.txt.swp", std::ios::trunc);

	if (!temp.is_open()) {
		std::exit(1);
	}

	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			std::string word = line.substr(0, line.find(","));
			int num = std::stoi(line.substr(line.find(",") + 1));

			std::vector<std::string>::iterator it = std::find(troubleWords.begin(), troubleWords.end(), word);
			if (it != troubleWords.end()) {
				temp << word << "," << num + config.troubleInc << "\n";
				troubleWords.erase(it);
			} else {
				it = std::find(goodWords.begin(), goodWords.end(), word);
				if (it != goodWords.end()) {
					if (num > 1) {
						temp << word << "," << num - config.troubleDec << "\n";
					}
					goodWords.erase(it);
				} else {
					temp << line << "\n";
				}
			}
		}
	}

	for (std::string word : troubleWords) {
		temp << word << "," << config.startTroubleScore << "\n";
	}

	file.close();
	temp.close();

	std::remove("words/troublewords.txt");
	std::rename("words/.troublewords.txt.swp", "words/troublewords.txt");

	wpmLabel->set_text("WPM: " + std::to_string((int) ((charsCorrect / 5.0) / (start.count() / 60.0))));
	wordNumLabel->set_text("Words: " + std::to_string(wordNum));
	wordsCorrectLabel->set_text("Correct: " + std::to_string(wordsCorrect));
	wordsWrongLabel->set_text("Wrong: " + std::to_string(wordNum - wordsCorrect));
	charNumLabel->set_text("Characters: " + std::to_string(charNum));
	charsCorrectLabel->set_text("Correct: " + std::to_string(charsCorrect));
	charsWrongLabel->set_text("Wrong: " + std::to_string(charNum - charsCorrect));
	troubleWordsLabel->set_text(troubleWordsStr);
}
