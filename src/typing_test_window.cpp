// Copyright (C) 2017 Justin Frank, Jason Waataja
//
// This file is part of TypingTest.
//
// TypingTest is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// TypingTest is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// TypingTest.  If not, see <http://www.gnu.org/licenses/>.

#include "typing_test_window.h"

#include <err.h>
#include <stdlib.h>

#include <fstream>

namespace typingtest {

TypingTestWindow::TypingTestWindow(BaseObjectType *cobject,
	const Glib::RefPtr<Gtk::Builder>& builder)
	: Gtk::ApplicationWindow(cobject), builder(builder)
{
	initWidgets();
	connectSignals();

	config.loadConfig();
	genNewTest();

	insertConnection = typingEntry->signal_insert_text().connect(sigc::mem_fun(
			this, &TypingTestWindow::textInsert));
	backspConnection = typingEntry->signal_delete_text().connect(sigc::mem_fun(
			this, &TypingTestWindow::textDelete));


}

void TypingTestWindow::initWidgets()
{
	builder->get_widget("newtestbutton", newTest);
	builder->get_widget("settings", settingsItem);
	builder->get_widget("font", fontItem);
	builder->get_widget("advanced", advItem);
	builder->get_widget("quit", quitItem);
	builder->get_widget("viewtroublewords", troubleItem);
	builder->get_widget("about", aboutItem);
	builder->get_widget("testbox", testBox);

	builder->get_widget("textview", textView);
	builder->get_widget("typingentry", typingEntry);
	builder->get_widget("timerlabel", timerLabel);
	builder->get_widget("wpmlabel", wpmLabel);
	builder->get_widget("wordslabel", wordNumLabel);
	builder->get_widget("correctwordslabel", wordsCorrectLabel);
	builder->get_widget("wrongwordslabel", wordsWrongLabel);
	builder->get_widget("characterslabel", charNumLabel);
	builder->get_widget("correctcharlabel", charsCorrectLabel);
	builder->get_widget("wrongcharlabel", charsWrongLabel);
	builder->get_widget("troublewordslabel", troubleWordsLabel);

	testBox->override_font(Pango::FontDescription("15"));
	textView->override_font(Pango::FontDescription(currFont));

	textBuffer = Glib::RefPtr<Gtk::TextBuffer>::cast_static(builder->get_object(
			"textbuffer"));
	textTags = Glib::RefPtr<Gtk::TextTagTable>::cast_static(builder->get_object(
			"texttags"));
	currentTag = Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object(
			"currenttag"));
	currentErrorTag = Glib::RefPtr<Gtk::TextTag>::cast_static(
		builder->get_object("currenterrortag"));
	errorTag = Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object(
			"errortag"));
	goodTag = Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object(
			"goodtag"));
	uglyHackTag = Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object(
			"uglyhacktag"));

	textTags->add(currentTag);
	textTags->add(currentErrorTag);
	textTags->add(errorTag);
	textTags->add(goodTag);
	textTags->add(uglyHackTag);

	// Prepare settings window
	builder->get_widget("settingsdialog", settingsDialog);
	builder->get_widget("cancelbutton", cancel);
	builder->get_widget("applybutton", apply);
	builder->get_widget("testtypebox", testTypeBox);
	builder->get_widget("topwordsentry", topWords);
	builder->get_widget("minlengthentry", minWordLength);
	builder->get_widget("maxlengthentry", maxWordLength);
	builder->get_widget("testlengthentry", testLength);
	builder->get_widget("seedentry", seedEntry);
	builder->get_widget("randomizeseed", randomizeSeed);
	builder->get_widget("personalratioentry", personalFrequencyButton);

	// Advanced settings window
	builder->get_widget("advancedsettingsdialog", advSettingsDialog);
	builder->get_widget("startwords", startWords);
	builder->get_widget("minzscore", minZScore);
	builder->get_widget("maxzscore", maxZScore);
	builder->get_widget("starttrouble", startTroubleScore);
	builder->get_widget("troubledec", troubleDec);
	builder->get_widget("troubleinc", troubleInc);
	builder->get_widget("wordwrongmult", wordWrongMult);
	builder->get_widget("restoredefaultadv", restoreDefaultAdv);
	builder->get_widget("canceladv", cancelAdv);
	builder->get_widget("applyadv", applyAdv);

	// Trouble words viewer
	builder->get_widget("troubledialog", troubleDialog);
	builder->get_widget("troublelist", troubleList);
	builder->get_widget("troubleclosebutton", troubleClose);

	Gtk::TreeModelColumnRecord cols;
	cols.add(strCol);
	cols.add(valCol);

	troubleListStore = Gtk::ListStore::create(cols);
	troubleList->set_model(troubleListStore);

	troubleList->append_column("Trouble Word", strCol);
	troubleList->append_column("Weight", valCol);

	troubleListStore->set_sort_column(valCol, Gtk::SortType::SORT_DESCENDING);

	// About dialog
	builder->get_widget("aboutdialog", aboutDialog);
}

