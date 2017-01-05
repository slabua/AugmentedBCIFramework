#ifndef WINSOR_H
#define WINSOR_H


//  Classe che applica la winsorizzazione al segnale in input
class Winsor{
	public:
		Winsor(float**,float,int,int,int);
		~Winsor();
		void train();			// Metodo che addestra la classe
		float **apply();		// Metodo che applica la winsorizzazione e ritorna la matrice
	private:		
		float p;				// Percentuale di campioni da clippare per canale
		int numberOfChannels;	// Dimensione dei vettori
		int numberOfTrials;		// Numero di trial
		int numberOfSamples;	// Numero di campioni
		float *limit_l;			// Limite inferiore
		float *limit_h;			// Limite superiore
		float **wMatrix;		// Matrice su cui operare
		int clip;				// Valore di clipping
};

#endif
