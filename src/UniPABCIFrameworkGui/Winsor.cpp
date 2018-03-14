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

