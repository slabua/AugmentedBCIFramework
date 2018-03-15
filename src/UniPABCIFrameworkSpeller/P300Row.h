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

#ifndef P300ROW_H
#define P300ROW_H

#include "P300Square.h"

// Classe che definisce una riga della matrice di interfaccia. Al suo interno ciascuna riga contiene un 
// determinato numero di oggetti Square (caselle)

// Uso una costante per indicare il numero massimo di righe e colonne supportate
const int dim = 10;


class P300Row {
public:
	P300Row();						// Costruttore vuoto
	P300Row(int);					// Costruttore con numero di quadrati
	~P300Row();						// Distruttore
	void setNumberSquare(int);		// Metodo set
	int getNumberSquare();			// Metodo get
	P300Square* getSquare(int);		// Metodo get
	bool addSquare(P300Square*);	// Metodo set
private:
	int numberSquare;			// Numero di quadrati
	P300Square* square[dim];	// Quadrati
};

#endif

