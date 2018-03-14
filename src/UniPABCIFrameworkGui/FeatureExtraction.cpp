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

#include "FeatureExtraction.h"
#include "rootpaths.h"
#include "engine.h"
#include "matrix.h"
#include "pugixml.hpp"
#include <Windows.h>
#include <iostream>
#include <algorithm>

using namespace std;

Engine *eFE;

const char* pathToFeaturesFile;

float epy_norm		=  1;
float egy_norm		= -1;
float rsq_norm		= -1;


FeatureExtraction::FeatureExtraction(void)
{
}


FeatureExtraction::~FeatureExtraction(void)
{
}

bool FeatureExtraction::isBaseline() {

	readConfiguration();
	string baselteststring = "Basel";
	
	return binfilename.find(baselteststring) != std::string::npos;

}

string FeatureExtraction::GetExePath() {
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);

	string path = string(buffer);
    string::size_type pos = path.find_last_of("\\/");
	std::replace(path.begin(), path.end(), '\\', '/');
	
    return path.substr(0, pos+1);
}

void FeatureExtraction::readConfiguration() {
	
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str());
	
	if (result) {
		username		= doc.child("name").first_child().value();
		mode			= atoi(doc.child("mode").first_child().value());
		rows			= atoi(doc.child("row_num_interface").first_child().value());
		flash			= atoi(doc.child("num_flash").first_child().value());
		//channels		= atoi(doc.child("num_ch").first_child().value());
		int num_ch		= atoi(doc.child("num_ch").first_child().value());
		int num_ch_plus	= atoi(doc.child("num_ch_plus").first_child().value());
		channels		= num_ch + num_ch_plus;
		samplerate		= atoi(doc.child("samplerate").first_child().value());
		timeflash		= atoi(doc.child("time_flash").first_child().value());
		tagPredicted	= atoi(doc.child("flash_analysis").first_child().value());
		past_epy		= atof(doc.child("epy_cum").first_child().value());
		past_egy		= atof(doc.child("egy_cum").first_child().value());
		epy_min			= atof(doc.child("epy_min").first_child().value());
		epy_max			= atof(doc.child("epy_max").first_child().value());
		egy_min			= atof(doc.child("egy_min").first_child().value());
		egy_max			= atof(doc.child("egy_max").first_child().value());
		past_rsq		= atof(doc.child("rsq_cum").first_child().value());
		//past_rsq_gap	= atof(doc.child("rsq_gap").first_child().value());
		rsq_min			= atof(doc.child("rsq_min").first_child().value());
		rsq_max			= atof(doc.child("rsq_max").first_child().value());
		binfilename		= doc.child("binfilename").first_child().value();
	}
	else
		cout << "Could not find configuration file." << endl;
}

void FeatureExtraction::updateConfiguration() {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str());
	if (result) {
		pugi::xml_node node = doc.child("stimuli_entropy");
		node.first_child().set_value(std::to_string(stimuli_entropy).c_str());
		node = doc.child("stimuli_energy");
		node.first_child().set_value(std::to_string(stimuli_energy).c_str());
		node = doc.child("epy_cum");
		node.first_child().set_value(std::to_string(past_epy).c_str());
		node = doc.child("egy_cum");
		node.first_child().set_value(std::to_string(past_egy).c_str());
		node = doc.child("epy_min");
		node.first_child().set_value(std::to_string(epy_min).c_str());
		node = doc.child("epy_max");
		node.first_child().set_value(std::to_string(epy_max).c_str());
		node = doc.child("egy_min");
		node.first_child().set_value(std::to_string(egy_min).c_str());
		node = doc.child("egy_max");
		node.first_child().set_value(std::to_string(egy_max).c_str());
		node = doc.child("epy_norm");
		node.first_child().set_value(std::to_string(epy_norm).c_str());
		node = doc.child("egy_norm");
		node.first_child().set_value(std::to_string(egy_norm).c_str());

		node = doc.child("rsq");
		node.first_child().set_value(std::to_string(rsq).c_str());
		node = doc.child("rsq_cum");
		node.first_child().set_value(std::to_string(past_rsq).c_str());
		//node = doc.child("rsq_gap");
		//node.first_child().set_value(std::to_string(rsq_gap).c_str());
		node = doc.child("rsq_min");
		node.first_child().set_value(std::to_string(rsq_min).c_str());
		node = doc.child("rsq_max");
		node.first_child().set_value(std::to_string(rsq_max).c_str());
		node = doc.child("rsq_norm");
		node.first_child().set_value(std::to_string(rsq_norm).c_str());

		doc.save_file((configFilesRoot + "/configuration.xml").c_str());
	}
	else
		cout << "Could not find configuration file." << endl;
}

