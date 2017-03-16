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

#include <random>
#include <set>
#include <thread>

#include <gtkmm.h>

#include "config.h"
#include "note.h"
#include "sticker_buffer.h"
#include "test_info.h"
#include "test_settings.h"
#include "typing_test.h"
#include "word.h"

#ifndef TYPING_TEST_TYPING_TEST_WINDOW_H
#define TYPING_TEST_TYPING_TEST_WINDOW_H

namespace typingtest {

// The maximum amount of tests to store in history.
constexpr int HISTORY_SIZE = 100;
// The string to put in the history view if there is a note.
constexpr char HAS_NOTE_STRING[] = "Note";
// Display nothing in the history view if there is no note in that column.
constexpr char NO_NOTE_STRING[] = "No";

// A window that is the main driver for the TypingTest program. An application
// window that the user interacts with to take typing tests.
class TypingTestWindow : public Gtk::ApplicationWindow {
public:
	// Constructor for use with Gtk::Builder.
	TypingTestWindow(BaseObjectType *cobject,
		const Glib::RefPtr<Gtk::Builder> &builder);
	// Creates a new TypingTestWindow. Can throw a Glib::FileError,
	// Gio::ResourceError, or Gtk::BuilderError.
	//
	// This uses a standard library smart pointer so that it can be
	// dereferenced which Glib smart pointers don't support. Dereferencing is
	// required for easily calling GApplication::run with a window. If someone
	// knows a better way to do this with RAII then that would be great.
	static std::shared_ptr<TypingTestWindow> create();
	static TypingTestWindow *create_pointer();

private:
	// The builder used to construct the window's children.
	Glib::RefPtr<Gtk::Builder> builder;

	// Test configuration.
	Config config;

	// The current test being taken. Contains info about the words and can be
	// used to retreive text to be used with textBuffer.
	TypingTest currentTest;

	// Initializes widgets from builder file.
	void initWidgets();
	// Connects most of the widget's signals.
	void connectSignals();

	// Main window widgets.
	Gtk::ApplicationWindow *appWindow;
	Gtk::Button *newTest;
	Gtk::ImageMenuItem *settingsItem;
	Gtk::ImageMenuItem *fontItem;
	Gtk::ImageMenuItem *advItem;
	Gtk::ImageMenuItem *quitItem;
	Gtk::ImageMenuItem *troubleItem;
	Gtk::ImageMenuItem *aboutItem;
	Gtk::Box *testBox;

	Glib::RefPtr<Gtk::TextBuffer> textBuffer;
	Glib::RefPtr<Gtk::TextTagTable> textTags;
	Glib::RefPtr<Gtk::TextTag> currentTag;
	Glib::RefPtr<Gtk::TextTag> currentErrorTag;
	Glib::RefPtr<Gtk::TextTag> errorTag;
	Glib::RefPtr<Gtk::TextTag> goodTag;
	Glib::RefPtr<Gtk::TextTag> uglyHackTag;

	Gtk::TextView *textView;
	Gtk::Entry *typingEntry;
	Gtk::Label *timerLabel;
	Gtk::Label *wpmLabel;
	Gtk::Label *wordNumLabel;
	Gtk::Label *wordsCorrectLabel;
	Gtk::Label *wordsWrongLabel;
	Gtk::Label *charNumLabel;
	Gtk::Label *charsCorrectLabel;
	Gtk::Label *charsWrongLabel;
	Gtk::Label *troubleWordsLabel;

	Gtk::Button *createNoteButton;
	Gtk::TextView *noteView;
	Gtk::Button *saveNoteButton;
	Gtk::Button *insertStickerButton;

	Glib::RefPtr<StickerBuffer> noteBuffer;

	// Settings window widgets.
	Gtk::Dialog *settingsDialog;
	Gtk::Button *cancel;
	Gtk::Button *apply;
	Gtk::ComboBoxText *testTypeBox;
	Gtk::SpinButton *topWords;
	Gtk::SpinButton *minWordLength;
	Gtk::SpinButton *maxWordLength;
	Gtk::SpinButton *testLength;
	Gtk::Entry *seedEntry;
	Gtk::Button *randomizeSeed;
	Gtk::SpinButton *personalFrequencyButton;
	Gtk::SpinButton *capitalFrequencyButton;

