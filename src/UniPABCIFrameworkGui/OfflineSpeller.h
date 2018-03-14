/*****************************************************************************
* 
* UniPA BCI Framework: an Augmented BCI Framework
* 
* Copyright (C) 2016-2017 Salvatore La Bua (slabua@gmail.com)
* RoboticsLab, Department of Industrial and Digital Innovation (DIID),
* Universita' degli Studi di Palermo, V. delle Scienze, Palermo, Italy.
* 
* http://www.slblabs.com/projects/unipa-bci-framework
* https://github.com/slabua/UniPABCIFramework
* 
* Based on a previous work -P300 core- of (in alphabetical order):
* Rosario Misuraca, Walter Tranchina, Giuseppe Trubia.
* 
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* 
*****************************************************************************/

#ifndef OFFLINE_SPELLER_H
#define OFFLINE_SPELLER_H

#include <string>
#include "Classification.h"

using std::string;


// Classe che calcola il numero ottimale di flash per il soggetto che ha calibrato il sistema
class OfflineSpeller{
	public:
		OfflineSpeller(int,int,int,int,int,int,int,bool,int,string);
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

