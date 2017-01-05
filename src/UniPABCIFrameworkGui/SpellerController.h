#ifndef SPELLERCONTROLLER_H
#define SPELLERCONTROLLER_H

#define SERVICE_CHANNELS	3
#define FILETYPE_SPELLING	-1

#include <afxmt.h>
#include <iostream>

using std::string;


class SpellerController {
	public:
		SpellerController(bool, int, int, int, int, int, string);
		void fillBuffer(float*);							// Funzione di acquisizione principale
		void downSampling();								// Funzione che effettua il downsampling dei dati di un fattore sampleRate
		void reshape();										// Funzione che effettua il reshape della matrice da (canali*finestraSottocampionata X numeroDiTag*numeroDiFlash) a (canali X finestraSottocampionata*numeroDiTag*numeroDiFlash)
		void invReshape();									// Funzione che effettua il reshape inverso
		//void printFile();									// Funzione ausiliaria per stampare su file
		int* prediction();
		void nextPrediction();								// Funzione che resetta le variabili utilizzate per il nuovo tag da predire
		void openBinFile();
		void readConfiguration();						// SLB
		void updateConfiguration(string);				// SLB
		void writeBinFile(float*);
		void closeBinFile();
		CEvent& getFinishedWriteBufferEvent();
	private:
		bool flashingMode;
		int size;										// dimensione della matrice (num righe)
		int tags;										// numero di tag
		int numberOfFlash;								// numero di flash
		int numberOfChannels;							// numero di canali
		int windowLen;									// dimensione della finestra
		int sampleRate;									// frequenza di sottocampionamento
		int n_samples;
		int n_trials;
		string username;
		CEvent _finishedWriteBuffer;					// evento che segnala il termine di scrittura sul buffer
		float **P300SpellerBuffer;						// buffer che memorizza i dati
		float **P300SpellerBufferDS;					// E' il buffer a valle del downsampling
		float **P300SpellerBufferReshaped;				// E' il buffer a valle del reshape
		bool *openBuffer;								// vettore di booleani che indica l'apertura o meno di un buffer
		int *buffersSize;								// Vettore di interi che indica la dimensione attuale di ciascuna colonna di P300SpellerBuffer
		int *stimuli;									// Vettore dei tag
		int buffered_tag;								// SLB
		bool buffered_tag_flag;							// SLB
		int stimuli_length;								// SLB
		int samplerate;									// SLB
		int number_of_channels_plus;					// SLB
		int timeflash;									// SLB
		CFile outputFile;
};

#endif