bool FeatureExtraction::InitialiseEngine() {
	
	string absoluteCurrentPath;

	if (eFE = engOpen("")) {
		cout << "Matlab engine succesfully started. (FeatureExtraction)" << endl << endl;
		
		readConfiguration();
		pathToFeatures		= outputFilesRoot + "/" + username + "_features.txt";
		pathToFeaturesFile	= pathToFeatures.c_str();

		absoluteCurrentPath	= GetExePath();
		matlabDir			= absoluteCurrentPath += "matlab";

		const char* cm;
		string command;
	
		command = "cd ";
		command += matlabDir;

		cm = command.c_str();
		engEvalString(eFE, cm);
		
		return true;
	}
	else {
		cout << "Matlab engine could not be started. (FeatureExtraction::InitialiseEngine)" << endl;
		return false;
	}

}

void FeatureExtraction::Process() { // main
	
	readConfiguration();
	
	writeFileName();

	clearWS();
	
	EpyEgy();
	printResultsEE();
	writeResultsEE();
	
	rSquare(tagPredicted);
	printResultRsq();
	writeResultRsq();

	EpyEgyNorm();
	rSquareNorm();

	updateConfiguration();
	
}

void FeatureExtraction::clearWS() {
	
	const char* cm;
	string command;

	command = "clear all, pack;";

	cm = command.c_str();
	engEvalString(eFE, cm);

}

void FeatureExtraction::writeFileName() {

	featuresFile.open(pathToFeaturesFile, std::ios_base::app | std::ios_base::out);

	featuresFile << "Bin File = " << binfilename << endl;

	featuresFile.flush();
	featuresFile.close();

}

void FeatureExtraction::BaselineAnalysis() {

	readConfiguration();

	if (isBaseline()) {
		
		const char* cm;
		string command;

		command = ""; ///////////////////////// SLB TODO INSERT MATLAB COMMAND HERE FOR BASELINE ANALYSIS

		cm = command.c_str();
		engEvalString(eFE, cm);

		// SLB TODO

	}

}

void FeatureExtraction::EpyEgy() {

	const char* cm;
	string command;
	
	command = "[epy_min, epy_max, egy_min, egy_max, signal_entropy, signal_energy, baseline_entropy, baseline_energy, stimuli_entropy, stimuli_energy, signal_entropy_theta, signal_entropy_alpha, signal_entropy_beta, signal_entropy_gamma, signal_energy_theta, signal_energy_alpha, signal_energy_beta, signal_energy_gamma, baseline_entropy_theta, baseline_entropy_alpha, baseline_entropy_beta, baseline_entropy_gamma, stimuli_entropy_theta, stimuli_entropy_alpha, stimuli_entropy_beta, stimuli_entropy_gamma, baseline_energy_theta, baseline_energy_alpha, baseline_energy_beta, baseline_energy_gamma, stimuli_energy_theta, stimuli_energy_alpha, stimuli_energy_beta, stimuli_energy_gamma] = ext_feat(";
	command += "'../";
	command += binfilename;
	command += "', ";
	command += std::to_string(channels);
	command += ", ";
	command += std::to_string(samplerate);
	command += ", ";
	command += std::to_string(timeflash);
	command += ");";

	cout << command << endl; ///

	cm = command.c_str();
	engEvalString(eFE, cm);
	
	signal_entropy			= *mxGetPr(engGetVariable(eFE, "signal_entropy"));
	signal_energy			= *mxGetPr(engGetVariable(eFE, "signal_energy"));

	baseline_entropy		= *mxGetPr(engGetVariable(eFE, "baseline_entropy"));
	baseline_energy			= *mxGetPr(engGetVariable(eFE, "baseline_energy"));
	
	stimuli_entropy			= *mxGetPr(engGetVariable(eFE, "stimuli_entropy"));
	stimuli_energy			= *mxGetPr(engGetVariable(eFE, "stimuli_energy"));
	
	baseline_entropy_alpha	= *mxGetPr(engGetVariable(eFE, "baseline_entropy_alpha"));
	baseline_entropy_beta	= *mxGetPr(engGetVariable(eFE, "baseline_entropy_beta"));
	baseline_entropy_gamma	= *mxGetPr(engGetVariable(eFE, "baseline_entropy_gamma"));
	baseline_entropy_theta	= *mxGetPr(engGetVariable(eFE, "baseline_entropy_theta"));

	baseline_energy_alpha	= *mxGetPr(engGetVariable(eFE, "baseline_energy_alpha"));
	baseline_energy_beta	= *mxGetPr(engGetVariable(eFE, "baseline_energy_beta"));
	baseline_energy_gamma	= *mxGetPr(engGetVariable(eFE, "baseline_energy_gamma"));
	baseline_energy_theta	= *mxGetPr(engGetVariable(eFE, "baseline_energy_theta"));

	stimuli_entropy_alpha	= *mxGetPr(engGetVariable(eFE, "stimuli_entropy_alpha"));
	stimuli_entropy_beta	= *mxGetPr(engGetVariable(eFE, "stimuli_entropy_beta"));
	stimuli_entropy_gamma	= *mxGetPr(engGetVariable(eFE, "stimuli_entropy_gamma"));
	stimuli_entropy_theta	= *mxGetPr(engGetVariable(eFE, "stimuli_entropy_theta"));

	stimuli_energy_alpha	= *mxGetPr(engGetVariable(eFE, "stimuli_energy_alpha"));
	stimuli_energy_beta		= *mxGetPr(engGetVariable(eFE, "stimuli_energy_beta"));
	stimuli_energy_gamma	= *mxGetPr(engGetVariable(eFE, "stimuli_energy_gamma"));
	stimuli_energy_theta	= *mxGetPr(engGetVariable(eFE, "stimuli_energy_theta"));

	float epy_min_tmp		= *mxGetPr(engGetVariable(eFE, "epy_min"));
	float epy_max_tmp		= *mxGetPr(engGetVariable(eFE, "epy_max"));
	float egy_min_tmp		= *mxGetPr(engGetVariable(eFE, "egy_min"));
	float egy_max_tmp		= *mxGetPr(engGetVariable(eFE, "egy_max"));

	float entropy			= signal_entropy;
	float energy			= signal_energy;
	
	if ((epy_min_tmp != 1) && (epy_max_tmp != 1) && (egy_min_tmp != -1) && (egy_max_tmp != -1)) {
		// initial state, start with a calibration session
		past_epy	= entropy;
		past_egy	= energy;

		epy_min		= epy_min_tmp;
		epy_max		= epy_max_tmp;
		egy_min		= egy_min_tmp;
		egy_max		= egy_max_tmp;
	}
	else {
		(past_epy == 1)  ? past_epy = entropy : past_epy = (past_epy + entropy) / 2;
		(past_egy == -1) ? past_egy = energy  : past_egy = (past_egy + energy)  / 2;

		if (entropy < epy_min)
			epy_min = entropy;
		else if (entropy > epy_max)
			epy_max = entropy;

		if (energy < egy_min)
			egy_min = energy;
		else if (energy > egy_max)
			egy_max = energy;
	}
	
}

