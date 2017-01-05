#include "Classification.h"
#include <time.h>
#include <iostream>
#include <fstream>
#include "Winsor.h"
#include "Normalize.h"
#include "BayesianLDA.h"
#include "rootpaths.h" //SLB
#include <vector>

using std::endl;
using std::ifstream;
using std::ofstream;


// Classe per la classificazione dei segnali
Classification::Classification(float **p_signal, int *p_stimuli, int p_n_runs, int p_n_tag,
								int p_n_flash, int p_n_channels, int p_n_samples, int p_n_trials, 
								int p_sampleRate, int p_windowLen, bool p_mode, string p_user) {
	signal = p_signal;
	stimuli = p_stimuli;
	n_runs = p_n_runs;
	n_tag = p_n_tag;
	n_flash = p_n_flash;
	n_channels = p_n_channels;
	n_samples = p_n_samples;
	n_trials = p_n_trials;
	sampleRate = p_sampleRate;
	windowLen = p_windowLen;
	mode = p_mode;
	username = p_user;
	
	w = new float[n_channels*(windowLen/sampleRate)+1];
	readWfromFile();
	m = new float[n_tag*n_flash];

	size = (int) (n_channels * windowLen / sampleRate); // SLB added to global
}

float** Classification::reshape(float **oldSignal) { 

	float **reshaped;
	reshaped = new float*[n_channels];
	for (int i=0; i<n_channels; i++)
		reshaped[i] = new float[n_flash*windowLen*n_tag];

	int index = 0;
	int k;
	for (int i=0; i<n_channels; i++) {
		//index = 0;
		k = i;
		for (int index = 0; index < (n_tag*n_flash*windowLen / sampleRate); k += n_channels) {
			for (int j = 0; j < n_tag*n_flash; j++) {
				reshaped[i][index] = oldSignal[k][j];
				index++;
			}
		}
	}

	return reshaped;
}

float** Classification::invReshape(float **oldSignal) {

	float **reshaped;
	//int size = (int) (n_channels * windowLen / sampleRate); // SLB moved to global

	reshaped = new float*[size];
	for (int i=0; i<size; i++)
		reshaped[i] = new float[n_tag*n_flash];

	//int index[4] = { 0, 0, 0, 0 };
	// SLB
	std::vector <int> index;
	for (int i=0; i<n_channels; i++) {
		index.push_back(0);
	}
	//

	//for (int i=0; i<windowLen; i++) {
	for (int i=0; i<size; i++) { // SLB
		for (int j = 0; j < n_tag*n_flash; j++) {
			reshaped[i][j] = oldSignal[i % n_channels][index[i % n_channels]];
			index[i % n_channels]++;
		}
	}

	return reshaped;
}

