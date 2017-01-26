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

namespace typingtest {

TypingTestWindow::TypingTestWindow(BaseObjectType *cobject, const
	Glib::RefPtr<Gtk::Builder>& builder) : Gtk::ApplicationWindow(cobject),
	builder(builder)
{
}

void genNewTest()
{
	delete test;
	test = new TypingTest(appWindow, widgets, settings);
}

void updateSettings()
{
	TestType type = getTypeFromNumber(testTypeBox->get_active_row_number());
	switch (type) {
		case CUSTOM :
			topWords->set_sensitive(true);
			minWordLength->set_sensitive(true);
			maxWordLength->set_sensitive(true);
			testLength->set_sensitive(true);
			seedEntry->set_sensitive(true);
			randomizeSeed->set_sensitive(true);
			personalFrequency->set_sensitive(true);
			settings.type = CUSTOM;
			break;
		default :
			topWords->set_sensitive(false);
			minWordLength->set_sensitive(false);
			maxWordLength->set_sensitive(false);
			testLength->set_sensitive(false);
			seedEntry->set_sensitive(false);
			randomizeSeed->set_sensitive(false);
			personalFrequency->set_sensitive(false);
			settings = getTestTypeSettings(type);
			break;
	}

	topWords->set_value(settings.topWords);
	minWordLength->set_value(settings.minLength);
	maxWordLength->set_value(settings.maxLength);
	testLength->set_value(settings.seconds.count());
	seedEntry->set_text(std::to_string(settings.seed));
}

void randomSeed()
{
	seedEntry->set_text(std::to_string(
				std::chrono::system_clock::now().time_since_epoch().count() % UINT32_MAX));
}

void openSettings()
{
	testTypeBox->set_active(getTypeNumber(settings.type));
	updateSettings();
	int response = settingsDialog->run();
	if (response == Gtk::RESPONSE_APPLY) {
		settings.topWords = topWords->get_value_as_int();
		settings.minLength = minWordLength->get_value_as_int();
		settings.maxLength = maxWordLength->get_value_as_int();
		settings.seconds = std::chrono::seconds(testLength->get_value_as_int());
		settings.seed = std::stoul(seedEntry->get_text());
		settings.personalFrequency = personalFrequency->get_value();

		genNewTest();
	}
	settingsDialog->close();
}

void openFont()
{
	fontChooser->set_font(currFont);
	int response = fontChooser->run();
	if (response == Gtk::RESPONSE_OK) {
		widgets.textView->override_font(Pango::FontDescription(fontChooser->get_font()));
	}
	fontChooser->close();
}

void updateAdvSettings()
{
	startWords->set_value(config.startWords);
	minZScore->set_value(config.minZScore);
	maxZScore->set_value(config.maxZScore);
	startTroubleScore->set_value(config.startTroubleScore);
	troubleDec->set_value(config.troubleDec);
	troubleInc->set_value(config.troubleInc);
	wordWrongMult->set_value(config.wordWrongWeight);
}

void applyDefaultSettings()
{
	Config newConfig;
	config = newConfig;

	updateAdvSettings();
}

void openAdvSettings()
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
		saveConfig();
		genNewTest();
	}

	advSettingsDialog->close();
}

void openTroubleWords()
{
	std::ifstream trWords(data_dir + "troublewords.txt");

	troubleListStore->clear();

	std::string line;
	while (std::getline(trWords, line)) {
		std::string word = line.substr(0, line.find(","));
		unsigned int val = std::stoi(line.substr(line.find(",") + 1));
		Gtk::ListStore::Row row = *(troubleListStore->append());
		row[strCol] = word;
		row[valCol] = val;
	}

	troubleDialog->run();
	troubleDialog->close();
}

void openAbout()
{
	aboutDialog->run();
	aboutDialog->close();
}

