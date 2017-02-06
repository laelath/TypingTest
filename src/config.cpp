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

#include <err.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <glibmm.h>

namespace typingtest {

Config::Config()
{
	setPaths();
}

void Config::loadConfig()
{
	std::ifstream reader(configDir + "config.cfg");
	if (!reader.is_open()) {
		saveConfig();
		return;
	}

	std::string line;
	while(std::getline(reader, line))
		processLine(line);
	reader.close();
}

void Config::processLine(const std::string& line)
{
	std::string::size_type splitPosition = line.find("=");
	if (splitPosition == std::string::npos)
		return;
	std::string var = line.substr(0, line.find("="));
	std::string param = line.substr(line.find("=") + 1);
	try {
		if (var == "start_words")
			startWords = std::stoi(param);
		else if (var == "min_zscore")
			minZScore = std::stod(param);
		else if (var == "max_zscore")
			maxZScore = std::stod(param);
		else if (var == "start_trouble_words")
			startTroubleScore = std::stoi(param);
		else if (var == "trouble_dec")
			troubleDec = std::stoi(param);
		else if (var == "trouble_inc")
			troubleInc = std::stoi(param);
		else if (var == "word_wrong_mult")
			wordWrongWeight = std::stod(param);
	} catch (const std::invalid_argument& e) {
	}
}

void Config::saveConfig()
{
	std::ofstream writer(configDir + ".config.cfg.swp", std::ios::trunc);
	if (!writer.is_open())
		errx(EXIT_FAILURE, NULL);

	writer << "start_words=" << startWords << "\n";
	writer << "min_zscore=" << minZScore << "\n";
	writer << "max_zscore=" << maxZScore << "\n";
	writer << "start_trouble_words=" << startTroubleScore << "\n";
	writer << "trouble_dec=" << troubleDec << "\n";
	writer << "trouble_inc=" << troubleInc << "\n";
	writer << "word_wrong_mult=" << wordWrongWeight << "\n";

	std::remove((configDir + "config.cfg").c_str());
	std::rename((configDir + ".config.cfg.swp").c_str(),
		(configDir + "config.cfg").c_str());
	writer.close();
}

void Config::setPaths()
{
	configDir = Glib::get_user_config_dir();
	mkdir(configDir.c_str(), 0755);

	dataDir = Glib::get_user_data_dir();
	mkdir(dataDir.c_str(), 0755);
}
} // namespace typingtest
