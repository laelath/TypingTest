#ifndef CONFIG_H
#define CONFIG_H

#include <string>

extern struct Config {
	int startWords = 500;
	double wordWrongWeight = 0.4;
	double minZScore = -2.0;
	double maxZScore = 0.0;
	int startTroubleScore = 3;
	int troubleDec = 1;
	int troubleInc = 1;
} config;

extern std::string config_dir;
extern std::string data_dir;

void loadConfig();
void saveConfig();

void getPaths();

#endif