int* Classification::classify() { // Attualmente classifica su una sola calibrazione
	Winsor *winsor;
	Normalize *normalize;
	BayesianLDA *lda;

	int *maxIndex = new int[n_runs*2];
	maxIndex[0] = 0;
	maxIndex[1] = -1;

	float **support1;
	float **support2;
	for (int i=0; i<n_runs; i++) {
		support1 = reshape(signal);  // Reshape per applicare normalizzazione e winsorizzazione

		winsor = new Winsor(support1, 0.1, n_channels, n_trials, n_samples);
		winsor->train();
		support2 = winsor->apply(); // Applico winsorizzazione

		ofstream wins;
		wins.open((outputFilesRoot + "LDA/winsor.txt").c_str()); // SLB
		//wins.open(outputFilesRoot + "LDA/winsor.txt");
		float** tempW = invReshape(support2);
		//for (int i=0; i<200; i++)
		//int size = (int) (n_channels * windowLen / sampleRate); // SLB moved to global
		for (int i=0; i<size; i++)
			for (int j=0;j<n_trials;j++)
				 wins << tempW[i][j] << endl;

		wins.close();

		normalize = new Normalize(support2, n_channels, n_trials, n_samples);
		normalize->train();
		support1 = normalize->apply();	// Applico normalizzazione

		signal = invReshape(support1);	// Reshape inverso

		//wins.open((outputFilesRoot + "LDA/norm.txt").c_str()); // SLB
		wins.open(outputFilesRoot + "LDA/norm.txt");
		//for (int i=0; i<200; i++)
		for (int i=0; i<size; i++) // SLB
			for (int j=0;j<n_trials;j++)
				 wins << signal[i][j] << endl;
		wins.close();

		float *scores = new float[n_tag];	// Inizializzo a zero il vettore degli score
		for (int j = 0; j < n_tag; j++)
			scores[j] = 0;

		ofstream aaa;
		time_t now;
		char* timeNow = new char[12];
		now = time(NULL);
		strftime(timeNow, 80, "%Y_%m_%d_%H_%M_%S", gmtime(&now));

		aaa.open((outputFilesRoot + "LDA/stimuliSpeller.txt").c_str()); // SLB
		//aaa.open(outputFilesRoot + "LDA/stimuliSpeller.txt");
		for (int i=0; i<n_tag*n_flash; i++)
			aaa << stimuli[i] << endl;
		aaa.close();

		weighSignal(true);						// Ritorno il vettore w*x

		// SLB
		string rootPath = outputFilesRoot + "LDA/m_";
		rootPath += timeNow;
		rootPath += ".txt";
		ofstream testfile;
		testfile.open(rootPath);
		//
		for (int i=0; i<n_tag*n_flash; i++)
			testfile << m[i] << endl;
		testfile.close();

		ofstream tuttecose;
		tuttecose.open((outputFilesRoot + "LDA/classification.txt").c_str(), std::fstream::app); // SLB
		//tuttecose.open((outputFilesRoot + "LDA/classification.txt"), std::fstream::app);

		for (int j = 0; j < n_flash; j++)  {			// Inizio la classificazione
			int start = ((j)*n_tag);
			int stop = ((j+1)*n_tag)-1;

			tuttecose << start << " " << stop << endl << endl;
			int *stimulussequence = new int[n_tag];
			int *su = stimuli;

			for (int k = start; k <= stop; k++) {
				stimulussequence[k-start] = stimuli[k]-1;
				tuttecose << stimulussequence[k-start] << endl;
			}

			for (int k = 0; k < n_tag; k++) {
				scores[stimulussequence[k]] += m[k + start];
			}

			for (int k = 0; k < n_tag; k++) {
				tuttecose << scores[k] << endl;
			}

			tuttecose << "-------------------------------------" << endl;
			if (mode) {		// Se siamo in modalità RC il vettore degli indici avrà dimensione 2*n_runs,
							//la prima metà sarà l'ID di riga e la seconda sarà l'ID di colonna.
				maxIndex[i] = 0;
				maxIndex[i + n_runs] = n_tag/2; 
				float maxScoreR=scores[0];
				float maxScoreC=scores[n_tag/2];
				for (int k = 0; k < (int)(n_tag / 2); k++) {
					if (scores[k] > maxScoreR) {
						maxIndex[i] = k;
						maxScoreR = scores[k]; 
					}
					if (scores[k + (int)(n_tag / 2)] > maxScoreC) {
						maxIndex[i + n_runs] = k + (int)(n_tag / 2);
						maxScoreC = scores[k + (int)(n_tag / 2)]; 
					}
				}
				maxIndex[i] = maxIndex[i]+1;
				maxIndex[i+n_runs] = maxIndex[i+n_runs]+1;

			} else {
				maxIndex[i] = 0;
				float maxScore = scores[0];
				for (int k = 0; k < n_tag; k++) {
					if (scores[k] > maxScore) {
						maxIndex[i] = k;
						maxScore = scores[k]; 
					}
				}
				maxIndex[i] = maxIndex[i]+1;	
			}
		}

		// SLB delete[]
		/*
		delete[] timeNow;
		*/
		
	}

	return maxIndex;
}

/* SLB not necessary, never used in originalcd
void Classification::weighSignalFromFile() {
	ifstream file;
	file.open((outputFilesRoot + "LDA/m.txt").c_str()); // SLB
	//file.open(outputFilesRoot + "LDA/m.txt");
	string line; 
	int i = 0;

	if (file.is_open()) {
		while (getline(file, line)) {
			std::string::size_type sz;     // alias of size_t
			float val = ::atof(line.c_str());
			Classification::m[i] = val;
			i++;
		}
	}
	
}
*/

