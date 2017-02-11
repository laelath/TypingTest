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

#include "config.h"

#include <sys/stat.h>

#include <iostream>
#include <cstdlib>
#include <glibmm.h>
#include <glibmm/keyfile.h>

namespace typingtest {

HighlightMode getHighlight(std::string str)
{
	if (str == "Some") return HighlightMode::SOME;
	else if (str == "None") return HighlightMode::NONE;
	else return HighlightMode::ALL;
}

std::string getString(HighlightMode hl)
{
	if (hl == HighlightMode::SOME) return "Some";
	else if (hl == HighlightMode::NONE) return "None";
	else return "All";
}

Config::Config()
{
	setPaths();
}

void Config::loadConfig()
{
	Glib::KeyFile settingsFile;
	try {
		if (settingsFile.load_from_file(configDir + "typingtest.conf")) {
			startWords = settingsFile.get_integer("Test Setup", "start_words");
			minZScore = settingsFile.get_double("Trouble Words", "min_zscore");
			maxZScore = settingsFile.get_double("Trouble Words", "max_zscore");
			startTroubleScore = settingsFile.get_integer("Trouble Words",
					"start_trouble_score");
			troubleDec = settingsFile.get_integer("Trouble Words",
					"trouble_dec");
			troubleInc = settingsFile.get_integer("Trouble Words",
					"trouble_inc");
			hlMode = getHighlight(settingsFile.get_string("Highlights",
						"highlight_mode"));
		} else {
			saveConfig();
		}
	} catch (Glib::FileError) {
		saveConfig();
	} catch (Glib::KeyFileError) {
		saveConfig();
	}
}

void Config::saveConfig()
{
	Glib::KeyFile settingsFile;
	settingsFile.set_integer("Test Setup", "start_words", startWords);
	settingsFile.set_double("Trouble Words", "min_zscore", minZScore);
	settingsFile.set_double("Trouble Words", "max_zscore", maxZScore);
	settingsFile.set_integer("Trouble Words", "start_trouble_score",
			startTroubleScore);
	settingsFile.set_integer("Trouble Words", "trouble_dec", troubleDec);
	settingsFile.set_integer("Trouble Words", "trouble_inc", troubleInc);
	settingsFile.set_string("Highlights", "highlight_mode", getString(hlMode));
	settingsFile.save_to_file(configDir + "typingtest.conf");
}

void Config::setPaths()
{
	configDir = Glib::get_user_config_dir() + "/typingtest/";
	dataDir = Glib::get_user_data_dir() + "/typingtest/";

#ifdef __linux__
	mkdir(configDir.c_str(), 0755);
	mkdir(dataDir.c_str(), 0755);
#elif _WIN32
	mkdir(configDir.c_str());
	mkdir(dataDir.c_str());
#endif
}
} // namespace typingtest
