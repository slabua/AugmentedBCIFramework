#ifndef NORMALIZE_H
#define NORMALIZE_H


// Classe che normalizza un segnale
class Normalize{
	public:
		Normalize(float**,int,int,int);
		~Normalize();
		void train();			// Metodo che calcola media e deviazione standard
		float **apply();		// Metodo che applica le statistiche al segnale

	private:
		int numberOfChannels;	// Dimensione dei vettori
		int numberOfTrials;		// Numero di trial
		int numberOfSamples;	// Numero di campioni
		float **nMatrix;		// Matrice su cui operare
		float *meanValues;		// Media di ogni canale
		float *stdValues;		// Deviazione standard di ogni canale
};

#endif