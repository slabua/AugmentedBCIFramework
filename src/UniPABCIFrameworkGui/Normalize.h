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

#ifndef NORMALIZE_H
#define NORMALIZE_H


// Classe che normalizza un segnale
class Normalize{
	public:
		Normalize(float**,int,int,int);
		~Normalize();
		void train();			// Metodo che calcola media e deviazione standard
		float **apply();		// Metodo che applica le statistiche al segnale

	private:
		int numberOfChannels;	// Dimensione dei vettori
		int numberOfTrials;		// Numero di trial
		int numberOfSamples;	// Numero di campioni
		float **nMatrix;		// Matrice su cui operare
		float *meanValues;		// Media di ogni canale
		float *stdValues;		// Deviazione standard di ogni canale
};

#endif