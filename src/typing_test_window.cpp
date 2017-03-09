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
// TypingTest.  If not, see <http:// www.gnu.org/licenses/>.

#include "typing_test_window.h"

#include <stdlib.h>

#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <numeric>

#include <gdk/gdkkeysyms.h>

#include "files.h"
#include "popup_menu.h"
#include "sticker_dialog.h"

namespace typingtest {

TypingTestWindow::TypingTestWindow(BaseObjectType *cobject,
	const Glib::RefPtr<Gtk::Builder> &builder)
	: Gtk::ApplicationWindow(cobject),
	  builder(builder),
	  settings(TestSettings::BASIC)
{
	initWidgets();
	connectSignals();
	initActions();

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

	// Notes
	builder->get_widget("note_view", noteView);
	builder->get_widget("save_note_button", saveNoteButton);
	builder->get_widget("insert_sticker_button", insertStickerButton);

	noteView->set_wrap_mode(Gtk::WRAP_WORD);
	noteBuffer = StickerBuffer::create();
	noteView->set_buffer(noteBuffer);

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
	builder->get_widget("capital_frequency_entry", capitalFrequencyButton);

	// Advanced settings window
	builder->get_widget("advancedsettingsdialog", advSettingsDialog);
	builder->get_widget("startwords", startWords);
	builder->get_widget("minzscore", minZScore);
	builder->get_widget("maxzscore", maxZScore);
	builder->get_widget("starttrouble", startTroubleScore);
	builder->get_widget("troubledec", troubleDec);
	builder->get_widget("troubleinc", troubleInc);
	builder->get_widget("highlight_mode_box", hlModeBox);
	builder->get_widget("restoredefaultadv", restoreDefaultAdv);
	builder->get_widget("canceladv", cancelAdv);
	builder->get_widget("applyadv", applyAdv);

	builder->get_widget("history_dialog", historyDialog);
	builder->get_widget("history_close_button", historyCloseButton);
	builder->get_widget("erase_history_button", eraseHistoryButton);
	builder->get_widget("average_speed_label", averageSpeedLabel);
	builder->get_widget("fastest_time_label", fastestTimeLabel);
	builder->get_widget("current_fastest_time_label", currentFastestTimeLabel);
	builder->get_widget("current_slowest_time_label", currentSlowestTimeLabel);
	builder->get_widget("test_history_view", testHistoryView);
	builder->get_widget("current_standard_deviation_label",
		currentStandardDeviationLabel);

	historyColumnRecord.add(wpmColumn);
	historyColumnRecord.add(lengthColumn);
	historyColumnRecord.add(typeColumn);
	historyStore = Gtk::ListStore::create(historyColumnRecord);
	testHistoryView->set_model(historyStore);
	testHistoryView->append_column("WPM", wpmColumn);
	testHistoryView->append_column("Length", lengthColumn);
	testHistoryView->append_column("Type", typeColumn);

	builder->get_widget("notes_dialog", notesDialog);
	builder->get_widget("dialog_save_note_button", dialogSaveNoteButton);
	builder->get_widget("dialog_insert_sticker_button",
		dialogInsertStickerButton);
	builder->get_widget("close_notes_button", closeNotesButton);
	builder->get_widget("notes_view", notesView);
	builder->get_widget("note_name_entry", noteNameEntry);
	builder->get_widget("dialog_note_view", dialogNoteView);

	notesColumnRecord.add(noteNameColumn);
	notesColumnRecord.add(noteDateColumn);
	notesColumnRecord.add(noteContentsColumn);
	notesStore = Gtk::ListStore::create(notesColumnRecord);
	notesView->set_model(notesStore);
	notesView->append_column("Name", noteNameColumn);
	notesView->append_column("Date", noteDateColumn);
	dialogNoteBuffer = StickerBuffer::create();
	dialogNoteView->set_buffer(dialogNoteBuffer);

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
	/* typingEntry->signal_key_press_event().connect_notify(sigc::mem_fun(*this, */
	typingEntry->signal_key_press_event().connect(sigc::mem_fun(*this,
			&TypingTestWindow::onTypingEntryKeyPress));

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

	saveNoteButton->signal_clicked().connect(sigc::mem_fun(*this,
			&TypingTestWindow::onSaveNoteButtonClicked));
	insertStickerButton->signal_clicked().connect(sigc::mem_fun(*this,
			&TypingTestWindow::onInsertStickerButtonClicked));

	troubleClose->signal_clicked().connect(sigc::bind<int>(sigc::mem_fun(
				troubleDialog, &Gtk::Dialog::response), Gtk::RESPONSE_CLOSE));

	historyCloseButton->signal_clicked().connect(sigc::mem_fun(*this,
			&TypingTestWindow::onHistoryCloseButtonClicked));
	eraseHistoryButton->signal_clicked().connect(sigc::mem_fun(*this,
			&TypingTestWindow::onEraseHistoryButtonClicked));


	dialogSaveNoteButton->signal_clicked().connect(sigc::mem_fun(*this,
			&TypingTestWindow::onDialogSaveNoteButtonClicked));
	dialogInsertStickerButton->signal_clicked().connect(sigc::mem_fun(*this,
			&TypingTestWindow::onDialogInsertStickerButtonClicked));
	notesView->signal_button_press_event().connect_notify(sigc::mem_fun(*this,
			&TypingTestWindow::onDialogNotesViewButtonPressEvent));
	notesView->signal_row_activated().connect(sigc::mem_fun(*this,
			&TypingTestWindow::onNotesViewRowActivated));
	closeNotesButton->signal_clicked().connect(sigc::mem_fun(*notesDialog,
			&Gtk::Dialog::close));
}

void TypingTestWindow::genNewTest()
{
	testEnded = false;
	testStarted = false;

	seconds = settings.seconds;
	start = settings.seconds;

	currentTest = TypingTest(this, settings, config);
	wordIndex = 0;
	wordCharIndex = 0;
	words = currentTest.getWords();

	timerLabel->set_text("Timer: " + getTime());
	textBuffer->set_text(currentTest.getWordsAsString());
	textBuffer->apply_tag_by_name("current", textBuffer->get_iter_at_offset(0),
			textBuffer->get_iter_at_offset(words[0]->getWord().length()));
	textBuffer->apply_tag_by_name( "uglyhack", textBuffer->get_iter_at_offset(
			words[0]->getWord().length() + 1), textBuffer->end());

	typingEntry->set_text("");
	typingEntry->grab_focus();

	timerConnection.disconnect();
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
		capitalFrequencyButton->set_sensitive(true);
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
		capitalFrequencyButton->set_sensitive(false);
		settings = type;
		break;
	}