void TypingTestWindow::connectSignals()
{
	// Connect signals
	newTest->signal_clicked().connect(sigc::mem_fun(*this,
			&TypingTestWindow::genNewTest));
	settingsItem->signal_activate().connect(sigc::mem_fun(*this,
			&TypingTestWindow::openSettings));
	fontItem->signal_activate().connect(sigc::mem_fun(*this,
			&TypingTestWindow::openFont));
	advItem->signal_activate().connect(sigc::mem_fun(*this,
			&TypingTestWindow::openAdvSettings));
	quitItem->signal_activate().connect(sigc::mem_fun(*this,
			&Gtk::ApplicationWindow::close));
	troubleItem->signal_activate().connect(sigc::mem_fun(*this,
			&TypingTestWindow::openTroubleWords));
	aboutItem->signal_activate().connect(sigc::mem_fun(*this,
			&TypingTestWindow::openAbout));

	testTypeBox->signal_changed().connect(sigc::mem_fun(*this,
			&TypingTestWindow::updateSettings));
	randomizeSeed->signal_clicked().connect(sigc::mem_fun(*this,
			&TypingTestWindow::randomSeed));
	apply->signal_clicked().connect(
		sigc::bind<int>(sigc::mem_fun(settingsDialog, &Gtk::Dialog::response),
			Gtk::RESPONSE_APPLY));
	cancel->signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(
				settingsDialog, &Gtk::Dialog::response),
			Gtk::RESPONSE_CANCEL));

	applyAdv->signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(
				advSettingsDialog, &Gtk::Dialog::response),
			Gtk::RESPONSE_APPLY));
	cancelAdv->signal_clicked().connect(
		sigc::bind<int>(sigc::mem_fun(advSettingsDialog,
				&Gtk::Dialog::response), Gtk::RESPONSE_CANCEL));
	restoreDefaultAdv->signal_clicked().connect(sigc::mem_fun(*this,
			&TypingTestWindow::applyDefaultSettings));

	troubleClose->signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(
				troubleDialog, &Gtk::Dialog::response), Gtk::RESPONSE_CLOSE));
}

void TypingTestWindow::genNewTest()
{
	seconds = settings.seconds;
	start = settings.seconds;

	currentTest = TypingTest(this, settings, config);
	words = currentTest.getWords();

	timerLabel->set_text("Timer: " + getTime());
	textBuffer->set_text(currentTest.getWordsAsString());
	textBuffer->apply_tag_by_name("current", textBuffer->get_iter_at_offset(0),
			textBuffer->get_iter_at_offset(words[0]->getWord().length()));
	textBuffer->apply_tag_by_name( "uglyhack", textBuffer->get_iter_at_offset(
			words[0]->getWord().length() + 1), textBuffer->end());

	typingEntry->set_text("");
	typingEntry->grab_focus();

}

void TypingTestWindow::updateSettings()
{
	TestSettings::TestType type = static_cast<TestSettings::TestType>(
		testTypeBox->get_active_row_number());
	switch (type) {
	case TestSettings::CUSTOM :
		topWords->set_sensitive(true);
		minWordLength->set_sensitive(true);
		maxWordLength->set_sensitive(true);
		testLength->set_sensitive(true);
		seedEntry->set_sensitive(true);
		randomizeSeed->set_sensitive(true);
		personalFrequencyButton->set_sensitive(true);
		settings.type = TestSettings::CUSTOM;
		break;
	default :
		topWords->set_sensitive(false);
		minWordLength->set_sensitive(false);
		maxWordLength->set_sensitive(false);
		testLength->set_sensitive(false);
		seedEntry->set_sensitive(false);
		randomizeSeed->set_sensitive(false);
		personalFrequencyButton->set_sensitive(false);
		settings = type;
		break;
	}

	topWords->set_value(settings.topWords);
	minWordLength->set_value(settings.minLength);
	maxWordLength->set_value(settings.maxLength);
	testLength->set_value(settings.seconds.count());
	seedEntry->set_text(std::to_string(settings.seed));
}

