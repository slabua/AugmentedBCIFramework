#include "P300Interface.h"
#include <iostream>

using std::cout;
using std::endl;


P300Interface::P300Interface() {
	setNumberEl(-1);
	for (int i=0; i<dim_i; i++)
		row[i] = NULL;
}

P300Interface::P300Interface(int elem) {
	setNumberEl(elem);
	
	/* SLB redundant code
	int tmp = ceil(sqrt(elem));
	int rows;
	
	if (tmp <= 4)
		rows = 2;
	else if ((tmp > 4) && (tmp <= 9))
		rows = 3;
	else if ((tmp > 9) && (tmp <= 16))
		rows = 4;
	else if ((tmp > 16) && (tmp <= 25))
		rows = 5;
	else if ((tmp > 25) && (tmp <= 36))
		rows = 6;
	else if ((tmp > 36) && (tmp <= 49))
		rows = 7;
	else if ((tmp > 49) && (tmp <= 64))
		rows = 8;
	else if ((tmp > 64) && (tmp <= 81))
		rows = 9;
	else if ((tmp > 81) && (tmp <= 100))
		rows = 10;
	else {
		cout << endl << "Errore, inserito numero di elementi errato." << endl;
		exit(1);
	}
	*/

	// SLB
	int rows = ceil(sqrt(elem));
	if (rows > 100) {
		cout << endl << "Errore, inserito numero di elementi errato." << endl;
		exit(1);
	}
	//

	for (int i=0; i<rows; i++)
		row[i] = new P300Row(rows);
}

P300Interface::P300Interface(int elem, int width, int height, int freq, int flash, int sessions, string path) {
	setNumberEl(elem);
	setWindowWidth(width);
	setWindowHeight(height);
	setFlashFrequency(freq);
	setNumberOfFlash(flash);
	setMaxSessions(sessions);
	setTexturePathName(path);
}

P300Interface::~P300Interface() {
	for (int i=0; i<dim_i; i++)
		delete row[i];
	delete row;
}

void P300Interface::setNumberEl(int number) {
	numberEl = number;
}

void P300Interface::setWindowWidth(int width) {
	windowWidth = width;
}

void P300Interface::setWindowHeight(int height) {
	windowHeight = height;
}

void P300Interface::setFlashFrequency(int freq) {
	flashFrequency = freq;
}

void P300Interface::setTexturePathName(string path) {
	texturePathName = path;
}

void P300Interface::setNumberOfFlash(int flash) {
	numberOfFlash = flash;
}

void P300Interface::setMaxSessions(int sessions) {
	maxSessions = sessions;
}

int P300Interface :: getNumberEl() {
	return numberEl;
}

P300Row* P300Interface::getRow(int index) {
	return row[index];
}

bool P300Interface::addRow(P300Row *r_row) {
	if (numberEl == -1) {
		setNumberEl(0);
		row[numberEl] = r_row;
	}
	else if ((numberEl != -1) && (numberEl<dim)) {
		for (int i=0; i<numberEl; i++) {
			if (row[i] == NULL) {
				row[i] = r_row;
				numberEl = i;

				return true;
			}
		}
	}
	return false;
}

int P300Interface::getWindowWidth() {
	return windowWidth;
}

int P300Interface::getWindowHeight() {
	return windowHeight;
}

int P300Interface::getFlashFrequency() {
	return flashFrequency;
}

string P300Interface::getTexturePathName() {
	return texturePathName;
}

int P300Interface::getNumberOfFlash() {
	return numberOfFlash;
}

int P300Interface::getMaxSessions() {
	return maxSessions;
}