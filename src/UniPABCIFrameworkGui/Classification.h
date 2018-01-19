/*****************************************************************************
* 
* UniPA BCI Framework: an Augmented BCI Framework
* 
* Copyright (C) 2016-2017 Salvatore La Bua (slabua@gmail.com)
* RoboticsLab, Department of Industrial and Digital Innovation (DIID),
* Universita'  degli Studi di Palermo, V. delle Scienze, Palermo, Italy.
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

#ifndef CLASSIFICATION_H
#define CLASSIFICATION_H

#include <string>

using std::string;


// Classe per la classificazione
class Classification{
	public:
		Classification(float**, int*, int, int, int, int, int, int, int, int, bool, string);
		float** reshape(float**);			// Funzione che effettua il reshape del segnale
		float** invReshape(float**);		// Funzione che effettua il reshape inverso del segnale
		void weighSignal(bool);				// Funzione che effettua il prodotto w*buffer						
		int* classify();					// Funzionc che classifica il segnale in ID
		int* flashVector();					// Funzione che ritorna il vettore con i tag predetti per ogni flash
		void vectorXmatrix(int, int,float**);
		void readWfromFile();
		//void weighSignalFromFile(); // SLB not necessary, never used in originalcd
	private:
		float **signal;			// Segnale in input
		float *w;				// Vettore dei pesi calcolati da classificazione
		int *stimuli;			// Vettore degli stimoli (tag)
		int n_runs;				// Numero di classificazioni da fare
		int n_tag;				// Numero di tag
		int n_flash;			// Numero di flash
		int n_channels;			// Numero di canali
		int n_samples;			// Numero di campioni
		int n_trials;			// Numero di trials
		int sampleRate;			// Fattore di riduzione del sample rate, se passiamo da 256 a 64, il fattore sarà 4
		int windowLen;			// Dimensione della finestra di acquisizione
		bool mode;				// Modalità d'esecuzione, true = RC; false = Square
		std::string username;
		float* m;				// Vettore dei pesi dopo il prodotto w*buffer
		int size;				// SLB
};

#endif