	// Font chooser.
	Gtk::FontChooserDialog *fontChooser;

	// Trouble words window.
	Gtk::Dialog *troubleDialog;
	Gtk::TreeView *troubleList;
	Gtk::Button *troubleClose;

	// History info widgets.
	Gtk::Dialog *historyDialog;
	Gtk::Button *historyCloseButton;
	Gtk::Button *eraseHistoryButton;
	Gtk::Label *averageSpeedLabel;
	Gtk::Label *fastestTimeLabel;
	Gtk::Label *currentFastestTimeLabel;
	Gtk::Label *currentSlowestTimeLabel;
	Gtk::Label *currentStandardDeviationLabel;
	Gtk::TreeView *testHistoryView;

	static Glib::ustring hasNoteString(bool hasNote);

	// Helper objects for history dialog.
	Glib::RefPtr<Gtk::ListStore> historyStore;
	Gtk::TreeModelColumnRecord historyColumnRecord;
	Gtk::TreeModelColumn<int> wpmColumn;
	Gtk::TreeModelColumn<Glib::ustring> lengthColumn;
	Gtk::TreeModelColumn<Glib::ustring> typeColumn;
	Gtk::TreeModelColumn<Glib::ustring> hasNoteColumn;
	Gtk::TreeModelColumn<std::shared_ptr<TestInfo>> testInfoColumn;

	// Notes dialog widgets
	Gtk::Dialog *noteDialog;
	Gtk::Button *dialogSaveNoteButton;
	Gtk::Button *dialogInsertStickerButton;
	Gtk::Button *closeNotesButton;
	Gtk::TextView *dialogNoteView;

	// Helper object for notes dialog.
	Glib::RefPtr<StickerBuffer> dialogNoteBuffer;

	// List of trouble words to be used with the trouble words display.
	Glib::RefPtr<Gtk::ListStore> troubleListStore;

	Gtk::TreeModelColumn<std::string> strCol;
	Gtk::TreeModelColumn<unsigned int> valCol;

	// Test settings
	TestSettings settings;
	std::string currFont = "Serif 25";

	// Advanced settings widgets
	Gtk::Dialog *advSettingsDialog;
	Gtk::SpinButton *startWords;
	Gtk::SpinButton *minZScore;
	Gtk::SpinButton *maxZScore;
	Gtk::SpinButton *startTroubleScore;
	Gtk::SpinButton *troubleDec;
	Gtk::SpinButton *troubleInc;
	Gtk::ComboBoxText *hlModeBox;
	Gtk::Button *restoreDefaultAdv;
	Gtk::Button *cancelAdv;
	Gtk::Button *applyAdv;

	//About dialog
	Gtk::AboutDialog *aboutDialog;

	// Returns the current time as a string.
	std::string getTime();
	// Signal to be used when text is inserted to textBuffer. It may start the
	// test if it hasn't been started yet and updates the words being typed on
	// the display.
	void textInsert(std::string text, int *pos);
	// Signal for when text is deleted from textbuffer.
	void textDelete(int pos, int num);
	// Signal handler for keeping track of test times. It is meant to be called
	// once per second by the timeout. Calculates the score if the test is
	// over.
	//
	// Returns true if another second is meant to be waited, false if the test
	// is over and the timer needs to be stopped.
	bool updateTimer();
	// Calculates the score based on the words and how many were correct and
	// incorrect.
	void calculateScore();

	// Connection for inserting text.
	sigc::connection insertConnection;
	// Connection for deleting text.
	sigc::connection backspConnection;
	// Connection that is connected to the timeout with updateTimer as a
	// callback. Meant to be cleared when the test is over and connected when
	// it starts.
	sigc::connection timerConnection;

	// Random number generator.
	std::default_random_engine rand;

