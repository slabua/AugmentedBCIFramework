#ifndef OFFLINE_SPELLER_H
#define OFFLINE_SPELLER_H

#include <string>
#include "Classification.h"

using std::string;


// Classe che calcola il numero ottimale di flash per il soggetto che ha calibrato il sistema
class OfflineSpeller{
	public:
		OfflineSpeller(int,int,int,int,int,int,int,bool,int,string);
		//void configBuffer(vector<int>);
		int calculateNumberOfFlash();
		int calculateNumberOfFlashCalibAnalisys();
		int calculateNumberOfFlashOnline(int);
		int mappingNumbers(int, int);
		~OfflineSpeller();
	private:
		int optimalFlash;
		int numberOfFlash;
		int numberOfTag;
		int numberOfCalibrationTags;
		int numberOfChannels;
		int sampledWindow;   // 50
		int numberOfSessions;
		int sampleRate;
		bool flashingMode;
		int rowsNumber;
		float **buffers;
		float *w;
		int *stimuli;
		int *labels;
		int *target;
		string username;
		Classification *classification;
};

#endif