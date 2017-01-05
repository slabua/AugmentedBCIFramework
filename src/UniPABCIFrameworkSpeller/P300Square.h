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