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

#ifndef WINSOR_H
#define WINSOR_H


//  Classe che applica la winsorizzazione al segnale in input
class Winsor{
	public:
		Winsor(float**,float,int,int,int);
		~Winsor();
		void train();			// Metodo che addestra la classe
		float **apply();		// Metodo che applica la winsorizzazione e ritorna la matrice
	private:		
		float p;				// Percentuale di campioni da clippare per canale
		int numberOfChannels;	// Dimensione dei vettori
		int numberOfTrials;		// Numero di trial
		int numberOfSamples;	// Numero di campioni
		float *limit_l;			// Limite inferiore
		float *limit_h;			// Limite superiore
		float **wMatrix;		// Matrice su cui operare
		int clip;				// Valore di clipping
};

#endif

