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
#include <vector>

#define ACTIVATION_THRESHOLD 0.25
// Threshold value has been choosen by observing a valid value
// computed during the preliminary experiments


class ResponseModulation
{
public:
	ResponseModulation(void);
	virtual ~ResponseModulation(void);

	virtual void readConfiguration();
	virtual void updateConfiguration();
	virtual bool isCalibration();
	virtual std::string getResult();
	virtual void writeResult();
	virtual void thresholdModule();
	virtual void modulationModule();
	virtual void Process();

private:
	bool bioHybridMode;
	int flash_analysis;
	int selectedID;
	std::vector <int> tobiiresultIDs;
	float stimuli_entropy, epy_min, epy_max, epy_cum, epy_norm;
	float stimuli_energy, egy_min, egy_max, egy_cum, egy_norm;
	//float rsq, rsq_cum, rsq_gap, rsq_min, rsq_max, rsq_norm;
	float rsq, rsq_cum, rsq_min, rsq_max, rsq_norm;
	bool activation_threshold;
	float behaviour_intensity;

	float flash_precision;
	float tobiiresultPC;
	std::string username;
	std::string binfilename;

	std::string pathToResults;
	std::ofstream resultsFile;

};

