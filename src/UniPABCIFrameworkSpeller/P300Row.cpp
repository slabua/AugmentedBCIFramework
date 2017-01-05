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
