#ifndef CONFIG_H
#define CONFIG_H

extern struct Config {
	int startWords = 500;
	double wordWrongWeight = 0.4;
	double minZScore = -2.0;
	double maxZScore = 0.0;
	int startTroubleScore = 3;
	int troubleDec = 1;
	int troubleInc = 1;
} config;

void loadConfig();
void saveConfig();

#endif
