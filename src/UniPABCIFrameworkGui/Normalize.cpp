#include "Normalize.h"
#include <gsl/gsl_statistics_float.h>


Normalize::Normalize(float **p_matrix, int p_numberOfChannels, int p_numberOfTrials, int p_numberOfSamples) {
	nMatrix = p_matrix;
	numberOfChannels = p_numberOfChannels;
	numberOfTrials = p_numberOfTrials;
	numberOfSamples = p_numberOfSamples;
	meanValues = new float[numberOfChannels];
	stdValues = new float[numberOfChannels];
}

Normalize::~Normalize() {
	delete meanValues;
	delete stdValues;
}

void Normalize::train() {  
	for (int i=0; i<numberOfChannels; i++) {
		meanValues[i] = gsl_stats_float_mean(nMatrix[i], 1, numberOfSamples*numberOfTrials);
		stdValues[i] = gsl_stats_float_sd(nMatrix[i], 1, numberOfSamples*numberOfTrials);
	}
}

float** Normalize::apply() {
	float **newM;
	newM = new float*[numberOfChannels];
	for (int i=0; i<numberOfChannels; i++) {
		newM[i] = new float[numberOfSamples*numberOfTrials];
		for (int j = 0; j < numberOfSamples*numberOfTrials; j++) {
			newM[i][j] = nMatrix[i][j] - meanValues[i];
			newM[i][j] /= stdValues[i];
		}
	}

	return newM;
}