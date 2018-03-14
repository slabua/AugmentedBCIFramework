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

#ifndef P300FRAMEWORK_H
#define P300FRAMEWORK_H

#include <string>
#include "P300SignalAcquisition.h"
#include "InterfaceListener.h"
#include "CalibrationController.h"
#include "SpellerController.h"
#include "OfflineSpeller.h"
#include "Gui.h"				// SLB
#include "FeatureExtraction.h"	// SLB
#include "ActionSelection.h"	// SLB
#include "ResponseModulation.h"	// SLB

using std::string;

#define SERVICE_CHANNELS	3
#define ocular_filter		60


class P300Framework {
	public:
		P300Framework(Gui*);						// SLB
		void readConfiguration();					// metodo che setta alcuni parametri di classe leggendoli dal file xml di config
		void thread_initialization();				// metodo che inizializza i thread di acquisizione
		void thresholdAndScale();					// SLB
		void signalGainScale();						// SLB
		void calibration();
		void ldaClassification();
		void resetCalibration();
		void spelling();							// SLB
		void resetSpeller();
		int offlineSpellerStart();
		void sendTagtoInterface(int*);
		void sendCommandToInterface(int);
		int offlineSpellerOnStart(int);
		int CalibAnalysis();
		int getNumberOfCalibrations();
		int getInputNewFlash();
		void writeTagPredicted(int*);
		//void createStatisticsXMLfile();
		void signalCheck();							// SLB
		void gameSession();							// SLB
		void baselineAcquisitionInit();				// SLB
		void baselineAcquisition(int, int);			// SLB
		void baselineAcquisitionTerm();				// SLB
		void BaselineAnalysis();					// SLB
		void ExtractFeatures();						// SLB
		void ActionSelect();						// SLB
		void ModulateResponse();					// SLB
		string getResponse();						// SLB
		void setbaselineAcquisitionStarted(bool);	// SLB
		void runModules();							// SLB
		void runModulesGame();						// SLB
	private:
		P300SignalAcquisition* module;				// modulo di acquisizione del segnale dalla BCI
		InterfaceListener* listener;				// modulo di comunicazione con l'interfaccia
		CalibrationController* controller;			// modulo di calibrazione
		SpellerController* speller;					// modulo di spelling
		OfflineSpeller* offspeller;					// modulo di analisi per il calcolo del numero ottimale di flash
		FeatureExtraction* featuresExtractor;		// SLB Entropy/Energy/R^2
		ActionSelection* actionSelector;			// SLB
		ResponseModulation* responseModulator;		// SLB
		bool bioHybridMode;							// SLB
		bool engineenabled;							// SLB
		bool baselineAcquisitionStarted;			// SLB
		int number_of_calibration_tag;				// numero di tag per la calibrazione
		int number_of_flash;						// numero di flash per singolo tag
		int* target_of_calibration;					// vettore dei target scelto per la calibrazione
		int number_of_row_interface;				// numbero di righe dell'interfaccia
		int number_of_tags;							// numero di tag totali dell'interfaccia
		string username;							// nome dell'utente
		int counter_stimuli;						// contatore dei tag di calibrazione
		bool flashingMode;							// flag per indicare la modalita' di flashing (true=RC, false=S)
		int window_len;								// dimensione della finestra di campioni acquisiti per ogni tag
		int sampleRate;								// fattore di downsampling
		int number_of_channels;						// numero di canali per l'acquisizione
		int number_of_channels_plus;				// SLB
		int signalGain;								// SLB
		bool tobiienabled;							// SLB
		int seconds_running;						
		int buffer_size;
		int number_of_scan;
		int buffer_size_second;
		int sample_rate_hz;
		float* receivedData;
		Gui* guiObject;								// SLB
		CWinThread* _networkTagThread;
		CWinThread* _dataAcquisitionThread;
};

#endif

