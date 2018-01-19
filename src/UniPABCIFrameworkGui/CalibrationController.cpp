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

#include "CalibrationController.h"
#include <iostream>
#include "Winsor.h"
#include "Normalize.h"
#include "clientTCP.h"
#include "rootpaths.h" // SLB
#include "pugixml.hpp"

using std::cout;
using std::endl;
using std::ifstream;


CalibrationController::CalibrationController(int numCh, int windLen, int NumCalibTag, int numFlash, int p_tags, int SampleRate, bool flashing, int rowNumb, string p_user) {

	numberOfChannels = numCh;
	windowLen = windLen;		
	numberOfCalibrationTag = NumCalibTag;
	numberOfFlash = numFlash;						
	tags = p_tags;
	sampleRate = SampleRate;
	flashingMode = flashing;
	numberOfRowInterface = rowNumb;
	username = p_user;

	P300CalibrationBuffer = new float*[numberOfChannels*windowLen];
	for (int i=0; i<numberOfChannels*windowLen; i++) {
		P300CalibrationBuffer[i] = new float[tags*numberOfFlash*numberOfCalibrationTag];
	}

	int size = (int) (numberOfChannels * windowLen / sampleRate);
	P300CalibrationBufferDS = new float*[size];
	for (int i=0; i<size; i++)
		P300CalibrationBufferDS[i] = new float[tags*numberOfFlash*numberOfCalibrationTag];

	openBuffer = new bool[tags*numberOfFlash*numberOfCalibrationTag];
	buffersSize = new int[tags*numberOfFlash*numberOfCalibrationTag];
	stimuli = new int[tags*numberOfFlash*numberOfCalibrationTag];
	labels = new int[tags*numberOfFlash*numberOfCalibrationTag];
	for (int i=0; i<tags*numberOfFlash*numberOfCalibrationTag; i++) {
		openBuffer[i] = false;
		buffersSize[i] = 0;
		stimuli[i] = 0;
		labels[i] = -1;
	}

	target = new int[numberOfCalibrationTag];
	//currentCalibrationTag = 1;
	currentCalibrationTag = 0; // SLB was 1

	numberOfCalibrations = 0;

	remove((outputFilesRoot + "LDA/tempBuffer.txt").c_str());	// SLB
	remove((outputFilesRoot + "LDA/tempStimuli.txt").c_str());	// SLB
	remove((outputFilesRoot + "LDA/tempLabel.txt").c_str());	// SLB

	// SLB
	buffered_tag		= 0;
	buffered_tag_flag	= false;
}

