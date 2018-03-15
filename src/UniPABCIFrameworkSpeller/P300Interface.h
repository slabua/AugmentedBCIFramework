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

#ifndef P300INTERFACE_H
#define P300INTERFACE_H

#include <string>
#include "P300Row.h"

using std::string;


// Classe che gestisce l'intera interfaccia grafica. Possiede attributi per la visualizzazione e metodi relativi
const int dim_i = 10; // Costante che definisce il numero massimo di righe e colonne supportate da quest'applicazione


class P300Interface {
	public:
		P300Interface();					// Costruttore vuoto
		P300Interface(int);					// Costruttore con solo il numero di elementi
		P300Interface(int, int, int, int, int, int, string);	// Costruttore completo
		~P300Interface();					// Distruttore
		void setNumberEl(int);				// Metodo set
		bool addRow(P300Row*);				// Metodo set
		void setWindowWidth(int);			// Metodo set
		void setWindowHeight(int);			// Metodo set
		void setFlashFrequency(int);		// Metodo set
		void setTexturePathName(string);	// Metodo set
		void setNumberOfFlash(int);			// Metodo set
		void setMaxSessions(int);			// Metodo set
		int getNumberEl();					// Metodo get
		P300Row* getRow(int);				// Metodo get
		int getWindowWidth();				// Metodo get
		int getWindowHeight();				// Metodo get
		int getFlashFrequency();			// Metodo get
		string getTexturePathName();		// Metodo get
		int getNumberOfFlash();				// Metodo get
		int getMaxSessions();				// Metodo get
	private:
		int numberEl;			// Numero di elementi nell'interfaccia
		P300Row* row[dim_i];	// Righe di quadrati
		int windowWidth;		// Larghezza della finestra
		int windowHeight;		// Altezza della finestra
		int flashFrequency;		// Frequenza di flash
		int numberOfFlash;      // Numero di flash per sessione 
		int maxSessions;        // Numero di sessioni
		string texturePathName; // Path delle texture
};
#endif