	topWords->set_value(settings.topWords);
	minWordLength->set_value(settings.minLength);
	maxWordLength->set_value(settings.maxLength);
	testLength->set_value(settings.seconds.count());
	seedEntry->set_text(std::to_string(settings.seed));
	personalFrequencyButton->set_value(settings.personalFrequency);
	capitalFrequencyButton->set_value(settings.capitalFrequency);
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
		settings.capitalFrequency = capitalFrequencyButton->get_value();

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
	hlModeBox->set_active(config.hlMode);
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
		config.hlMode = static_cast<HighlightMode>(
				hlModeBox->get_active_row_number());
		config.saveConfig();
		genNewTest();
	}

	advSettingsDialog->close();
}

void TypingTestWindow::openTroubleWords()
{
	std::unique_lock<std::mutex>{troubleWordsFileLock};
	std::ifstream trWords(getTroubleWordsPath());

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
	std::string secstr = (seconds.count() % 60 < 10 ? "0"
		+ std::to_string(seconds.count() % 60) : std::to_string(seconds.count()
			% 60));
	return std::to_string(seconds.count() / 60) + ":" + secstr;
}

void TypingTestWindow::textInsert(std::string text, int *)
{
	if (!testStarted && text.length() > 0) {
		testStarted = true;
		timerConnection = Glib::signal_timeout().connect(sigc::mem_fun(*this,
					&TypingTestWindow::updateTimer), 1000);
		words[0]->startTime();
	}
	if (testEnded)
		return;
	if (text[0] == ' ')
		checkWord();
	else if (!words[wordIndex]->getStarted())
		words[wordIndex]->startTime();

	if (config.hlMode == HighlightMode::ALL) {
		applyHighlight();
	}
}