int CalibrationController::concCalibrations() {

	pugi::xml_document doc;
	//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	pugi::xml_node tmp = doc.child("calibration_analysis");
	string cal = tmp.first_child().value();

	char *s4 = new char[cal.size() + 1];
	memcpy(s4, cal.c_str(), cal.size() + 1);
	char *pch = strtok(s4, "-");

	std::vector<string> vecIndex;

	while (pch != NULL) {
		string ciao(pch);
		vecIndex.push_back(ciao);
		pch = strtok(NULL, "-");
	}

	int numberOfCal = vecIndex.size();

	remove((outputFilesRoot + "LDA/concBuffer.txt").c_str()); // SLB
	ofstream bufferTotal; // SLB
	bufferTotal.open((outputFilesRoot + "LDA/concBuffer.txt").c_str(), std::fstream::app); // SLB

	remove((outputFilesRoot + "LDA/concLabel.txt").c_str()); // SLB
	ofstream labelTotal; // SLB
	labelTotal.open((outputFilesRoot + "LDA/concLabel.txt").c_str(), std::fstream::app); // SLB

	remove((outputFilesRoot + "LDA/concStimuli.txt").c_str()); // SLB
	ofstream stimuliTotal; // SLB
	stimuliTotal.open((outputFilesRoot + "LDA/concStimuli.txt").c_str(), std::fstream::app); // SLB

	ifstream tempBuffer;
	ifstream tempLabel;
	ifstream tempStimuli;

	for (int i=0; i<numberOfCal; i++) {
		string index = vecIndex[i];
		
		tempBuffer.open((outputFilesRoot + "LDA/calib_" + index + ".txt").c_str());
		tempLabel.open((outputFilesRoot + "LDA/label_" + index + ".txt").c_str());
		tempStimuli.open((outputFilesRoot + "LDA/stimuli_" + index + ".txt").c_str());
		/*
		tempBuffer.open(outputFilesRoot + "LDA/calib_" + index + ".txt");
		tempLabel.open(outputFilesRoot + "LDA/label_" + index + ".txt");
		tempStimuli.open(outputFilesRoot + "LDA/stimuli_" + index + ".txt");
		*/
		string line;
		if (tempBuffer.is_open()) {
			while (getline(tempBuffer, line)) {
				//std::string::size_type sz;     // alias of size_t
				float val = atof(line.c_str());
				bufferTotal << val << endl;
			}
		}
		
		tempBuffer.close();

		if (tempLabel.is_open()) {
			while (getline(tempLabel, line)) {
				//std::string::size_type sz;     // alias of size_t
				float val = atoi(line.c_str());
				labelTotal << val << endl;
			}
		}
		tempLabel.close();

		if (tempStimuli.is_open()) {
			while (getline(tempStimuli, line)) {
				//std::string::size_type sz;     // alias of size_t
				float val = atoi(line.c_str());
				stimuliTotal << val << endl;
			}
		}
		tempStimuli.close();
	}

	bufferTotal.close();
	labelTotal.close();
	stimuliTotal.close();

	// SLB delete[]
	/*
	delete[] s4;
	delete[] pch;
	*/

	return numberOfCal;
}

void CalibrationController::setIdTag(int* tagvector) {

	for (int i=0; i<numberOfCalibrationTag; i++)
		target[i] = tagvector[i];
}

void CalibrationController::fillBuffer(float *receivedData) {

	// SLB
	int channels = numberOfChannels + number_of_channels_plus;

	// settaggio delle variabili openBuffer, stimuli e labels corrispondenti all'i-esimo tag che flasha
	if (receivedData[channels + 1] > 0) {
		for (int i=0; i<numberOfFlash*tags*numberOfCalibrationTag; i++) {
			if (buffersSize[i] == 0 && openBuffer[i] == false) {
				openBuffer[i] = true;
				stimuli[i] = receivedData[channels + 1];
				if (flashingMode) {
					//int* index = mappingIDtoRC(target[currentCalibrationTag - 1]);
					int* index = mappingIDtoRC(target[currentCalibrationTag]); // SLB
					if (receivedData[channels + 1] == index[0] || receivedData[channels + 1] == index[1]) {
						labels[i] = 1;
					}
				}
				else {
					//if (receivedData[numberOfChannels + 1] == target[currentCalibrationTag - 1]) {
					if (receivedData[channels + 1] == target[currentCalibrationTag]) { // SLB
						labels[i] = 1;
					}
				}
				// SLB
				//cout << stimuli[i] << endl;			///////////////////////////
				//cout << labels[i] << endl << endl;	///////////////////////////
				//
				break;
			}
		}
	}

	// riempimento della corrispondente colonna di P300CalBuffer
	for (int i=0; i<numberOfFlash*tags*numberOfCalibrationTag; i++) {
		if (openBuffer[i]) {
			for (int j = 0; j < numberOfChannels; j++)
				P300CalibrationBuffer[j + buffersSize[i]][i] = receivedData[j];
			//buffersSize[i] = buffersSize[i] + numberOfChannels;
			buffersSize[i] += numberOfChannels; // SLB
			if (buffersSize[i] >= windowLen*numberOfChannels)
				openBuffer[i] = false;
		}
	}

	//condizione che indica il completamento del buffer per ciascun tag di calibrazione
	//if (buffersSize[tags*currentCalibrationTag*numberOfFlash-1] >= windowLen*numberOfChannels) {
	if (buffersSize[tags*(currentCalibrationTag+1)*numberOfFlash-1] >= windowLen*numberOfChannels) { // SLB
		_finishStoreSingleTargetData.SetEvent();
		cout << "EVENTO FINE SCRITTURA BUFFER SINGOLO TARGET" << endl;
		currentCalibrationTag++;						//incremento l'indice del target di calibrazione utilizzato
	}
	
	// condizione che indica il completamento del buffer per l'intera sessione di calibrazione
	if (buffersSize[tags*numberOfCalibrationTag*numberOfFlash-1] >= windowLen*numberOfChannels) {
		_finishStoreAllSessionData.SetEvent();
		cout << "EVENTO FINE SCRITTURA BUFFER INTERO" << endl;
	}
	
}

