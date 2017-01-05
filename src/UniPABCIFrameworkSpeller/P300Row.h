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