void FeatureExtraction::EpyEgyNorm() {

	float entropy	= signal_entropy;
	float energy	= signal_energy;

	if (epy_max != epy_min) {
		epy_norm		 = entropy	- epy_max;
		epy_norm		/= epy_min	- epy_max;
	}
	else {
		epy_norm		= 0.5; // SLB workaround when using the multiple identical targets during calibration
	}

	if (egy_max != egy_min) {
		egy_norm		 = energy	- egy_min;
		egy_norm		/= egy_max	- egy_min;
	}
	else {
		egy_norm		= 0.5; // SLB workaround when using the multiple identical targets during calibration
	}
	
}

void FeatureExtraction::printResultsEE() {
	
	cout << "signal entropy         -> " << signal_entropy			<< endl;
	cout << "signal energy          -> " << signal_energy			<< endl;

	cout << "baseline entropy       -> " << baseline_entropy		<< endl;
	cout << "baseline energy        -> " << baseline_energy			<< endl;

	cout << "stimuli entropy        -> " << stimuli_entropy			<< endl;
	cout << "stimuli energy         -> " << stimuli_energy			<< endl;

	cout << "baseline entropy theta -> " << baseline_entropy_theta	<< endl;
	cout << "baseline entropy alpha -> " << baseline_entropy_alpha	<< endl;
	cout << "baseline entropy beta  -> " << baseline_entropy_beta	<< endl;
	cout << "baseline entropy gamma -> " << baseline_entropy_gamma	<< endl;
	
	cout << "baseline energy theta  -> " << baseline_energy_theta	<< endl;
	cout << "baseline energy alpha  -> " << baseline_energy_alpha	<< endl;
	cout << "baseline energy beta   -> " << baseline_energy_beta	<< endl;
	cout << "baseline energy gamma  -> " << baseline_energy_gamma	<< endl;
	
	cout << "stimuli  entropy theta -> " << stimuli_entropy_theta	<< endl;
	cout << "stimuli  entropy alpha -> " << stimuli_entropy_alpha	<< endl;
	cout << "stimuli  entropy beta  -> " << stimuli_entropy_beta	<< endl;
	cout << "stimuli  entropy gamma -> " << stimuli_entropy_gamma	<< endl;
	
	cout << "stimuli  energy theta  -> " << stimuli_energy_theta	<< endl;
	cout << "stimuli  energy alpha  -> " << stimuli_energy_alpha	<< endl;
	cout << "stimuli  energy beta   -> " << stimuli_energy_beta		<< endl;
	cout << "stimuli  energy gamma  -> " << stimuli_energy_gamma	<< endl;
	
}

