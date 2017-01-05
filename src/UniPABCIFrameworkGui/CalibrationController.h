#ifndef CALIBRATIONCONTROLLER_H
#define CALIBRATIONCONTROLLER_H

#define WIN32_LEAN_AND_MEAN

#define SERVICE_CHANNELS 3

#include <afxmt.h>
#include <string>
#include <vector>
#include <fstream>
#include "BayesianLDA.h"

using std::string;
using std::ofstream;


class CalibrationController {
	public:
		CalibrationController(int numCh, int windLen, int NumCalibTag, int numFlash, int tags, int sampleRate, bool flashing, int rowInterface, string user);
		void fillBuffer(float*);					// Metodo che implementa il riempimento del buffer
		void downSampling();						// Metodo che implementa il downsampling del buffer 
		void nextCalibration();						// Metodo che resetta il buffer e le variabili utilizzate per avviare la nuova sessione di calibrazione (DA IMPLEMENTARE)
		void LDAelaboration();						// Metodo che avvia la classificazione dopo aver applicato windsorizzazione e normalizzazione
		void LDAcalibAnalysis(int);
		float** reshape(float**);
		float** invReshape(float**);
		int* mappingIDtoRC(int);
		void setNumberOfChannels(int);				// SLB never used?
		void setWindowLen(int);
		void setSampleRata(int);
		void setNumberOfCalibrationTag(int);
		void setNumberOfFlash(int);
		void setIdTag(int*);
		int getNumberOfSession();
		long getNumSecondsRunning();
		int getNumberOfCalibration();
		void sendToInterface(int);
		void openBinFile();
		void openGameBinFile();						// SLB
		void openGameTrialBinFile();				// SLB
		void openBaselineBinFile();					// SLB
		void readConfiguration();					// SLB
		void updateConfiguration(string);			// SLB
		void writeBinFile(float*);
		void writeRawBinFile(float*);				// SLB
		void writeRawTrialBinFile(float*);			// SLB
		void closeBinFile();
		void closeBaselineBinFile();				// SLB
		void closeGameTrialBinFile();				// SLB
		int concCalibrations();
		CEvent& getFinishStoreSingleTargetDataEvent();
		CEvent& getFinishStoreAllSessionDataEvent();
	private:
		float **P300CalibrationBuffer;				// Matrice di float [num_channels*window_len, tags*num_flash*num_calibration_tags] (si reinizializza ad ogni sessione di cal)
		float **P300CalibrationBufferDS;	        // Matrice di float [(num_channels*window_len)/sampleRate, tags*num_flash*num_calibration_tags]
		int *stimuli;								// Vettore degli stimoli per il classificatore
		int *labels;								// Vettore delle labels per il classificatore
		bool *openBuffer;							// Vettore di booleani per la gestione del salvataggio dei dati nel buffer
		int *buffersSize;							// Vettore di interi che indica la dimensione corrente di ciascuna colonna di P300CalibrationBuffer
		int tags;									// Numero di tag presenti nell'interfaccia
		int *target;								// Vettore dei target per la singola sessione di calibrazione
		int currentCalibrationTag;					// Indice del tag di calibrazione corrente
	    int sampleRate;								// Fattore di downsampling 
		int numberOfChannels;						// Numero di canali
		int windowLen;								// Dimensione della finestra di valori memorizzati per ciascun tag (num_samples)
		int numberOfFlash;						    // Numero di flash per tag dell'interfaccia
		int numberOfCalibrationTag;				    // Numero di caratteri da far flashare per la singola sessione di calibrazione
		int numberOfCalibrations;					// Numero di calibrazioni
		bool flashingMode;
		std::string username;
		int numberOfRowInterface;
		int buffered_tag;							// SLB
		bool buffered_tag_flag;						// SLB
		int stimuli_length;							// SLB
		int samplerate;								// SLB
		int timeflash;								// SLB
		int number_of_channels_plus;				// SLB
		std::vector <int> targets;					// SLB
		CEvent _finishStoreSingleTargetData;		// Evento che indica il completamento del buffer per singolo target di calibrazione (riempimento di tags*num_flash colonne)
		CEvent _finishStoreAllSessionData;			// Evento che indica il completamento del buffer per l'intera sessione di calibrazione (riempimento di tags*num_flash*numCalTag colonne)
		CFile outputFile;
		CFile outputTrialFile;						// SLB
		ofstream tempBuffer;
		ofstream tempStimuli;
		ofstream tempLabel;
		BayesianLDA *classifier;
};

#endif