void CalibrationController::downSampling() {

	int index = 0;
	//for (int i=0; i<windowLen*numberOfChannels; i = i + numberOfChannels*sampleRate) {
	for (int i=0; i<windowLen*numberOfChannels; i += (numberOfChannels*sampleRate)) { // SLB
		for (int j = 0; j < numberOfChannels; j++) {
			for (int k = 0; k < tags*numberOfFlash*numberOfCalibrationTag; k++)
				P300CalibrationBufferDS[index][k] = P300CalibrationBuffer[i + j][k];
			index++;
		}
	}
}

float** CalibrationController::reshape(float **oldSignal) {
	float **reshaped;
	reshaped = new float*[numberOfChannels];
	for (int i=0; i<numberOfChannels; i++)
		reshaped[i] = new float[numberOfFlash*windowLen/sampleRate*tags*numberOfCalibrationTag*numberOfCalibrations];

	int index = 0;
	int k;
	for (int i=0; i<numberOfChannels; i++) {
		index = 0;
		k = i;
		for (; index < (numberOfCalibrationTag*tags*numberOfFlash*(windowLen/sampleRate)*numberOfCalibrations); k += numberOfChannels) {
			for (int j = 0; j < numberOfCalibrationTag*tags*numberOfFlash*numberOfCalibrations; j++) {
				reshaped[i][index] = oldSignal[k][j];
				index++;
			}
		}
	}

	return reshaped;

}

float** CalibrationController::invReshape(float **oldSignal) {

	float **reshaped;
	int size = (int) (numberOfChannels * (windowLen / sampleRate));

	reshaped = new float*[size];
	for (int i=0; i<size; i++)
		reshaped[i] = new float[numberOfCalibrationTag*tags*numberOfFlash*numberOfCalibrations];

	//int index[4] = { 0, 0, 0, 0 };
	// SLB
	std::vector <int> index;
	for (int i=0; i<numberOfChannels; i++) {
		index.push_back(0);
	}
	//
	
	//for (int i=0; i<windowLen; i++) {
	for (int i=0; i<size; i++) { // SLB // ST
		for (int j = 0; j < numberOfCalibrationTag*tags*numberOfFlash*numberOfCalibrations; j++) {
			reshaped[i][j] = oldSignal[i % numberOfChannels][index[i % numberOfChannels]];
			index[i % numberOfChannels]++;
		}
	}

	return reshaped;
}

int CalibrationController::getNumberOfCalibration() {

	return numberOfCalibrations;
}

