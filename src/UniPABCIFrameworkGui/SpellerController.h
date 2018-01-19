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

#ifndef SPELLERCONTROLLER_H
#define SPELLERCONTROLLER_H

#define SERVICE_CHANNELS	3
#define FILETYPE_SPELLING	-1

#include <afxmt.h>
#include <iostream>

using std::string;


class SpellerController {
	public:
		SpellerController(bool, int, int, int, int, int, string);
		void fillBuffer(float*);							// Funzione di acquisizione principale
		void downSampling();								// Funzione che effettua il downsampling dei dati di un fattore sampleRate
		void reshape();										// Funzione che effettua il reshape della matrice da (canali*finestraSottocampionata X numeroDiTag*numeroDiFlash) a (canali X finestraSottocampionata*numeroDiTag*numeroDiFlash)
		void invReshape();									// Funzione che effettua il reshape inverso
		//void printFile();									// Funzione ausiliaria per stampare su file
		int* prediction();
		void nextPrediction();								// Funzione che resetta le variabili utilizzate per il nuovo tag da predire
		void openBinFile();
		void readConfiguration();						// SLB
		void updateConfiguration(string);				// SLB
		void writeBinFile(float*);
		void closeBinFile();
		CEvent& getFinishedWriteBufferEvent();
	private:
		bool flashingMode;
		int size;										// dimensione della matrice (num righe)
		int tags;										// numero di tag
		int numberOfFlash;								// numero di flash
		int numberOfChannels;							// numero di canali
		int windowLen;									// dimensione della finestra
		int sampleRate;									// frequenza di sottocampionamento
		int n_samples;
		int n_trials;
		string username;
		CEvent _finishedWriteBuffer;					// evento che segnala il termine di scrittura sul buffer
		float **P300SpellerBuffer;						// buffer che memorizza i dati
		float **P300SpellerBufferDS;					// E' il buffer a valle del downsampling
		float **P300SpellerBufferReshaped;				// E' il buffer a valle del reshape
		bool *openBuffer;								// vettore di booleani che indica l'apertura o meno di un buffer
		int *buffersSize;								// Vettore di interi che indica la dimensione attuale di ciascuna colonna di P300SpellerBuffer
		int *stimuli;									// Vettore dei tag
		int buffered_tag;								// SLB
		bool buffered_tag_flag;							// SLB
		int stimuli_length;								// SLB
		int samplerate;									// SLB
		int number_of_channels_plus;					// SLB
		int timeflash;									// SLB
		CFile outputFile;
};

#endif