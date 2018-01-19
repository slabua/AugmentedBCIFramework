/*****************************************************************************
* 
* UniPA BCI Framework: an Augmented BCI Framework
* 
* Copyright (C) 2016-2017 Salvatore La Bua (slabua@gmail.com)
* RoboticsLab, Department of Industrial and Digital Innovation (DIID),
* Universita' degli Studi di Palermo, V. delle Scienze, Palermo, Italy.
* 
* http://www.slblabs.com/projects/unipa-bci-framework
* https://github.com/slabua/UniPABCIFramework
* 
* Based on a previous work -P300 core- of (in alphabetical order):
* Rosario Misuraca, Walter Tranchina, Giuseppe Trubia.
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

#ifndef BAYESIANLDA_H
#define BAYESIANLDA_H

#include <string>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>

using std::string;


//  Classe che implementa il classificatore LDA
class BayesianLDA {
	public:
		//BayesianLDA(int, int, int); // SLB not necessary, never used, must've been a test function
		BayesianLDA(int, int, int, float**, int*, int*, string);
		void compute_eigenvectors_and_eigenvalues();							// Metodo che calcola gli autovettori e gli autovalori 
		gsl_vector_float* vectorXmatrix(gsl_vector_float*, gsl_matrix_float*);  // Metodo che effettua il prodotto vettoreXmatrice
		gsl_vector_float* matrixXvector(gsl_matrix_float*,gsl_vector_float*);	// Metodo che effettua il prodotto matriceXvettore
		gsl_matrix_float* matrixXmatrix(gsl_matrix_float*,gsl_matrix_float*);	// Metodo che effettua il prodotto matriceXmatrice di float
		gsl_matrix* matrixXtmatrix(gsl_matrix_float*);					        // Metodo che effettua il prodotto matriceXmatrice di float
		gsl_matrix* matrixXmatrix(gsl_matrix*,gsl_matrix*);						// Metodo che effettua il prodotto matriceXmatrice 
		gsl_vector_float* vecXconst(gsl_vector_float*,double);					// Metodo che effettua il prodotto vettoreXcostante
		gsl_vector_float* sumVector(gsl_vector_float*,gsl_vector_float*);		// Metodo che effettua la somma vettore+vettore
		void printvec(gsl_vector_float*,string);	
		void printmatrix(gsl_matrix_float*,string);
		void printvectorintofile(gsl_vector_float*,string);
		void printmatrixintofile(gsl_matrix_float*,string);
		void train();															// Metodo che addestra la classe calcolando il vettore dei pesi w
		void compute_regression_targets();										// Metodo che modifica il vettore delle label
		void compute_vxy();														// Metodo che calcola il prodotto tra matrice degli autovettori, x e y
		void compute_w();														// Metodo che calcola il vettore dei pesi
		void compute_err();														// Metodo che calcola l'errore commesso
		void compute_gamma();													// Metodo che calcola il valore gamma
		void compute_alpha(float);												// Metodo che aggiorna il valore del parametro alpha e d_alpha
		void compute_beta(float);												// Metodo che aggiorna il valore del parametro beta e d_beta
		gsl_matrix* inverse(gsl_matrix*);										// Metodo che calcola l'inversa di una matrice
		//void readInputXfromfile(); // SLB not necessary, never used, must've been a test function
		//void readInputYfromfile(); // SLB not necessary, never used, must've been a test function
		gsl_matrix_float* get_evec();			
		gsl_vector_float* get_eval();
		double get_alpha();
		double get_beta();
		double get_dalpha();
		double get_dbeta();
		double get_err();
		double get_gamma();
		double get_iteration();
		gsl_vector_float* get_eigenvalues();									
		gsl_matrix_float* get_eigenvectors();
		gsl_vector_float* get_w();
		gsl_vector_float* get_vxy();
	private:
		int n_channels;															// Numero di canali
		int n_samples;															// Numero di campioni estratti per ciascun tag
		int n_trial;															// Numero di tag
		int n_feature;															// Numero di canali*numero di campioni
		gsl_matrix_float *x;													// Matrice dei campioni estratti dalla BCI
		gsl_vector_int *yi;														// Vettore delle label
		gsl_vector_float *y;													// Vettore delle label in formato float
		gsl_vector_int *stimuli;												// Vettore degli stimoli
		gsl_vector_float *eval;													// Vettore degli autovalori
		gsl_matrix_float *evec;													// Vettore degli autovettori
		gsl_vector_float *w;													// Vettore dei pesi
		float d_beta;															// Differenza tra nuovo e vecchio valore di alpha 
		float d_alpha;														    // Differenza tra nuovo e vecchio valore di beta
		float alpha;															// (initial) inverse variance of prior distribution
		float biasalpha;														// (initial) inverse variance of prior for bias term
		float beta;																// (initial) inverse variance around targets
		float stopeps;															// Precisione desiderata per i valori alpha e beta
		double ntot;															// Numero totale di label
		float err;																// Errore commesso 
		float gamma;
		int iteration;															// Numero di iterazioni eseguite per calcolare i parametri
		int maxit;																// Numero massimo di iterazioni
		gsl_vector_float* e;													// Vettore di uni con ultimo termine pari al valore di biasalpha
		gsl_vector_float* vxy;													// Prodotto tra matrice degli autovettori, x e y
		string username;
};


#endif