void TypingTestWindow::textDelete(int, int)
{
	if (!testEnded && config.hlMode == HighlightMode::ALL) {
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
	if (testEnded)
		return false;
	seconds--;
	timerLabel->set_text("Timer: " + getTime());
	if (seconds != std::chrono::seconds::duration::zero())
		return true;
	else {
		textBuffer->erase(textBuffer->get_iter_at_offset(wordCharIndex
				+ words[wordIndex]->getWord().length()), textBuffer->end());
		calculateScore();
		testEnded = true;
		return false;
	}
}

void TypingTestWindow::calculateScore()
{
	// Test information
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

	// Trouble words
	std::vector<std::shared_ptr<Word>> enteredWords(words.begin(),
			words.begin() + wordIndex);

	// Calculate averages and total
	double total = 0;
	for (std::shared_ptr<Word> word : enteredWords) {
		if (word->getCorrect())
			total += word->getScore();
	}
	double mean = total / wordsCorrect;

	// Standard deviation
	double sum = 0;
	for (std::shared_ptr<Word> word : enteredWords) {
		if (word->getCorrect())
			sum += std::pow(word->getScore() - mean, 2);
	}
	double stdDev = std::sqrt(sum / wordsCorrect);
	// Sort scores
	std::sort(enteredWords.begin(), enteredWords.end(),
		[](std::shared_ptr<Word> i, std::shared_ptr<Word> j) -> bool {
			return i->getScore() < j->getScore();
		});

	std::set<std::string> troubleWords;
	std::set<std::string> goodWords;

	// Find trouble and good words
	for (std::shared_ptr<Word> word : enteredWords) {
		if (word->getScore() - mean <= config.minZScore * stdDev) {
			std::string wordStr = word->getWord();
			wordStr[0] = std::tolower(wordStr[0]);
			troubleWords.insert(wordStr);
		} else if (word->getScore() - mean > config.maxZScore * stdDev) {
			std::string wordStr = word->getWord();
			wordStr[0] = std::tolower(wordStr[0]);
			goodWords.insert(wordStr);
		}
	}

	// Create label text
	std::string troubleWordsStr;
	for (std::string word : troubleWords)
		troubleWordsStr += word + "\n";

	updateTroubleWordsFile(troubleWords, goodWords);


	int wpm = static_cast<int>((charsCorrect / 5.0) / (start.count() / 60.0));

	wpmLabel->set_text("WPM: " + std::to_string(wpm));
	wordNumLabel->set_text("Words: " + std::to_string(wordNum));
	wordsCorrectLabel->set_text("Correct: " + std::to_string(wordsCorrect));
	wordsWrongLabel->set_text("Wrong: " + std::to_string(wordNum
			- wordsCorrect));
	charNumLabel->set_text("Characters: " + std::to_string(charNum));
	charsCorrectLabel->set_text("Correct: " + std::to_string(charsCorrect));
	charsWrongLabel->set_text("Wrong: " + std::to_string(charNum
			- charsCorrect));
	troubleWordsLabel->set_text(troubleWordsStr);

	lastScore = wpm;
	hasLastScore = true;

	updateHistoryFile(wpm);
}

void TypingTestWindow::initActions()
{
	this->add_action("show-history", sigc::mem_fun(*this,
			&TypingTestWindow::onActionShowHistory));
	this->add_action("open-notes", sigc::mem_fun(*this,
			&TypingTestWindow::onActionOpenNotes));
}

void TypingTestWindow::onHistoryCloseButtonClicked()
{
	historyDialog->response(Gtk::RESPONSE_CLOSE);
}

void TypingTestWindow::onActionShowHistory()
{
	std::unique_lock<std::mutex> lock{historyFileLock};

	std::string outputPath{getHistoryPath()};

	int recordWpm{0};
	std::vector<TestInfo> historyInfo{readHistory(outputPath, recordWpm)};

	int averageWpm{static_cast<int>(getAverageWpm(historyInfo))};
	double standardDeviation{getStandardDeviation(historyInfo)};
	int maxWpm{getMaxWpm(historyInfo)};
	int minWpm{getMinWpm(historyInfo)};

	fastestTimeLabel->set_text(std::to_string(recordWpm));
	averageSpeedLabel->set_text(std::to_string(averageWpm));
	currentFastestTimeLabel->set_text(std::to_string(maxWpm));
	currentSlowestTimeLabel->set_text(std::to_string(minWpm));
	currentStandardDeviationLabel->set_text(std::to_string(standardDeviation));

	historyStore->clear();
	for (const auto &info : historyInfo) {
		Gtk::TreeIter iter{historyStore->append()};
		Gtk::TreeRow row{*iter};
		row[wpmColumn] = info.getWpm();
		row[lengthColumn] = std::to_string(info.getLength().count());
		row[typeColumn] = toString(info.getType());
	}

	historyDialog->run();
	historyDialog->close();
}

void TypingTestWindow::onActionOpenNotes()
{
	notesStore->clear();
	loadNotes();

	notesDialog->run();
	notesDialog->close();
	clearNotesDialog();
}

void TypingTestWindow::updateHistoryFile(int wpm)
{
	std::unique_lock<std::mutex> lock{historyFileLock};
	int recordWpm{0};
	std::string historyPath{getHistoryPath()};
	std::string historySwapPath{getSwapPath(historyPath)};
	std::vector<TestInfo> history = readHistory(historyPath, recordWpm);
	history.push_back(TestInfo{wpm, settings});
	if (history.size() > HISTORY_SIZE) {
		std::vector<TestInfo>newHistory{history.end() - HISTORY_SIZE,
			history.end()};
		history = newHistory;
	}

	recordWpm = (wpm > recordWpm) ? wpm : recordWpm;
	std::ofstream writer{historySwapPath};
	if (writer.is_open()) {
		writer << recordWpm << std::endl;
		for (const auto &info : history)
			writer << info << std::endl;
		writer.close();
		save(historyPath, historySwapPath);
	}
}

void TypingTestWindow::updateTroubleWordsFile(
	std::set<std::string> troubleWords,
	std::set<std::string> goodWords)
{
	std::unique_lock<std::mutex>{troubleWordsFileLock};

	std::string troubleWordsPath{getTroubleWordsPath()};
	std::string troubleWordsSwapPath{getSwapPath(troubleWordsPath)};

	std::ofstream temp{troubleWordsSwapPath};
	if (!temp.is_open())
		return;

	std::ifstream file{troubleWordsPath};
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			std::string word = line.substr(0, line.find(","));
			int num = std::stoi(line.substr(line.find(",") + 1));

			auto it = std::find(troubleWords.begin(), troubleWords.end(),
				word);
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

	save(troubleWordsPath, troubleWordsSwapPath);
}

double TypingTestWindow::getAverageWpm(const std::vector<TestInfo> &history)
{
	if (history.size() == 0)
		return 0;
	int averageWpm{0};
	for (const auto &info : history)
		averageWpm += info.getWpm();
	averageWpm /= history.size();
	return averageWpm;
}

double TypingTestWindow::getStandardDeviation(
	const std::vector<TestInfo> &history)
{
	if (history.size() == 0)
		return 0;
	double average{getAverageWpm(history)};
	auto diff = [&average](double total, const TestInfo &info) {
		return total + std::pow(info.getWpm() - average, 2);
	};
	double variance{std::accumulate(history.begin(), history.end(), 0.0,
		diff) / history.size()};
	return std::sqrt(variance);
}

int TypingTestWindow::getMaxWpm(const std::vector<TestInfo> &history)
{
	auto maxIt = std::max_element(history.begin(), history.end(),
		&TypingTestWindow::compareWpm);
	return (maxIt != history.end()) ? maxIt->getWpm() : 0;
}

int TypingTestWindow::getMinWpm(const std::vector<TestInfo> &history)
{
	auto maxIt = std::min_element(history.begin(), history.end(),
		&TypingTestWindow::compareWpm);
	return (maxIt != history.end()) ? maxIt->getWpm() : 0;
}

bool TypingTestWindow::compareWpm(const TestInfo &t1, const TestInfo &t2)
{
	return t1.getWpm() < t2.getWpm();
}

std::string TypingTestWindow::getHistoryPath() const
{
	return config.dataDir + "history.txt";
}

std::string TypingTestWindow::getTroubleWordsPath() const
{
	return config.dataDir + "troublewords.txt";
}

std::vector<TestInfo> TypingTestWindow::readHistory(const std::string &path,
	int &recordWpm)
{
	std::ifstream reader{path};
	std::vector<TestInfo> history;
	recordWpm = 0;

	// The data file stats with one line that is the record wpm, then a list of
	// TestInfo objects.
	if (reader.is_open() && reader >> recordWpm) {
		TestInfo testInfo;
		while (reader >> testInfo)
			history.push_back(testInfo);
	}
	reader.close();
	return history;
}

void TypingTestWindow::onEraseHistoryButtonClicked()
{
	Gtk::MessageDialog dialog{*historyDialog, "Delete history?", false,
		Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO, true};
	int response = dialog.run();
	if (response == Gtk::RESPONSE_YES) {
		std::remove(getHistoryPath().c_str());
		resetHistoryDisplay();
	}
}

void TypingTestWindow::resetHistoryDisplay()
{
	std::string zeroString{std::to_string(0)};
	std::string zeroDoubleString{std::to_string(0.0)};

	averageSpeedLabel->set_text(zeroString);
	fastestTimeLabel->set_text(zeroString);
	currentFastestTimeLabel->set_text(zeroString);
	currentSlowestTimeLabel->set_text(zeroString);
	currentStandardDeviationLabel->set_text(zeroDoubleString);

	historyStore->clear();
}

void TypingTestWindow::onDialogSaveNoteButtonClicked()
{
	Glib::ustring noteName{noteNameEntry->get_text()};
	if (noteName.length() == 0)
		return;
	if (isEditingNote)
		Note::deleteNote(editedNoteName, noteDir());
	else if (Glib::file_test(noteDir() + "/" + noteName,
			Glib::FILE_TEST_EXISTS)) {
		Gtk::MessageDialog dialog{*notesDialog,
			"Note with that name already exists.", false, Gtk::MESSAGE_WARNING,
			Gtk::BUTTONS_OK, true};
		dialog.run();
		return;
	}
	Note note{noteName};
	note.setContents(dialogNoteBuffer->getTextWithStickers());
	if (note.save(noteDir())) {
		noteNameEntry->set_text("");
		dialogNoteBuffer->set_text("");
		if (isEditingNote)
			eraseNoteWithName(editedNoteName);
		addNoteToDialog(note);
	}
}

void TypingTestWindow::onDialogInsertStickerButtonClicked()
{
	StickerDialog dialog{*this};
	dialog.run();
	if (dialog.hasSticker())
		dialogNoteBuffer->insertSticker(dialog.getStickerName());
}

void TypingTestWindow::onInsertStickerButtonClicked()
{
	StickerDialog dialog{*this};
	dialog.run();
	if (dialog.hasSticker())
		noteBuffer->insertSticker(dialog.getStickerName());
}

void TypingTestWindow::onSaveNoteButtonClicked()
{
	std::string noteName{"Anonymous Note"};
	if (hasLastScore) {
		noteName = "Note for test with " + std::to_string(lastScore);
	}
	noteName = Note::uniqueNoteName(noteName, noteDir());
	Note note{noteName};
	std::string contents{noteBuffer->getTextWithStickers()};
	note.setContents(contents);
	if (note.save(noteDir()))
		noteBuffer->set_text("");
	else {
		Gtk::MessageDialog dialog{*this, "Failed to save note.", false,
			Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true};
		dialog.run();
	}
}

std::shared_ptr<TypingTestWindow> TypingTestWindow::create()
{
	auto builder = Gtk::Builder::create_from_resource(
		"/us/laelath/typingtest/ui/typingui.glade");
	typingtest::TypingTestWindow *window = nullptr;
	builder->get_widget_derived("typingtest", window);
	return std::shared_ptr<TypingTestWindow>{window};
}

bool TypingTestWindow::onTypingEntryKeyPress(GdkEventKey *keyEvent)
{
	if (keyEvent->keyval == GDK_KEY_Tab) {
		newTest->grab_focus();
		return true;
	}
	return false;;
}

void TypingTestWindow::addNoteToDialog(const Note &note)
{
	Gtk::TreeRow row = *notesStore->append();

	row[noteNameColumn] = note.getName();
	row[noteContentsColumn] = note.getContents();
	row[noteDateColumn] = note.getTimeString();
}

std::string TypingTestWindow::noteDir() const
{
	return config.dataDir + "notes";
}

void TypingTestWindow::loadNotes()
{
	if (!Glib::file_test(noteDir(), Glib::FILE_TEST_EXISTS)) {
		auto dirFile = Gio::File::create_for_path(noteDir());
		dirFile->make_directory();
	}
	Glib::Dir notesDirectory{noteDir()};
	for (const auto &dirent : notesDirectory) {
		std::string notePath{noteDir() + "/" + dirent};
		if (Glib::file_test(notePath, Glib::FILE_TEST_IS_REGULAR))
			addNoteToDialog(Note::readNote(notePath));
	}
}

void TypingTestWindow::onDialogNotesViewButtonPressEvent(GdkEventButton *button)
{
    if (button->type != GDK_BUTTON_PRESS
        || button->button != GDK_BUTTON_SECONDARY)
        return;

    Gtk::TreePath selectedPath;
    bool isOnRow = notesView->get_path_at_pos(button->x, button->y,
		selectedPath);
	if (!isOnRow)
		return;
	Gtk::TreeRowReference selectedRef{notesStore, selectedPath};
	Gtk::TreeRow selectedRow{*notesStore->get_iter(selectedPath)};

	PopupMenu *menu{PopupMenu::create(*this)};

	menu->addItem("Delete", sigc::mem_fun(*this,
			&TypingTestWindow::onDialogDeleteNote));
	menu->addItem("Load", sigc::mem_fun(*this,
			&TypingTestWindow::onLoadNoteItemActivated));

	menu->run(selectedRef, button->button, button->time);
}

void TypingTestWindow::onDialogDeleteNote(Gtk::TreeRowReference selectedRef)
{
	if (!selectedRef)
		return;

	Gtk::TreeRow selectedRow{*notesStore->get_iter(selectedRef.get_path())};
	Glib::ustring name = selectedRow[noteNameColumn];
	Note::deleteNote(name, noteDir());
	eraseNoteWithName(name);
}

void TypingTestWindow::loadNote(const std::string &name)
{
	std::string path{noteDir() + "/" + name};
	try {
		Note note{Note::readNote(path)};
		dialogNoteBuffer->setAddNewlines(false);
		dialogNoteBuffer->set_text(note.getContents());
		dialogNoteBuffer->setAddNewlines(true);
		/* dialogNoteBuffer->substituteStickers(); */
		// dialogNoteBuffer->replaceWords();
		noteNameEntry->set_text(note.getName());
		isEditingNote = true;
		editedNoteName = note.getName();
	} catch (std::runtime_error) {
	}
}

void TypingTestWindow::onNotesViewRowActivated(const Gtk::TreePath &path,
	Gtk::TreeViewColumn *)
{
	Gtk::TreeRow selectedRow{*notesStore->get_iter(path)};
	Glib::ustring name{selectedRow[noteNameColumn]};
	loadNote(name);
}

void TypingTestWindow::onLoadNoteItemActivated(
	Gtk::TreeRowReference selectedRef)
{
	if (!selectedRef)
		return;

	Gtk::TreeRow selectedRow{*notesStore->get_iter(selectedRef.get_path())};
	Glib::ustring noteName{selectedRow[noteNameColumn]};
	loadNote(noteName);
}

bool TypingTestWindow::eraseNoteWithName(const std::string &name)
{
	for (auto &iter : notesStore->children()) {
		Gtk::TreeRow row = *iter;
		if (row[noteNameColumn] == name) {
			notesStore->erase(iter);
			return true;
		}
	}
	return false;
}

void TypingTestWindow::clearNotesDialog()
{
	noteNameEntry->set_text("");
	dialogNoteBuffer->set_text("");
	isEditingNote = false;
	editedNoteName = "";
}

void TypingTestWindow::checkWord()
{
	std::string word = typingEntry->get_text();
	word.erase(std::remove(word.begin(), word.end(), ' '), word.end());
	typingEntry->set_text("");
	textBuffer->remove_tag_by_name("current",
		textBuffer->get_iter_at_offset(wordCharIndex),
		textBuffer->get_iter_at_offset(wordCharIndex
			+ words[wordIndex]->getWord().length()));
	textBuffer->remove_tag_by_name("currenterror",
		textBuffer->get_iter_at_offset(wordCharIndex),
		textBuffer->get_iter_at_offset(wordCharIndex +
			words[wordIndex]->getWord().length()));

	bool correct = words[wordIndex]->enterWord(word);
	if (config.hlMode != HighlightMode::NONE) {
		if (correct)
			textBuffer->apply_tag_by_name("good",
				textBuffer->get_iter_at_offset(wordCharIndex),
				textBuffer->get_iter_at_offset(wordCharIndex +
					words[wordIndex]->getWord().length()));
		else
			textBuffer->apply_tag_by_name("error",
				textBuffer->get_iter_at_offset(wordCharIndex),
				textBuffer->get_iter_at_offset(wordCharIndex
					+ words[wordIndex]->getWord().length()));
	}
	wordCharIndex += words[wordIndex]->getWord().length() + 1;
	++wordIndex;
	if (config.hlMode != HighlightMode::ALL)
		textBuffer->apply_tag_by_name("current",
			textBuffer->get_iter_at_offset(wordCharIndex),
			textBuffer->get_iter_at_offset(wordCharIndex
				+ words[wordIndex]->getWord().length()));
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
}

void TypingTestWindow::applyHighlight()
{
	std::string text = typingEntry->get_text();
	std::string word = words[wordIndex]->getWord();
	if (text.length() <= word.length()
		&& text == word.substr(0, text.length())) {
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
} // namespace typingtest