void TypingTestWindow::randomSeed()
{
	seedEntry->set_text(std::to_string(
			std::chrono::system_clock::now().time_since_epoch().count()
			% UINT32_MAX));
}

void TypingTestWindow::openSettings()
{
	testTypeBox->set_active(settings.type);
	updateSettings();
	int response = settingsDialog->run();
	if (response == Gtk::RESPONSE_APPLY) {
		settings.topWords = topWords->get_value_as_int();
		settings.minLength = minWordLength->get_value_as_int();
		settings.maxLength = maxWordLength->get_value_as_int();
		settings.seconds = std::chrono::seconds(testLength->get_value_as_int());
		settings.seed = std::stoul(seedEntry->get_text());
		settings.personalFrequency = personalFrequencyButton->get_value();

		genNewTest();
	}
	settingsDialog->close();
}

void TypingTestWindow::openFont()
{
	Gtk::FontChooserDialog fontChooser("Select a font", *this);
	fontChooser.set_font(currFont);
	int response = fontChooser.run();
	if (response == Gtk::RESPONSE_OK) {
		textView->override_font(
			Pango::FontDescription(fontChooser.get_font()));
	}
}

void TypingTestWindow::updateAdvSettings()
{
	startWords->set_value(config.startWords);
	minZScore->set_value(config.minZScore);
	maxZScore->set_value(config.maxZScore);
	startTroubleScore->set_value(config.startTroubleScore);
	troubleDec->set_value(config.troubleDec);
	troubleInc->set_value(config.troubleInc);
	wordWrongMult->set_value(config.wordWrongWeight);
}

void TypingTestWindow::applyDefaultSettings()
{
	Config newConfig;
	config = newConfig;

	updateAdvSettings();
}

void TypingTestWindow::openAdvSettings()
{
	updateAdvSettings();

	int response = advSettingsDialog->run();
	if (response == Gtk::RESPONSE_APPLY) {
		config.startWords = startWords->get_value_as_int();
		config.minZScore = minZScore->get_value();
		config.maxZScore = maxZScore->get_value();
		config.startTroubleScore = startTroubleScore->get_value_as_int();
		config.troubleDec = troubleDec->get_value_as_int();
		config.troubleInc = troubleInc->get_value_as_int();
		config.wordWrongWeight = wordWrongMult->get_value();
		config.saveConfig();
		genNewTest();
	}

	advSettingsDialog->close();
}

void TypingTestWindow::openTroubleWords()
{
	std::ifstream trWords(config.dataDir + "troublewords.txt");

	troubleListStore->clear();

	std::string line;
	while (std::getline(trWords, line)) {
		std::string word = line.substr(0, line.find(","));
		unsigned int val = std::stoi(line.substr(line.find(",") + 1));
		Gtk::ListStore::Row row = *(troubleListStore->append());
		row[strCol] = word;
		row[valCol] = val;
	}
	trWords.close();

	troubleDialog->run();
	troubleDialog->close();
}

void TypingTestWindow::openAbout()
{
	aboutDialog->run();
	aboutDialog->close();
}

std::string TypingTestWindow::getTime()
{
	std::string secstr = (seconds.count() % 60 < 10 ?  "0"
		+ std::to_string(seconds.count() % 60) : std::to_string(seconds.count()
			% 60));
	return std::to_string(seconds.count() / 60) + ":" + secstr;
}