int* Classification::flashVector() { // Attualmente classifica su una sola calibrazione

	Winsor *winsor;
	Normalize *normalize;
	ofstream file;
	file.open((outputFilesRoot + "LDA/file.txt").c_str(), std::fstream::app); // SLB
	//file.open((outputFilesRoot + "LDA/file.txt"), std::fstream::app);

	int *vector;
	int *maxIndex;

	if (mode) {
		maxIndex = new int[n_runs * 2];
		vector = new int[n_flash * 2];
	} else {
		maxIndex = new int[n_runs];
		vector = new int[n_flash];
	}
	
	float **support1;
	float **support2;

	for (int i=0; i<n_runs; i++) {
		support1 = reshape(signal);  // Reshape per applicare normalizzazione e winsorizzazione

		winsor = new Winsor(support1, 0.1, n_channels, n_trials, n_samples);
		winsor->train();
		support2 = winsor->apply(); // Applico winsorizzazione

		normalize = new Normalize(support2, n_channels, n_trials, n_samples);
		normalize->train();
		support1 = normalize->apply();	// Applico normalizzazione

		signal = invReshape(support1);	// Reshape inverso
		
		float *scores = new float[n_tag];	// Inizializzo a zero il vettore degli score
		for (int j = 0; j < n_tag; j++)
			scores[j] = 0;

		weighSignal(false);						// Ritorno il vettore w*x

		//weighSignalFromFile();
		ofstream fileM;
		fileM.open((outputFilesRoot + "LDA/fileM.txt").c_str(), std::fstream::app); // SLB
		//fileM.open((outputFilesRoot + "LDA/fileM.txt"), std::fstream::app);
		fileM << endl;
		for (int ss = 0; ss < n_flash*n_tag; ss++)
			fileM << m[ss] << endl;
		fileM.close();
		int indiceVector = 0;

		for (int j=0; j<n_flash; j++) {			// Inizio la classificazione

			int start = (j * n_tag);
			int stop = ((j + 1) * n_tag) - 1;
			int *stimulussequence = new int[n_tag + 1];
			
			file << "---------------------------------------------------------" << endl;
			file << " ESECUZIONE " << j << ": " << endl;
			file << "START: " << start << " ; STOP: " << stop << endl;

			for (int k=start; k<=stop; k++) {
				stimulussequence[k - start] = stimuli[k] - 1;
				file << "STIMULUS SEQUENCE " << k - start << ": " << stimulussequence[k - start] << endl;
			}

			for (int k=0; k<n_tag; k++) {
				// SLB TODO FIX here it crashes systematically
				scores[stimulussequence[k]] += m[k + start];
			}

			for (int k=0; k<n_tag; k++) {
				file << "SCORES " << k << ": " << scores[k] << endl;
			}

			file << endl << "-----------------------------------------------------" << endl;

			if (mode) { // Se siamo in modalità RC il vettore degli indici avrà dimensione 2*n_runs, la prima metà sarà l'ID
						// di riga e la seconda sarà l'ID di colonna.
				maxIndex[i] = 0;
				maxIndex[i + n_runs] = n_tag / 2;

				float maxScoreR = scores[0];
				float maxScoreC = scores[n_tag / 2];

				for (int k=0; k<(int)(n_tag/2); k++) {
					if (scores[k] > maxScoreR) {
						maxIndex[i] = k;
						maxScoreR = scores[k];
					}
					if (scores[k + (int)(n_tag/2)] > maxScoreC) {
						maxIndex[i + n_runs] = k + (int)(n_tag/2);
						maxScoreC = scores[k + (int)(n_tag/2)];
					}
				}

				maxIndex[0] = maxIndex[i] + 1;
				maxIndex[1] = maxIndex[i + n_runs] + 1;

			}
			else {
				maxIndex[i] = 0;
				float maxScore = scores[0];

				for (int k=0; k<n_tag; k++) {
					if (scores[k] > maxScore) {
						maxIndex[i] = k;
						maxScore = scores[k];
					}
				}
				maxIndex[i] = maxIndex[i] + 1;
			}
			
			vector[indiceVector] = maxIndex[i];

			if (mode) {
				vector[indiceVector+1] = maxIndex[i+n_runs];
				indiceVector++;
			}

			indiceVector++;

		}
	}

	file << "PREDETTO: " << maxIndex[0] << endl;

	if (mode) {
		file << "PREDETTO: " << maxIndex[1] << endl;
	}

	file.close();
	return vector;
}

void Classification::weighSignal(bool flag) {

	float** temp;											//copia di signal con il termine di bias incluso
	temp = new float*[n_channels*(windowLen/sampleRate)+1];
	for (int i=0; i<n_channels*(windowLen/sampleRate)+1; i++) {
		temp[i] = new float[n_tag*n_flash];
			for (int j=0;j<n_tag*n_flash;j++) {
				if (i != n_channels*(windowLen/sampleRate))
					temp[i][j]=signal[i][j];
				else
					temp[i][j]=1;
			}
	}

	if (flag) {
		ofstream testfile;
		testfile.open((outputFilesRoot + "LDA/bufferSpeller.txt").c_str()); // SLB
		//testfile.open(outputFilesRoot + "LDA/bufferSpeller.txt");

		//for (int i=0; i<201; i++)
		//int size = (int) (n_channels * windowLen / sampleRate); // SLB moved to global
		for (int i=0; i<size+1; i++)
			for (int j=0;j<n_tag*n_flash;j++)
				testfile << temp[i][j] << endl; 
		
		testfile.close();
	}

	vectorXmatrix((n_channels*(windowLen/sampleRate))+1, n_tag*n_flash,temp);		//prodotto w*x

}

void Classification::vectorXmatrix(int size_vector, int n_trials,float** temp) {
	
	int dim = size_vector;
	int n_col=n_trials;
	float prod=0;
	
	for (int i=0; i<n_col; i++) {
		for (int j=0;j<dim;j++) 
			prod += w[j]*temp[j][i];
		m[i] = prod;
		prod = 0;
	}
}

void Classification::readWfromFile() {

	string line; 
	username.erase(0, username.find_first_not_of(' '));       //prefixing spaces
	username.erase(username.find_last_not_of(' ')+1); 
	ifstream file(outputFilesRoot + "/w_" + username + ".txt");
	int i=0;
	if (file.is_open()) {
		while (getline(file, line)) {
			//std::string::size_type sz;     // alias of size_t
			float val = ::atof(line.c_str());
			w[i] = val;
			i++;
		}
	}
}