void
TypingTestWindow::initWidgets()
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

	textBuffer = Glib::RefPtr<Gtk::TextBuffer>::cast_static(builder->get_object("textbuffer"));
	textTags = Glib::RefPtr<Gtk::TextTagTable>::cast_static(builder->get_object("texttags"));
	currentTag = Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("currenttag"));
	currentErrorTag = Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("currenterrortag"));
	errorTag = Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("errortag"));
	goodTag = Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("goodtag"));
	uglyHackTag = Glib::RefPtr<Gtk::TextTag>::cast_static(builder->get_object("uglyhacktag"));

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
	builder->get_widget("personalratioentry", personalFrequency);

	// Font settings window
	fontChooser = new Gtk::FontChooserDialog("Select a font", *appWindow);

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

	//Trouble words viewer
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

	//About dialog
	builder->get_widget("aboutdialog", aboutDialog);

	//Connect signals
	newTest->signal_clicked().connect(sigc::ptr_fun(&genNewTest));
	settingsItem->signal_activate().connect(sigc::ptr_fun(&openSettings));
	fontItem->signal_activate().connect(sigc::ptr_fun(&openFont));
	advItem->signal_activate().connect(sigc::ptr_fun(&openAdvSettings));
	quitItem->signal_activate().connect(sigc::mem_fun(appWindow, &Gtk::ApplicationWindow::close));
	troubleItem->signal_activate().connect(sigc::ptr_fun(&openTroubleWords));
	aboutItem->signal_activate().connect(sigc::ptr_fun(&openAbout));

	testTypeBox->signal_changed().connect(sigc::ptr_fun(&updateSettings));
	randomizeSeed->signal_clicked().connect(sigc::ptr_fun(&randomSeed));
	apply->signal_clicked().connect(
			sigc::bind<int>(sigc::mem_fun(settingsDialog, &Gtk::Dialog::response), Gtk::RESPONSE_APPLY));
	cancel->signal_clicked().connect(
			sigc::bind<int>(sigc::mem_fun(settingsDialog, &Gtk::Dialog::response), Gtk::RESPONSE_CANCEL));

	applyAdv->signal_clicked().connect(
			sigc::bind<int>(sigc::mem_fun(advSettingsDialog, &Gtk::Dialog::response), Gtk::RESPONSE_APPLY));
	cancelAdv->signal_clicked().connect(
			sigc::bind<int>(sigc::mem_fun(advSettingsDialog, &Gtk::Dialog::response), Gtk::RESPONSE_CANCEL));
	restoreDefaultAdv->signal_clicked().connect(sigc::ptr_fun(&applyDefaultSettings));

	troubleClose->signal_clicked().connect(
			sigc::bind<int>(sigc::mem_fun(troubleDialog, &Gtk::Dialog::response), Gtk::RESPONSE_CLOSE));
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

	timerLabel->set_text("Timer: " + getTime());

	gsize size;
	std::stringstream dictStream((char *) Gio::Resource::lookup_data_global(
				"/us/laelath/typingtest/dictionary.txt")->get_data(size));

	wordSelection.reserve(settings.topWords);
	for (size_t i = 0; i < settings.topWords; ) {
		std::string line;
		if (std::getline(dictStream, line)) {
			if (line.length() >= settings.minLength && line.length() <= settings.maxLength) {
				wordSelection.push_back(line);
				i++;
			}
		} else {
			break;
		}
	}

	if (settings.personalFrequency != 0) {
		std::ifstream trWords(data_dir + "troublewords.txt");
		if (!trWords.is_open()) {
			Gtk::MessageDialog error(*parent, "No trouble words file found.", false, Gtk::MESSAGE_ERROR,
					Gtk::BUTTONS_CLOSE, true);
			error.set_secondary_text("You need to finish a test before trouble words will be detected.");
			error.run();
			disconnectSignals();
			return;
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

		if (personalSelection.size() == 0) {
			Gtk::MessageDialog error(*parent, "Trouble words file empty.", false, Gtk::MESSAGE_ERROR,
					Gtk::BUTTONS_CLOSE, true);
			error.set_secondary_text("You need to finish a test before trouble words will be detected.");
			error.run();
			disconnectSignals();
			return;
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
	timerLabel->set_text("Timer: " + getTime());
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

	std::ifstream file(data_dir + "troublewords.txt");
	std::ofstream temp(data_dir + ".troublewords.txt.swp", std::ios::trunc);

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

	std::remove((data_dir + "troublewords.txt").c_str());
	std::rename((data_dir + ".troublewords.txt.swp").c_str(), (data_dir + "troublewords.txt").c_str());

	wpmLabel->set_text("WPM: " + std::to_string((int) ((charsCorrect / 5.0) / (start.count() / 60.0))));
	wordNumLabel->set_text("Words: " + std::to_string(wordNum));
	wordsCorrectLabel->set_text("Correct: " + std::to_string(wordsCorrect));
	wordsWrongLabel->set_text("Wrong: " + std::to_string(wordNum - wordsCorrect));
	charNumLabel->set_text("Characters: " + std::to_string(charNum));
	charsCorrectLabel->set_text("Correct: " + std::to_string(charsCorrect));
	charsWrongLabel->set_text("Wrong: " + std::to_string(charNum - charsCorrect));
	troubleWordsLabel->set_text(troubleWordsStr);
}
} /* namespace typingtest */
