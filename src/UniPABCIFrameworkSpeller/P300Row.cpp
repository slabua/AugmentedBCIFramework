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

#include "P300Row.h"


P300Row::P300Row() {
	setNumberSquare(-1);
	for (int i=0; i<dim; i++)
		square[i] = NULL;
}

P300Row::P300Row(int number) {
	setNumberSquare(number);
	for (int i=0; i<dim; i++)
		square[i] = NULL;
}

P300Row::~P300Row() {
	for (int i=0; i<dim; i++)
		delete square[i];
}

void P300Row::setNumberSquare(int number) {
	numberSquare = number;
}

int P300Row::getNumberSquare() {
	return numberSquare;
}

P300Square* P300Row::getSquare(int index) {
	return square[index];
}

bool P300Row::addSquare(P300Square *r_square) {
	if (numberSquare == -1) {
		setNumberSquare(0);
		square[numberSquare] = r_square;
	} else if ((numberSquare != -1)&&(numberSquare<dim)) {
		for (int i=0; i<numberSquare; i++)
		if (square[i] == NULL) {
			square[i] = r_square;
			numberSquare = i;

			return true;			
		}		
	}
	return false;
}