void TypingTestWindow::textInsert(std::string text, int *)
{
	if (!testStarted) {
		testStarted = true;
		timerConnection =
			Glib::signal_timeout().connect(sigc::mem_fun(*this,
					&TypingTestWindow::updateTimer), 1000);
		words[0]->startTime();
	}

	if (!testEnded) {
		if (text[0] == ' ') {
			std::string word = typingEntry->get_text();
			word.erase(std::remove(word.begin(), word.end(), ' '), word.end());
			typingEntry->set_text("");

			textBuffer->remove_tag_by_name("current",
				textBuffer->get_iter_at_offset(wordCharIndex),
				textBuffer->get_iter_at_offset(wordCharIndex
					+ words[wordIndex]->getWord().length()));
			textBuffer->remove_tag_by_name("currenterror",
				textBuffer->get_iter_at_offset(wordCharIndex),
				textBuffer->get_iter_at_offset(wordCharIndex
					+ words[wordIndex]->getWord().length()));

			if (words[wordIndex]->enterWord(word, config))
				textBuffer->apply_tag_by_name("good",
					textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex +
						words[wordIndex]->getWord().length()));
			else
				textBuffer->apply_tag_by_name("error",
					textBuffer->get_iter_at_offset(wordCharIndex),
					textBuffer->get_iter_at_offset(wordCharIndex
						+ words[wordIndex]->getWord().length()));

			wordCharIndex += words[wordIndex]->getWord().length() + 1;
			wordIndex++;

			textBuffer->remove_tag_by_name("uglyhack",
				textBuffer->get_iter_at_offset(0), textBuffer->end());

			std::string newWord = currentTest.genWord();
			words.push_back(std::shared_ptr<Word>(new Word(newWord)));
			textBuffer->insert(textBuffer->end(), " " + newWord);

			textBuffer->apply_tag_by_name("uglyhack",
				textBuffer->get_iter_at_offset(wordCharIndex
					+ words[wordIndex]->getWord().length() + 1),
				textBuffer->end());

			Gtk::TextBuffer::iterator itr =
				textBuffer->get_iter_at_offset(wordCharIndex);
			textView->scroll_to(itr, 0.2);
		} else {
			if (!words[wordIndex]->getStarted()) {
				words[wordIndex]->startTime();
			}
		}

		std::string text = typingEntry->get_text();
		std::string word = words[wordIndex]->getWord();
		if (text.length() <= word.length() && text == word.substr(0, text.length())) {
			textBuffer->remove_tag_by_name("currenterror",
				textBuffer->get_iter_at_offset(wordCharIndex),
				textBuffer->get_iter_at_offset(wordCharIndex
					+ words[wordIndex]->getWord().length()));
			textBuffer->apply_tag_by_name("current",
				textBuffer->get_iter_at_offset(wordCharIndex),
				textBuffer->get_iter_at_offset(wordCharIndex
					+ words[wordIndex]->getWord().length()));
		} else {
			textBuffer->remove_tag_by_name("current",
				textBuffer->get_iter_at_offset(wordCharIndex),
				textBuffer->get_iter_at_offset(wordCharIndex +
					words[wordIndex]->getWord().length()));
			textBuffer->apply_tag_by_name("currenterror",
				textBuffer->get_iter_at_offset(wordCharIndex),
				textBuffer->get_iter_at_offset(wordCharIndex +
					words[wordIndex]->getWord().length()));
		}
	}
}

void TypingTestWindow::textDelete(int, int)
{
	if (!testEnded) {
		std::string text = typingEntry->get_text();
		std::string word = words[wordIndex]->getWord();
		if (text.length() <= word.length() && text == word.substr(0,
				text.length())) {
			textBuffer->remove_tag_by_name("currenterror",
				textBuffer->get_iter_at_offset(wordCharIndex),
				textBuffer->get_iter_at_offset(wordCharIndex
					+ words[wordIndex]->getWord().length()));
			textBuffer->apply_tag_by_name("current",
				textBuffer->get_iter_at_offset(wordCharIndex),
				textBuffer->get_iter_at_offset(wordCharIndex
					+ words[wordIndex]->getWord().length()));
		} else {
			textBuffer->remove_tag_by_name("current",
				textBuffer->get_iter_at_offset(wordCharIndex),
				textBuffer->get_iter_at_offset(wordCharIndex
					+ words[wordIndex]->getWord().length()));
			textBuffer->apply_tag_by_name("currenterror",
				textBuffer->get_iter_at_offset(wordCharIndex),
				textBuffer->get_iter_at_offset(wordCharIndex
					+ words[wordIndex]->getWord().length()));
		}
	}
}