void FeatureExtraction::writeResultsEE() {

	featuresFile.open(pathToFeaturesFile, std::ios_base::app | std::ios_base::out);
	
	featuresFile << "signal entropy         = " << signal_entropy			<< endl;
	featuresFile << "signal energy			= " << signal_energy			<< endl;

	featuresFile << "baseline entropy       = " << baseline_entropy			<< endl;
	featuresFile << "baseline energy        = " << baseline_energy			<< endl;

	featuresFile << "stimuli entropy        = " << stimuli_entropy			<< endl;
	featuresFile << "stimuli energy         = " << stimuli_energy			<< endl;

	featuresFile << "baseline entropy theta = " << baseline_entropy_theta	<< endl;
	featuresFile << "baseline entropy alpha = " << baseline_entropy_alpha	<< endl;
	featuresFile << "baseline entropy beta  = " << baseline_entropy_beta	<< endl;
	featuresFile << "baseline entropy gamma = " << baseline_entropy_gamma	<< endl;
	
	featuresFile << "baseline energy theta  = " << baseline_energy_theta	<< endl;
	featuresFile << "baseline energy alpha  = " << baseline_energy_alpha	<< endl;
	featuresFile << "baseline energy beta   = " << baseline_energy_beta		<< endl;
	featuresFile << "baseline energy gamma  = " << baseline_energy_gamma	<< endl;
	
	featuresFile << "stimuli  entropy theta = " << stimuli_entropy_theta	<< endl;
	featuresFile << "stimuli  entropy alpha = " << stimuli_entropy_alpha	<< endl;
	featuresFile << "stimuli  entropy beta  = " << stimuli_entropy_beta		<< endl;
	featuresFile << "stimuli  entropy gamma = " << stimuli_entropy_gamma	<< endl;
	
	featuresFile << "stimuli  energy theta  = " << stimuli_energy_theta		<< endl;
	featuresFile << "stimuli  energy alpha  = " << stimuli_energy_alpha		<< endl;
	featuresFile << "stimuli  energy beta   = " << stimuli_energy_beta		<< endl;
	featuresFile << "stimuli  energy gamma  = " << stimuli_energy_gamma		<< endl;

	//featuresFile << "******************************" << endl;
	
	featuresFile.flush();
	featuresFile.close();

}

void FeatureExtraction::rSquare(int tagPredicted) {
	
	const char* cm;
	string command;

	command = "[rsq, rsq_min, rsq_max] = ext_rsq(";
	command += "'../";
	command += binfilename;
	command += "', ";
	command += std::to_string(rows);
	command += ", ";
	command += std::to_string(mode);
	command += ", ";
	command += std::to_string(tagPredicted);
	command += ", ";
	command += std::to_string(channels);
	command += ", ";
	command += std::to_string(samplerate);
	command += ", ";
	command += std::to_string(timeflash);
	command += ");";

	//cout << command << endl; ///

	cm = command.c_str();
	engEvalString(eFE, cm);
	
	rsq		= *mxGetPr(engGetVariable(eFE, "rsq"));
	
	float rsq_min_tmp = *mxGetPr(engGetVariable(eFE, "rsq_min"));
	float rsq_max_tmp = *mxGetPr(engGetVariable(eFE, "rsq_max"));
	
	if ((rsq_min_tmp != -1) && (rsq_max_tmp != -1)) {
		// it is a calibration session
		past_rsq	= rsq;			// this is actually the average of the rsq values computed for each target
		rsq_min		= rsq_min_tmp;	// updating min and max
		rsq_max		= rsq_max_tmp;
	}
	else {
		// it is a spelling session
		(past_rsq == -1) ? past_rsq = rsq : past_rsq = (past_rsq + rsq) / 2; // updating running average of rsq

		if (rsq < rsq_min)
			rsq_min = rsq;
		else if (rsq > rsq_max)
			rsq_max = rsq;
	}

	// outputs:
	// rsq
	// rsq_min
	// rsq_max
		
}

void FeatureExtraction::rSquareNorm() {

	if (rsq_max != rsq_min) {
		rsq_norm		 = rsq		- rsq_min;
		rsq_norm		/= rsq_max	- rsq_min;	// scaled in the interval [0-1] of all rsq calculated during the calibration session
	}
	else {
		 rsq_norm		= 0.5; // SLB workaround when using the multiple identical targets during calibration
	}
	
}

void FeatureExtraction::printResultRsq() {

	cout << "R^2 -> " << rsq << endl;
	
}

void FeatureExtraction::writeResultRsq() {

	featuresFile.open(pathToFeaturesFile, std::ios_base::app | std::ios_base::out);

	featuresFile << "R^2 = " << rsq << endl;
	
	featuresFile << "******************************" << endl;

	featuresFile.flush();
	featuresFile.close();

}

