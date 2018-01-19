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

#ifndef P300SQUARE_H
#define P300SQUARE_H

#include <string>
#include "glut.h"

// Classe che definisce una singola casella dell'interfaccia. La classe è corredata da costruttore, distruttore, metodi set e get.

using std::string;


class P300Square{
	public:
		P300Square();							// Costruttore vuoto
		P300Square(int);						// Costruttore con ID
		P300Square(int, GLfloat, GLfloat, GLfloat, string, string);		// Costruttore completo
		~P300Square();							// Distruttore
		void setID(int);						// Metodo set
		void setU(GLfloat);						// Metodo set
		void setV(GLfloat);						// Metodo set
		void setSize(GLfloat);					// Metodo set
		void setTexturePath(string);			// Metodo set
		void setTextureFlashPath(string);		// Metodo set
		int getID();							// Metodo get
		GLfloat getU();							// Metodo get
		GLfloat getV();							// Metodo get
		GLfloat getSize();						// Metodo get
		string getTexturePath();				// Metodo get
		string getTextureFlashPath();			// Metodo get
		void setTexture(bool);					// Setta la texture e la rende visibile nel canvas

	private:
		int ID;									// ID del quadrato
		GLfloat u, v, size;						// Dimensione e coordinate
		string texturePath, textureFlashPath;	// Path delle texture normali e flashate
};

#endif