void CalibrationController::nextCalibration() {


	ofstream bufferCalib;
	ofstream bufferLabel;
	ofstream bufferStimuli;
	
	bufferCalib.open((outputFilesRoot + "LDA/calib_" + std::to_string(numberOfCalibrations+1) + ".txt").c_str());

	tempBuffer.open((outputFilesRoot + "LDA/tempBuffer.txt").c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
	tempStimuli.open((outputFilesRoot + "LDA/tempStimuli.txt").c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
	tempLabel.open((outputFilesRoot + "LDA/tempLabel.txt").c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
	/*
	bufferCalib.open(outputFilesRoot + "LDA/calib_" + std::to_string(numberOfCalibrations+1) + ".txt");

	tempBuffer.open((outputFilesRoot + "LDA/tempBuffer.txt"), std::fstream::in | std::fstream::out | std::fstream::app);
	tempStimuli.open((outputFilesRoot + "LDA/tempStimuli.txt"), std::fstream::in | std::fstream::out | std::fstream::app);
	tempLabel.open((outputFilesRoot + "LDA/tempLabel.txt"), std::fstream::in | std::fstream::out | std::fstream::app);
	*/
	downSampling();
	numberOfCalibrations++;
	int size = (int) (numberOfChannels * windowLen / sampleRate);
	for (int i=0; i<size; i++) {
		for (int j = 0; j < tags*numberOfCalibrationTag*numberOfFlash; j++) {
				tempBuffer << P300CalibrationBufferDS[i][j] << endl;
				bufferCalib << P300CalibrationBufferDS[i][j] << endl; 
		}
	}

	bufferCalib.close();
	/*
	bufferLabel.open((outputFilesRoot + "LDA/label_" + std::to_string(numberOfCalibrations) + ".txt").c_str());
	bufferStimuli.open((outputFilesRoot + "LDA/stimuli_" + std::to_string(numberOfCalibrations) + ".txt").c_str());
	*/
	bufferLabel.open(outputFilesRoot + "LDA/label_" + std::to_string(numberOfCalibrations) + ".txt");
	bufferStimuli.open(outputFilesRoot + "LDA/stimuli_" + std::to_string(numberOfCalibrations) + ".txt");
	
	for (int i=0; i<tags*numberOfFlash*numberOfCalibrationTag; i++) {
		tempStimuli << stimuli[i] << endl;
		bufferStimuli << stimuli[i] << endl;
		tempLabel << labels[i] << endl;
		bufferLabel << labels[i] << endl;
		openBuffer[i] = false;
		buffersSize[i] = 0;
		stimuli[i] = 0;
		labels[i] = -1;
	}

	bufferLabel.close();
	bufferStimuli.close();

	tempLabel.close();
	tempStimuli.close();
	tempBuffer.close();

	//currentCalibrationTag = 1;
	currentCalibrationTag = 0; // SLB
}

int CalibrationController::getNumberOfSession() {

	return numberOfCalibrations;
}

int* CalibrationController::mappingIDtoRC(int tag) {

	int* index = new int[2];
	index[0] = ceil(((float) tag) / ((float) numberOfRowInterface));
	index[1] = (tag % numberOfRowInterface) + numberOfRowInterface;
	if (index[1] == numberOfRowInterface)
		index[1] += numberOfRowInterface;

	return index;
}

void CalibrationController::LDAelaboration() {

	int n_trials = tags*numberOfFlash*numberOfCalibrationTag;
	int n_samples = windowLen / sampleRate;
	ifstream readBuffer;	// File temporaneo per la lettura
	ifstream readLabel;		// File temporaneo per la lettura
	ifstream readStimuli;	// File temporaneo per la lettura
	ifstream fileTempBuffer;

	string line;

	float **newBuffer;
	int *newStimuli;
	int *newLabel;

	newBuffer = new float*[numberOfChannels*n_samples];
	for (int i=0; i<numberOfChannels*n_samples; i++) {
		newBuffer[i] = new float[n_trials*numberOfCalibrations*numberOfCalibrationTag];
	}

	newStimuli = new int[n_trials*numberOfCalibrations];
	newLabel = new int[n_trials*numberOfCalibrations];
	
	int iterator_start_label = 0;
	int iterator_stop_label = 0;

	int iterator_start = 0;
	int incr = (numberOfChannels*(windowLen/sampleRate)*numberOfFlash*tags*(numberOfCalibrationTag));
	int iterator_stop = (numberOfChannels*(windowLen / sampleRate)*numberOfFlash*tags*(numberOfCalibrationTag)) - 1;

	int cnt = 0;
	// numberOfSessions*numberOfCalibrationTags_

	for (int k = 0; k < numberOfCalibrationTag*numberOfCalibrations; k++) {
		fileTempBuffer.open((outputFilesRoot + "LDA/tempBuffer.txt").c_str()); // SLB
		//fileTempBuffer.open(outputFilesRoot + "LDA/tempBuffer.txt");

		int iterator = 0;
		cnt++;
		if (k == 1)
			cout << endl;

		if (cnt>numberOfCalibrationTag) {
			iterator_start += incr;
			iterator_stop = iterator_start + incr - 1;
			cnt = 0;
		}

		int i = 0;
		int j = k*tags*numberOfFlash;
		int start = iterator_start + (numberOfFlash*tags*(k%numberOfCalibrationTag));
		int stop = start + (numberOfFlash*tags) - 1;
		
		if (fileTempBuffer.is_open()) {
			while (getline(fileTempBuffer, line)) {

				if (iterator <= stop && start <= iterator && iterator <= iterator_stop) {
					//std::string::size_type sz;     // alias of size_t
					float val = ::atof(line.c_str());
					newBuffer[i][j] = val;
					j++;
					if (j == (numberOfFlash*tags*(k+1))) {
						j = k*tags*numberOfFlash;
						i++;
					}
				}

				if (iterator > stop && iterator <= iterator_stop) {
					start = stop + (numberOfFlash*tags*(numberOfCalibrationTag - 1)) + 1;
					stop = start + (numberOfFlash*tags) - 1;
				}
				iterator++;
			}
		}	

		fileTempBuffer.close();
	}

	int indexI = 0;
	readStimuli.open((outputFilesRoot + "LDA/tempStimuli.txt").c_str()); // SLB
	//readStimuli.open(outputFilesRoot + "LDA/tempStimuli.txt");
	if (readStimuli.is_open()) {
		while (getline(readStimuli, line)) {
			//std::string::size_type sz;     // alias of size_t
			int val = ::atoi(line.c_str());
			newStimuli[indexI] = val;
			indexI++;
		}
	}

	//readStimuli.close();

	indexI = 0;
	readLabel.open((outputFilesRoot + "LDA/tempLabel.txt").c_str()); // SLB
	//readLabel.open(outputFilesRoot + "LDA/tempLabel.txt");
	if (readLabel.is_open()) {
		while (getline(readLabel, line)) {
			//std::string::size_type sz;     // alias of size_t
			int val = ::atoi(line.c_str());
			newLabel[indexI] = val;
			indexI++;
		}
	}

	//readLabel.close();


	float **support1;
	support1 = reshape(newBuffer);

	Winsor* winsor = new Winsor(support1,0.1,numberOfChannels,n_trials*numberOfCalibrations,n_samples);
	winsor->train();
	float **P300CalibrationBufferW = winsor->apply();

	Normalize* normalize = new Normalize(P300CalibrationBufferW,numberOfChannels,n_trials*numberOfCalibrations,n_samples);
	normalize->train();
	float **P300CalibrationBufferN = normalize->apply();

	newBuffer = invReshape(P300CalibrationBufferN);
	classifier = new BayesianLDA(numberOfChannels, n_samples, n_trials*numberOfCalibrations, newBuffer, newLabel, newStimuli, username);
	classifier->train();

	ofstream targetFile;
	targetFile.open((outputFilesRoot + "/target.txt").c_str()); // SLB
	//targetFile.open(outputFilesRoot + "/target.txt");
	for (int j=0;j<numberOfCalibrations;j++)
		for (int i=0; i<numberOfCalibrationTag; i++)
			targetFile << target[i] << endl;
	targetFile.close();

}

void CalibrationController::LDAcalibAnalysis(int a) {

	
	numberOfCalibrations = a;
	
	int n_trials = tags*numberOfFlash*numberOfCalibrationTag;
	int n_samples = windowLen / sampleRate;
	ifstream readBuffer;	// File temporaneo per la lettura
	ifstream readLabel;		// File temporaneo per la lettura
	ifstream readStimuli;	// File temporaneo per la lettura
	ifstream fileTempBuffer;

	string line;

	float **newBuffer;
	int *newStimuli;
	int *newLabel;

	newBuffer = new float*[numberOfChannels*n_samples];
	for (int i=0; i<numberOfChannels*n_samples; i++) {
		newBuffer[i] = new float[n_trials*numberOfCalibrations*numberOfCalibrationTag];
	}

	newStimuli = new int[n_trials*numberOfCalibrations];
	newLabel = new int[n_trials*numberOfCalibrations];
	
	int iterator_start_label = 0;
	int iterator_stop_label = 0;

	int iterator_start = 0;
	int incr = (numberOfChannels*(windowLen/sampleRate)*numberOfFlash*tags*(numberOfCalibrationTag));
	int iterator_stop = (numberOfChannels*(windowLen / sampleRate)*numberOfFlash*tags*(numberOfCalibrationTag)) - 1;

	int cnt = 0;
	// numberOfSessions*numberOfCalibrationTags_

	for (int k = 0; k < numberOfCalibrationTag*numberOfCalibrations; k++) {
		fileTempBuffer.open((outputFilesRoot + "LDA/concBuffer.txt").c_str()); // SLB
		//fileTempBuffer.open(outputFilesRoot + "LDA/concBuffer.txt");

		int iterator = 0;
		cnt++;
		if (k == 1)
			cout << endl;

		if (cnt>numberOfCalibrationTag) {
			iterator_start += incr;
			iterator_stop = iterator_start + incr - 1;
			cnt = 0;
		}

		int i = 0;
		int j = k*tags*numberOfFlash;
		int start = iterator_start + (numberOfFlash*tags*(k%numberOfCalibrationTag));
		int stop = start + (numberOfFlash*tags) - 1;
		
		if (fileTempBuffer.is_open()) {
			while (getline(fileTempBuffer, line)) {

				if (iterator <= stop && start <= iterator && iterator <= iterator_stop) {
					//std::string::size_type sz;     // alias of size_t
					float val = ::atof(line.c_str());
					newBuffer[i][j] = val;
					j++;
					if (j == (numberOfFlash*tags*(k+1))) {
						j = k*tags*numberOfFlash;
						i++;
					}
				}

				if (iterator > stop && iterator <= iterator_stop) {
					start = stop + (numberOfFlash*tags*(numberOfCalibrationTag - 1)) + 1;
					stop = start + (numberOfFlash*tags) - 1;
				}
				iterator++;
			}
		}	

		fileTempBuffer.close();
	}

	int indexI = 0;
	readStimuli.open((outputFilesRoot + "LDA/concStimuli.txt").c_str()); // SLB
	//readStimuli.open(outputFilesRoot + "LDA/concStimuli.txt");
	if (readStimuli.is_open()) {
		while (getline(readStimuli, line)) {
			//std::string::size_type sz;     // alias of size_t
			int val = ::atoi(line.c_str());
			newStimuli[indexI] = val;
			indexI++;
		}
	}

	readStimuli.close();

	indexI = 0;
	readLabel.open((outputFilesRoot + "LDA/concLabel.txt").c_str()); // SLB
	//readLabel.open(outputFilesRoot + "LDA/concLabel.txt");
	if (readLabel.is_open()) {
		while (getline(readLabel, line)) {
			//std::string::size_type sz;     // alias of size_t
			int val = ::atoi(line.c_str());
			newLabel[indexI] = val;
			indexI++;
		}
	}

	readLabel.close();


	float **support1;
	support1 = reshape(newBuffer);

	Winsor* winsor = new Winsor(support1,0.1,numberOfChannels,n_trials*numberOfCalibrations,n_samples);
	winsor->train();
	float **P300CalibrationBufferW = winsor->apply();

	Normalize* normalize = new Normalize(P300CalibrationBufferW,numberOfChannels,n_trials*numberOfCalibrations,n_samples);
	normalize->train();
	float **P300CalibrationBufferN = normalize->apply();

	newBuffer = invReshape(P300CalibrationBufferN);
	classifier = new BayesianLDA(numberOfChannels, n_samples, n_trials*numberOfCalibrations, newBuffer, newLabel, newStimuli, username);
	classifier->train();

	ofstream targetFile;
	targetFile.open((outputFilesRoot + "/target.txt").c_str()); // SLB
	//targetFile.open(outputFilesRoot + "/target.txt");
	for (int j=0;j<numberOfCalibrations;j++)
		for (int i=0; i<numberOfCalibrationTag; i++)
			targetFile << target[i] << endl;
	targetFile.close();

}

void CalibrationController::sendToInterface(int com) {
	
	clientTCP *conn = new clientTCP;
	conn->sendCommand(std::to_string(com).c_str()); // SLB
	delete conn;
}

CEvent& CalibrationController::getFinishStoreSingleTargetDataEvent() {
	
	return _finishStoreSingleTargetData;
}

CEvent& CalibrationController::getFinishStoreAllSessionDataEvent() {

	return _finishStoreAllSessionData;
}

void CalibrationController::openBinFile() {

	time_t now;
	char* timeNow = new char[12];

	//initialize file stream where received data should be written to

	string rootPath = binFilesRoot + "/Calib_"; // SLB
	string x		= "x";
	string und		= "_";
	string number	= std::to_string(numberOfRowInterface);

	string dim		= und + number + x + number;

	string flash;
	flash			= (flashingMode) ? "_RC_" : "_SE_";

	now = time(NULL);
	strftime(timeNow, 80, "%Y_%m_%d_%H_%M_%S", gmtime(&now));

	// SLB
	rootPath += username;
	rootPath += dim;
	rootPath += flash;
	rootPath += timeNow;
	rootPath += ".bin";
	
	outputFile.Open(rootPath.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	
	readConfiguration();			// SLB
	updateConfiguration(rootPath);	// SLB

	// SLB delete[]
	/*
	delete[] timeNow;
	*/
	
}

// SLB
void CalibrationController::openGameBinFile() {

	time_t now;
	char* timeNow = new char[12];

	string rootPath = binFilesRoot + "/GameS_";
	/*
	string x		= "x";
	string und		= "_";
	string number	= std::to_string(numberOfRowInterface);

	string dim		= und + number + x + number;

	string flash	= (flashingMode) ? "_RC_" : "_SE_";
	*/
	now = time(NULL);
	strftime(timeNow, 80, "%Y_%m_%d_%H_%M_%S", gmtime(&now));

	rootPath += username;
	//rootPath += dim;
	//rootPath += flash;
	rootPath += "_";
	rootPath += timeNow;
	rootPath += ".bin";
	
	outputFile.Open(rootPath.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	
	readConfiguration();			// SLB
	//updateConfiguration(rootPath);	// SLB

	// SLB delete[]
	/*
	delete[] timeNow;
	*/
	
}

// SLB
void CalibrationController::openGameTrialBinFile() {
	
	time_t now;
	char* timeNow = new char[12];
	
	string rootPath = binFilesRoot + "/GameT_";
	
	now = time(NULL);
	strftime(timeNow, 80, "%Y_%m_%d_%H_%M_%S", gmtime(&now));
	
	rootPath += username;
	rootPath += "_";
	rootPath += timeNow;
	rootPath += ".bin";
	
	outputTrialFile.Open(rootPath.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	
	readConfiguration();			// SLB
	updateConfiguration(rootPath);	// SLB

	// SLB delete[]
	/*
	delete[] timeNow;
	*/
	
}

// SLB
void CalibrationController::openBaselineBinFile() {

	time_t now;
	char* timeNow = new char[12];

	string rootPath = binFilesRoot + "/Basel_";
	/*
	string x		= "x";
	string und		= "_";
	string number	= std::to_string(numberOfRowInterface);

	string dim		= und + number + x + number;

	string flash	= (flashingMode) ? "_RC_" : "_SE_";
	*/
	now = time(NULL);
	strftime(timeNow, 80, "%Y_%m_%d_%H_%M_%S", gmtime(&now));

	rootPath += username;
	//rootPath += dim;
	//rootPath += flash;
	rootPath += "_";
	rootPath += timeNow;
	rootPath += ".bin";
	
	outputFile.Open(rootPath.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	
	readConfiguration();			// SLB
	updateConfiguration(rootPath);	// SLB

	// SLB delete[]
	/*
	delete[] timeNow;
	*/
	
}

// SLB
void CalibrationController::readConfiguration() {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str());
	if (result) {
		samplerate		= atoi(doc.child("samplerate").first_child().value());
		timeflash		= atoi(doc.child("time_flash").first_child().value());

		number_of_channels_plus = atoi(doc.child("num_ch_plus").first_child().value());

		targets.clear();
		pugi::xml_node node = doc.child("target");
		int i=0;
		for (pugi::xml_node tool = node.child("t"); tool; tool = tool.next_sibling("t")) {
			targets.push_back(atoi(tool.first_child().value()));
			i++;
		}

	}

}

// SLB
void CalibrationController::updateConfiguration(string rootPath) {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str());
	if (result) {
		
		pugi::xml_node node = doc.child("binfilename");
		if (node.first_child().empty())
			node.append_child(pugi::node_pcdata).set_value(rootPath.c_str());
		else
			node.first_child().set_value(rootPath.c_str());
	}

	doc.save_file((configFilesRoot + "/configuration.xml").c_str());

}

// SLB
void CalibrationController::writeBinFile(float* receivedData) {

	// SLB
	int channels = numberOfChannels + number_of_channels_plus;

	// SLB //The (index of the) current calibration tag is added to the second last pseudo-channel
	receivedData[channels] =
		 //(currentCalibrationTag+1 <= numberOfCalibrationTag) ? currentCalibrationTag+1 : currentCalibrationTag;
		 (currentCalibrationTag < numberOfCalibrationTag) ? targets[currentCalibrationTag] : targets[currentCalibrationTag-1];
	// SLB //The tag is repeated the correct numner of times on the last pseudo-channel
	// whilst the tag itself is being displayed on the interface
	if (receivedData[channels + 1] > 0) {
		stimuli_length = 1;
		buffered_tag = receivedData[channels + 1];
		buffered_tag_flag = true;
	}
	int stimuli_burst = samplerate * timeflash / 1000;
	if (buffered_tag_flag) {
		receivedData[channels + 1] = buffered_tag;
		stimuli_length++;
		//if (stimuli_length > numberOfFlash * tags) {
		if (stimuli_length > stimuli_burst) {
			buffered_tag = 0;
			buffered_tag_flag = false;
		}
	}
	//
	outputFile.Write(receivedData, (channels + SERVICE_CHANNELS) * sizeof(float));
}

// SLB
void CalibrationController::writeRawBinFile(float* receivedData) {
	//receivedData[numberOfChannels + 1] =
	//	(currentCalibrationTag < numberOfCalibrationTag) ? currentCalibrationTag : currentCalibrationTag-1;
	// SLB
	int channels = numberOfChannels + number_of_channels_plus;

	outputFile.Write(receivedData, (channels + SERVICE_CHANNELS) * sizeof(float));
}

// SLB
void CalibrationController::writeRawTrialBinFile(float* receivedData) {
	
	int channels = numberOfChannels + number_of_channels_plus;

	outputTrialFile.Write(receivedData, (channels + SERVICE_CHANNELS) * sizeof(float));
}

void CalibrationController::closeBinFile() {

	outputFile.Close();
	//cout << "Calibration binary file closed." << endl; // SLB
	cout << "Binary file closed." << endl; // SLB
}

// SLB
void CalibrationController::closeBaselineBinFile() {

	if (outputFile.m_hFile != CFile::hFileNull) {
		outputFile.Close();
		cout << "Baseline binary file closed." << endl;
	}
}

// SLB
void CalibrationController::closeGameTrialBinFile() {

	outputTrialFile.Close();
	cout << "Game trial binary file closed." << endl;
}