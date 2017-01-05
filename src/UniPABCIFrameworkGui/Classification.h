#ifndef CLASSIFICATION_H
#define CLASSIFICATION_H

#include <string>

using std::string;


// Classe per la classificazione
class Classification{
	public:
		Classification(float**, int*, int, int, int, int, int, int, int, int, bool, string);
		float** reshape(float**);			// Funzione che effettua il reshape del segnale
		float** invReshape(float**);		// Funzione che effettua il reshape inverso del segnale
		void weighSignal(bool);				// Funzione che effettua il prodotto w*buffer						
		int* classify();					// Funzionc che classifica il segnale in ID
		int* flashVector();					// Funzione che ritorna il vettore con i tag predetti per ogni flash
		void vectorXmatrix(int, int,float**);
		void readWfromFile();
		//void weighSignalFromFile(); // SLB not necessary, never used in originalcd
	private:
		float **signal;			// Segnale in input
		float *w;				// Vettore dei pesi calcolati da classificazione
		int *stimuli;			// Vettore degli stimoli (tag)
		int n_runs;				// Numero di classificazioni da fare
		int n_tag;				// Numero di tag
		int n_flash;			// Numero di flash
		int n_channels;			// Numero di canali
		int n_samples;			// Numero di campioni
		int n_trials;			// Numero di trials
		int sampleRate;			// Fattore di riduzione del sample rate, se passiamo da 256 a 64, il fattore sarà 4
		int windowLen;			// Dimensione della finestra di acquisizione
		bool mode;				// Modalità d'esecuzione, true = RC; false = Square
		std::string username;
		float* m;				// Vettore dei pesi dopo il prodotto w*buffer
		int size;				// SLB
};

#endif