	// The list of for use with the test. It is a list of smart pointers so
	// when it is copied from currentTest it copies the same addresses. This is
	// desired as doing a deep copy would be a waste of memory.
	std::vector<std::shared_ptr<Word>> words;

	std::chrono::seconds seconds;
	std::chrono::seconds start;

	int wordIndex = 0;
	int wordCharIndex = 0;
	bool testStarted = false;
	bool testEnded = false;

	// Generates a new list of words and updates textBuffer to be ready for a
	// new test.
	void genNewTest();
	void updateSettings();
	void randomSeed();
	void openSettings();
	void openFont();
	void updateAdvSettings();
	void applyDefaultSettings();
	void openAdvSettings();
	void openTroubleWords();
	void openAbout();

	// Initialize actions so that action names from glade activate correctly.
	void initActions();

	void checkWord();
	void applyHighlight();

	// Signal handlers.
	// For the history dialog close button to give the dialog response signal.
	void onHistoryCloseButtonClicked();
	void onEraseHistoryButtonClicked();
	void onHistoryDialogButtonPress(GdkEventButton *button);

	void onDialogSaveNoteButtonClicked();
	void onDialogInsertStickerButtonClicked();
	void onDialogCancelClicked();

	void onCreateNoteButtonClicked();
	bool onTypingEntryKeyPress(GdkEventKey *keyEvent);

	void onNoteDialogResponse(int responseId);

	void onHistoryRowActivated(const Gtk::TreePath &path,
		Gtk::TreeViewColumn *);
	void onHistoryOpenNote(Gtk::TreeRowReference selectedRef);

	// Opens the history dialog.
	void onActionShowHistory();

	// Assuming a score of wpm was just achieved, updates the history file to
	// reflect the new score.
	void updateHistoryFile(int wpm);
	// Adds a note to the test at the given index in the history file. Throws
	// std::out_of_range if it encounters an error.
	void addNoteToHistory(int index, const std::string &note);
	// Reads the history file, adds a note to the last one and resaves it.
	void addNoteToLastTestHistory(const std::string &note);
	// Assuming a test with troubleWords and goodWords was achieved, then
	// updates the trouble words file accordingly. Passed by value because it
	// shouldn't affect the variables used but also needs to modify a copy.
	// Copying a short list of strings shouldn't be an issue.
	void updateTroubleWordsFile(std::set<std::string> troubleWords,
		std::set<std::string> goodWords);

	bool hasNote = false;
	std::string note;

	// Reads the file given by path and returns the list of tests it
	// represents. If there was an error then an empty vector is returned and
	// recordWpm is changed to 0.
	static std::vector<TestInfo> readHistory(const std::string &path,
		int &recordWpm);
	// Returns the average wpm if size is greater than 0 and 0 otherwise.
	static double getAverageWpm(const std::vector<TestInfo> &history);
	// Returns the standard deviation if the size is greater than 0 and 0
	// otherwise.
	static double getStandardDeviation(const std::vector<TestInfo> &history);
	// Returns the maximum wpm in history or 0 if there are no elements.
	static int getMaxWpm(const std::vector<TestInfo> &history);
	// Returns the minimum wpm in history or 0 if there are no elements.
	static int getMinWpm(const std::vector<TestInfo> &history);
	// Sets all labels in the history dialog to 0 and removes all items from
	// the tree view.
	void resetHistoryDisplay();

	// Implements comparator for the WPM of TestInfo objects.
	static bool compareWpm(const TestInfo &t1, const TestInfo &t2);

	// Returns the path to use for the file storing history data.
	std::string getHistoryPath() const;
	// Lock to ensure there is not simultaneous reading and writing to the
	// history file.
	std::mutex historyFileLock;
	// Returns the path to use for the file storing history data.
	std::string getTroubleWordsPath() const;
	// Lock to ensure there is not simultaneous access to the trouble words
	// file. There is still unsynchronized access from another class, but I
	// forget which one.
	std::mutex troubleWordsFileLock;
};
} // namespace typingtest

#endif // TYPING_TEST_TYPING_TEST_WINDOW_H