bool TypingTestWindow::updateTimer()
{
	seconds--;
	timerLabel->set_text("Timer: " + getTime());
	if (seconds != std::chrono::seconds::duration::zero()) {
		return true;
	} else {
		textBuffer->erase(textBuffer->get_iter_at_offset(wordCharIndex
				+ words[wordIndex]->getWord().length()), textBuffer->end());
		calculateScore();
		testEnded = true;
		return false;
	}
}

void TypingTestWindow::calculateScore()
{
	//Test information
	int wordNum = 0;
	int wordsCorrect = 0;
	int charNum = 0;
	int charsCorrect = 0;
	for (int i = 0; i < wordIndex; ++i) {
		wordNum++;
		charNum += words[i]->getWord().length() + 1;
		charsCorrect++;
		if (words[i]->getCorrect()) {
			wordsCorrect++;
			charsCorrect += words[i]->getWord().length();
		}
	}

	//Trouble words
	std::vector<std::tuple<std::string, double>> wordScores;
	for (int i = 0; i < wordIndex; ++i) {
		bool found = false;
		for (std::tuple<std::string, double> wordScore : wordScores) {
			if (words[i]->getWord() == std::get<0>(wordScore)) {
				std::get<1>(wordScore) = std::min(std::get<1>(wordScore),
					words[i]->getScore());
				found = true;
				break;
			}
		}
		if (!found)
			wordScores.push_back(std::make_tuple(words[i]->getWord(),
					words[i]->getScore()));
	}

	//Calculate averages and total
	double total = 0;
	for (std::tuple<std::string, double> wordScore : wordScores)
		total += std::get<1>(wordScore);
	double mean = total / wordScores.size();

	//Standard deviation
	double sum = 0;
	for (std::tuple<std::string, double> wordScore : wordScores)
		sum += std::pow(std::get<1>(wordScore) - mean, 2);

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
		} else if (std::get<1>(wordScore) - mean > config.maxZScore * stdDev)
			goodWords.push_back(std::get<0>(wordScore));
	}

	std::ifstream file(config.dataDir + "troublewords.txt");
	std::ofstream temp(config.dataDir + ".troublewords.txt.swp",
		std::ios::trunc);

	if (!temp.is_open())
		errx(EXIT_FAILURE, nullptr);

	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			std::string word = line.substr(0, line.find(","));
			int num = std::stoi(line.substr(line.find(",") + 1));

			std::vector<std::string>::iterator it
				= std::find(troubleWords.begin(), troubleWords.end(), word);
			if (it != troubleWords.end()) {
				temp << word << "," << num + config.troubleInc << "\n";
				troubleWords.erase(it);
			} else {
				it = std::find(goodWords.begin(), goodWords.end(), word);
				if (it != goodWords.end()) {
					if (num > 1)
						temp << word << "," << num - config.troubleDec << "\n";
					goodWords.erase(it);
				} else
					temp << line << "\n";
			}
		}
	}

	for (std::string word : troubleWords)
		temp << word << "," << config.startTroubleScore << "\n";

	file.close();
	temp.close();

	std::remove((config.dataDir + "troublewords.txt").c_str());
	std::rename((config.dataDir + ".troublewords.txt.swp").c_str(),
		(config.dataDir + "troublewords.txt").c_str());

	wpmLabel->set_text("WPM: " + std::to_string((int) ((charsCorrect / 5.0)
				/ (start.count() / 60.0))));
	wordNumLabel->set_text("Words: " + std::to_string(wordNum));
	wordsCorrectLabel->set_text("Correct: " + std::to_string(wordsCorrect));
	wordsWrongLabel->set_text("Wrong: " + std::to_string(wordNum
			- wordsCorrect));
	charNumLabel->set_text("Characters: " + std::to_string(charNum));
	charsCorrectLabel->set_text("Correct: " + std::to_string(charsCorrect));
	charsWrongLabel->set_text("Wrong: " + std::to_string(charNum
			- charsCorrect));
	troubleWordsLabel->set_text(troubleWordsStr);
}
} /* namespace typingtest */
