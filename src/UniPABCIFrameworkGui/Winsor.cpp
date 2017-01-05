#include "Winsor.h"
#include <iostream>
#include <gsl/gsl_sort_float.h>

float roundf(float x) {

   return x >= 0.0f ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}


Winsor::Winsor(float **p_matrix, float p_p, int p_numberOfChannels, int p_numberOfTrials, int p_numberOfSamples) {
	wMatrix = p_matrix;
	p = p_p;
	numberOfChannels = p_numberOfChannels;
	numberOfTrials = p_numberOfTrials;
	numberOfSamples =	p_numberOfSamples;
	limit_l = new float[numberOfChannels];
	limit_h = new float[numberOfChannels];
	clip = roundf(numberOfTrials*numberOfSamples*p / 2);
}

Winsor::~Winsor() {
	delete limit_l;
	delete limit_h;
}

void Winsor::train() {
	float *tmp;
	tmp = new float[numberOfTrials*numberOfSamples];
	for (int i=0; i<numberOfChannels; i++) {
		for (int j = 0; j < numberOfTrials*numberOfSamples; j++)
			tmp[j] = wMatrix[i][j];
		gsl_sort_float(tmp, 1, numberOfTrials*numberOfSamples);
		limit_l[i] = tmp[clip-1];
		limit_h[i] = tmp[(numberOfTrials*numberOfSamples) - clip];
	}
}

float** Winsor::apply() {
	float **matrix;
	matrix = new float*[numberOfChannels];

	for (int i=0; i<numberOfChannels; i++) {		
		matrix[i] = new float[numberOfSamples*numberOfTrials];
		for (int j = 0; j < numberOfSamples*numberOfTrials; j++) {
			if (wMatrix[i][j] > limit_h[i])
				matrix[i][j] = limit_h[i];

			else if (wMatrix[i][j] < limit_l[i])
				matrix[i][j] = limit_l[i];

			else
				matrix[i][j] = wMatrix[i][j];
		}
	}

	return matrix;
}