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