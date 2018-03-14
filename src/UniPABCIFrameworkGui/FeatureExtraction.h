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

#pragma once

#include <fstream>


class FeatureExtraction
{
public:
	FeatureExtraction(void);
	virtual ~FeatureExtraction(void);

	virtual bool isBaseline();
	virtual std::string GetExePath();
	virtual void readConfiguration();
	virtual void updateConfiguration();
	virtual bool InitialiseEngine();
	virtual void Process();
	virtual void clearWS();
	virtual void writeFileName();
	virtual void BaselineAnalysis();
	virtual void EpyEgy();
	virtual void EpyEgyNorm();
	virtual void printResultsEE();
	virtual void writeResultsEE();
	virtual void rSquare(int);
	virtual void rSquareNorm();
	virtual void printResultRsq();
	virtual void writeResultRsq();

private:
	std::string pathToFeatures;
	std::string matlabDir;
	std::ofstream featuresFile;

	std::string username;
	int mode;
	int rows;
	int flash;
	int channels;
	int samplerate;
	int timeflash;
	int tagPredicted;
	std::string binfilename;

	double signal_entropy;
	double signal_energy;
	double baseline_entropy;
	double baseline_energy;
	double stimuli_entropy, epy_min, epy_max;
	double stimuli_energy, egy_min, egy_max;
	double baseline_entropy_alpha,	baseline_entropy_beta,	baseline_entropy_gamma,	baseline_entropy_theta;
	double baseline_energy_alpha,	baseline_energy_beta,	baseline_energy_gamma,	baseline_energy_theta;
	double stimuli_entropy_alpha,	stimuli_entropy_beta,	stimuli_entropy_gamma,	stimuli_entropy_theta;
	double stimuli_energy_alpha,	stimuli_energy_beta,	stimuli_energy_gamma,	stimuli_energy_theta;

	double past_epy, past_egy;
	double past_rsq, rsq, rsq_min, rsq_max;
	
};

