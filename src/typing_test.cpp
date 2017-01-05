#include "typing_test.h"

#include <fstream>
#include <iostream>

#include <glibmm/main.h>

TestSettings getTestTypeSettings(TestType type)
{
	if (type == ADVANCED) {
		return advanced_test;
	} else if (type == ENDURANCE) {
		return endurance_test;
	}

	return basic_test;
}

TypingTest::TypingTest(const TestWidgets &widgets, const TestSettings &settings)
{
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

	entry->set_text("");
	entry->grab_focus();

	insertConnection = entry->signal_insert_text().connect(sigc::mem_fun(this, &TypingTest::textInsert));
	backspConnection = entry->signal_delete_text().connect(sigc::mem_fun(this, &TypingTest::textDelete));

	timerLabel->set_text(getTime());

	std::ifstream fileIn("words/google-10000-english-usa-no-swears.txt");
	if (!fileIn.is_open()) {
		std::exit(1);
	}

	wordSelection.reserve(settings.topWords);
	for (unsigned long i = 0; i < settings.topWords; ) {
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

	if (settings.seed == 0) {
		rand.seed(std::chrono::system_clock::now().time_since_epoch().count());
	} else {
		rand.seed(settings.seed);
	}

	words.reserve(START_WORDS);
	for (int i = 0; i < START_WORDS; ++i) {
		words.push_back(wordSelection[rand() % wordSelection.size()]);
	}

	textBuffer->set_text(getWords());
	textBuffer->apply_tag_by_name("current", textBuffer->get_iter_at_offset(0),
			textBuffer->get_iter_at_offset(words[0].getWord().length()));
	textBuffer->apply_tag_by_name("uglyhack", textBuffer->get_iter_at_offset(words[0].getWord().length() + 1),
			textBuffer->end());
}

TypingTest::~TypingTest()
{
	insertConnection.disconnect();
	backspConnection.disconnect();
	timerConnection.disconnect();
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

void TypingTest::textInsert(std::string text, int *pos)
{
	if (!testStarted) {
		testStarted = true;
		timerConnection =
			Glib::signal_timeout().connect(sigc::mem_fun(*this, &TypingTest::updateTimer), 1000);
		words[0].startTime();
	}

	if (!testEnded) {
		if (text[0] == ' ') {
			std::string word = entry->get_text().substr(0, *pos - 1);
			newWord = true;
			entry->delete_text(0, *pos);

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

			std::string newWord = wordSelection[rand() % wordSelection.size()];
			words.push_back(Word(newWord));
			textBuffer->insert(textBuffer->end(), " " + newWord);

			textBuffer->apply_tag_by_name("uglyhack",
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length() + 1),
					textBuffer->end());

			Gtk::TextBuffer::iterator itr = textBuffer->get_iter_at_offset(wordCharIndex);
			textView->scroll_to(itr, 0.2);
		} else {
			textBuffer->remove_tag_by_name("current", textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
			textBuffer->remove_tag_by_name("currenterror", textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
			if (!words[wordIndex].getStarted()) {
				words[wordIndex].startTime();
			}
		}

		std::string text = entry->get_text();
		std::string word = words[wordIndex].getWord();
		if (text.length() <= word.length() && text == word.substr(0, text.length())) {
			textBuffer->apply_tag_by_name("current", textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
		} else {
			textBuffer->apply_tag_by_name("currenterror", textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
		}
		//words[wordIndex].startTime();
	}
}

void TypingTest::textDelete(int, int)
{
	if (!testEnded) {
		textBuffer->remove_tag_by_name("current", textBuffer->get_iter_at_offset(wordCharIndex),
				textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
		textBuffer->remove_tag_by_name("currenterror", textBuffer->get_iter_at_offset(wordCharIndex),
				textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));

		if (!newWord) {
			std::string text = entry->get_text();
			std::string word = words[wordIndex].getWord();
			if (text.length() <= word.length() && text == word.substr(0, text.length())) {
				textBuffer->apply_tag_by_name("current", textBuffer->get_iter_at_offset(wordCharIndex),
						textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
			} else {
				textBuffer->apply_tag_by_name("currenterror", textBuffer->get_iter_at_offset(wordCharIndex),
						textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
			}
		} else {
			newWord = false;
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
		//textBuffer->remove_tag_by_name("current", textBuffer->get_iter_at_offset(wordCharIndex),
		//		textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
		//textBuffer->remove_tag_by_name("currenterror", textBuffer->get_iter_at_offset(wordCharIndex),
		//		textBuffer->get_iter_at_offset(wordCharIndex + words[wordIndex].getWord().length()));
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
	std::vector<std::tuple<std::string, int, double>> wordScores;
	for (int i = 0; i < wordIndex; ++i) {
		bool found = false;
		for (std::tuple<std::string, int, double> wordScore : wordScores) {
			if (words[i].getWord() == std::get<0>(wordScore)) {
				std::get<1>(wordScore)++;
				std::get<2>(wordScore) += words[i].getScore();
				found = true;
				break;
			}
		}
		if (!found) {
			wordScores.push_back(std::make_tuple(words[i].getWord(), 1, words[i].getScore()));
		}
	}

	//Calculate averages and total
	double total = 0;
	for (std::tuple<std::string, int, double> wordScore : wordScores) {
		std::get<2>(wordScore) = std::get<2>(wordScore) / std::get<1>(wordScore);
		total += std::get<2>(wordScore);
	}
	double mean = total / wordScores.size();

	//Standard deviation
	double sum = 0;
	for (std::tuple<std::string, int, double> wordScore : wordScores) {
		sum += std::pow(std::get<2>(wordScore) - mean, 2);
	}

	double stdDev = std::sqrt(sum / wordScores.size());
	
	//Sort scores
	std::sort(wordScores.begin(), wordScores.end(),
			[](std::tuple<std::string, int, double> i, std::tuple<std::string, int, double> j) -> bool
			{
				return std::get<2>(i) < std::get<2>(j);
			});

	//Print out scores
	std::string troubleWords = "";
	for (std::tuple<std::string, int, double> wordScore : wordScores) {
		if (mean - 2 * std::get<2>(wordScore) >= stdDev) {
			//std::cout << std::get<0>(wordScore) << std::endl;
			troubleWords += std::get<0>(wordScore) + "\n";
		} else {
			break;
		}
	}

	wpmLabel->set_text("WPM: " + std::to_string((int) ((charsCorrect / 5.0) / (start.count() / 60.0))));
	wordNumLabel->set_text("Words: " + std::to_string(wordNum));
	wordsCorrectLabel->set_text("Correct: " + std::to_string(wordsCorrect));
	wordsWrongLabel->set_text("Wrong: " + std::to_string(wordNum - wordsCorrect));
	charNumLabel->set_text("Characters: " + std::to_string(charNum));
	charsCorrectLabel->set_text("Correct: " + std::to_string(charsCorrect));
	charsWrongLabel->set_text("Wrong: " + std::to_string(charNum - charsCorrect));
	troubleWordsLabel->set_text(troubleWords);
}
