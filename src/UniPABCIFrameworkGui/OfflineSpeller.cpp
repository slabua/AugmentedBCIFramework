#include "OfflineSpeller.h"
#include <iostream>
#include <fstream>
#include "BayesianLDA.h"
#include "rootpaths.h" //SLB
//#include "pugixml.hpp"

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;


OfflineSpeller::OfflineSpeller(int p_numberOfFlash, int p_numberOfTag, int p_numberOfCalibrationTags, 
								int p_numberOfChannels, int p_sampledWindow, int p_numberOfSessions, 
								int p_sampleRate, bool p_flashingMode, int p_rows, string p_user) {
	numberOfFlash = p_numberOfFlash;
	numberOfTag = p_numberOfTag;
	numberOfCalibrationTags = p_numberOfCalibrationTags;
	numberOfChannels = p_numberOfChannels;
	sampledWindow = p_sampledWindow;
	numberOfSessions = p_numberOfSessions;
	sampleRate = p_sampleRate;
	flashingMode = p_flashingMode;
	rowsNumber = p_rows;
	username = p_user;

	w = new float[(numberOfChannels*sampledWindow) + 1];
	stimuli = new int[numberOfFlash*numberOfTag];
	target = new int[numberOfCalibrationTags*numberOfSessions];
	buffers = new float*[numberOfChannels*sampledWindow];
	for (int i=0; i<numberOfChannels*sampledWindow; i++)
		buffers[i] = new float[numberOfTag*numberOfFlash];
}

OfflineSpeller::~OfflineSpeller() {
	for (int i=0; i<numberOfChannels*sampledWindow; i++)
		delete buffers[i];
	delete buffers;
	delete classification;
}

/*
void OfflineSpeller::configBuffer(vector<int> indexCalib) {

	ofstream tempBuffer;
	tempBuffer.open("./OutputFileLDA/tempBufferProva.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	string line;
	for (int i=0; i<indexCalib.size(); i++) {
		ifstream singleCalib;
		singleCalib.open("./OutputFileLDA/bufferCalib_"+to_string(indexCalib[i])+".txt");
		if (singleCalib.is_open()) {
			while (getline(singleCalib, line)) {
				std::string::size_type sz;     // alias of size_t
				float val = ::atoi(line.c_str());
				tempBuffer << val;
				}
			}
		singleCalib.close();
	}

	tempBuffer.close();
}
*/

int OfflineSpeller::mappingNumbers(int indice1, int indice2) {
	int i = indice1 - 1;
	int j = indice2 - 1;
	return ((i*rowsNumber) + (j - rowsNumber))+1;
}

int OfflineSpeller::calculateNumberOfFlash() {
	BayesianLDA *weights;
	ifstream fileBuffer;
	ifstream fileStimuli;
	ifstream fileLabel;
	ifstream fileTarget;
	ifstream fileTempBuffer;
	ifstream fileTempStimuli;
	ifstream fileTempLabel;

	string line;
	string name;

	int ii = 0;

	name = outputFilesRoot + "/target.txt";
	fileTarget.open(name.c_str()); // SLB
	//fileTarget.open(name);
	ii = 0;

	if (fileTarget.is_open()) {
		while (getline(fileTarget, line)) {
			//std::string::size_type sz;     // alias of size_t
			float val = ::atoi(line.c_str());
			target[ii] = val;
			ii++;
		}
	}
	fileTarget.close();

	float **tempBuffer;
	int *tempLabel;
	int *tempStimuli;

	tempBuffer = new float*[sampledWindow*numberOfChannels];
	for (int i=0; i<sampledWindow*numberOfChannels; i++)
		tempBuffer[i] = new float[numberOfFlash*numberOfTag];

	tempLabel = new int[numberOfFlash*numberOfTag];
	tempStimuli = new int[numberOfFlash*numberOfTag];

	int *index = new int[numberOfSessions];

	int iterator_start_label = 0;
	int iterator_stop_label = 0;

	int iterator_start = 0;
	int incr = (numberOfChannels*sampledWindow*numberOfFlash*numberOfTag*(numberOfCalibrationTags));
	int iterator_stop = (numberOfChannels*sampledWindow*numberOfFlash*numberOfTag*(numberOfCalibrationTags)) - 1;

	float *percentuale = new float[numberOfFlash];
	for (int g = 0; g < numberOfFlash; g++)
		percentuale[g] = 0;

	
	int cnt = 0;
	// numberOfSessions*numberOfCalibrationTags
	for (int k = 0; k < numberOfSessions*numberOfCalibrationTags; k++) {
		int iterator = 0;
		cnt++;
		if (cnt>numberOfCalibrationTags) {
			iterator_start += incr;
			iterator_stop = iterator_start + incr - 1;
			cnt = 0;
		}

		iterator_start_label = numberOfFlash*numberOfTag*(k);
		iterator_stop_label = numberOfFlash*numberOfTag*(k+1);

		cout << "INIZIO " << iterator_start << endl;
		cout << "FINE " << iterator_stop << endl;

		name = outputFilesRoot + "LDA/tempBuffer.txt";
		fileTempBuffer.open(name.c_str()); // SLB
		//fileTempBuffer.open(name);

		int i = 0;
		int j = 0;
		int start = iterator_start + (numberOfFlash*numberOfTag*(k%numberOfCalibrationTags));
		int stop = start + (numberOfFlash*numberOfTag) - 1;

		if (fileTempBuffer.is_open()) {
			while (getline(fileTempBuffer, line)) {
				if (iterator <= stop && start <= iterator && iterator<iterator_stop) {
					//std::string::size_type sz;     // alias of size_t
					float val = ::atof(line.c_str());
					tempBuffer[i][j] = val;
					j++;
					if (j == numberOfFlash*numberOfTag) {
						j = 0;
						i++;
					}
				}

				if (iterator > stop && iterator<iterator_stop) {
					start = stop + (numberOfFlash*numberOfTag*(numberOfCalibrationTags - 1)) + 1;
					stop = start + (numberOfFlash*numberOfTag) - 1;
				}
				iterator++;
			}
		}
		
		cout << "ITERATOR " << iterator << endl;
		name = outputFilesRoot + "LDA/tempLabel.txt";
		fileTempLabel.open(name.c_str()); // SLB
		//fileTempLabel.open(name);
		iterator = 0;
		i = 0;

		if (fileTempLabel.is_open()) {
			while (getline(fileTempLabel, line)) {
				if (iterator < iterator_stop_label && iterator_start_label <= iterator) {
					//std::string::size_type sz;     // alias of size_t
					int val = ::atoi(line.c_str());
					tempLabel[i] = val;
					i++;
				}
				iterator++;
			}
		}

		name = outputFilesRoot + "LDA/tempStimuli.txt";
		fileTempStimuli.open(name.c_str()); // SLB
		//fileTempStimuli.open(name);
		iterator = 0;
		i = 0;

		if (fileTempStimuli.is_open()) {
			while (getline(fileTempStimuli, line)) {
				if (iterator < iterator_stop_label && iterator_start_label <= iterator) {
					//std::string::size_type sz;     // alias of size_t
					int val = ::atoi(line.c_str());
					tempStimuli[i] = val;
					i++;
				}
				iterator++;
			}
		}
							
		classification = new Classification(tempBuffer, tempStimuli, 1, numberOfTag, numberOfFlash,
		numberOfChannels, sampledWindow, numberOfFlash*numberOfTag, sampleRate, sampledWindow*sampleRate, flashingMode, username);

		int *predizione = classification->flashVector();

		cout << "Target[" << k << "]: " << target[k] << endl;
		
		if (flashingMode) {
			int j = 0;
			for (int i=0; i<numberOfFlash * 2; i+=2) {
				cout << "Predizione[" << j << "]: " << predizione[i] << " - " << predizione[i + 1] << endl;
				j++;
			}
			j = 0;
			for (int i=0; i<numberOfFlash*2; i+=2) {
				cout << mappingNumbers(predizione[i], predizione[i + 1]) << endl;
				if (mappingNumbers(predizione[i],predizione[i+1]) == target[k]) {
					index[k] = j;
					break;
				}
				j++;
			}
			j = 0;
			for (int i=0; i<numberOfFlash * 2; i += 2) {
				if (mappingNumbers(predizione[i], predizione[i + 1]) == target[k]) {
					percentuale[j]++;
				}
				j++;
			}
		} else {
			for (int i=0; i<numberOfFlash; i++) {
				cout << "Predizione[" << i << "]: " << predizione[i] << endl;
			}
			for (int i=0; i<numberOfFlash; i++)
				if (predizione[i] == target[k]) {
					index[k] = i;
					break;
				}

			for (int i=0; i<numberOfFlash; i++)
				if (predizione[i] == target[k]) {
					percentuale[i]++;
				}
		}

		fileTempBuffer.close();
		fileTempLabel.close();
		fileTempStimuli.close();
	}

	int maxIndex = 0;
	int maxScore = index[0];
	for (int k = 0; k < numberOfSessions*numberOfCalibrationTags; k++) {
		if (index[k] > maxScore) {
			maxIndex = k;
			maxScore = index[k];
		}
	}
	cout << endl << endl;
	ofstream flash;
	flash.open((outputFilesRoot + "/flash.txt").c_str()); // SLB
	//flash.open(outputFilesRoot + "/flash.txt");
	bool perc100 = false;
	for (int k = 0; k < numberOfFlash; k++) {
		percentuale[k] /= (numberOfCalibrationTags*numberOfSessions);
		percentuale[k] *= 100;
		cout << "Percentuale[" << k << "]: " << percentuale[k] << endl;
		flash << percentuale[k] << endl;
		if (percentuale[k] >= 99)
			perc100 = true;
	}

	flash.close();
	return (perc100 ? maxScore + 1 : numberOfFlash);
}

int OfflineSpeller::calculateNumberOfFlashCalibAnalisys() {
	BayesianLDA *weights;
	ifstream fileBuffer;
	ifstream fileStimuli;
	ifstream fileLabel;
	ifstream fileTarget;
	ifstream fileTempBuffer;
	ifstream fileTempStimuli;
	ifstream fileTempLabel;

	string line;
	string name;

	int ii = 0;

	name = outputFilesRoot + "/target.txt";
	fileTarget.open(name.c_str()); // SLB
	//fileTarget.open(name);
	ii = 0;

	if (fileTarget.is_open()) {
		while (getline(fileTarget, line)) {
			//std::string::size_type sz;     // alias of size_t
			float val = ::atoi(line.c_str());
			target[ii] = val;
			ii++;
		}
	}
	fileTarget.close();

	float **tempBuffer;
	int *tempLabel;
	int *tempStimuli;

	tempBuffer = new float*[sampledWindow*numberOfChannels];
	for (int i=0; i<sampledWindow*numberOfChannels; i++)
		tempBuffer[i] = new float[numberOfFlash*numberOfTag];

	tempLabel = new int[numberOfFlash*numberOfTag];
	tempStimuli = new int[numberOfFlash*numberOfTag];

	int *index = new int[numberOfSessions];

	int iterator_start_label = 0;
	int iterator_stop_label = 0;

	int iterator_start = 0;
	int incr = (numberOfChannels*sampledWindow*numberOfFlash*numberOfTag*(numberOfCalibrationTags));
	int iterator_stop = (numberOfChannels*sampledWindow*numberOfFlash*numberOfTag*(numberOfCalibrationTags)) - 1;

	float *percentuale = new float[numberOfFlash];
		for (int g = 0; g < numberOfFlash; g++)
			percentuale[g] = 0;

	
	int cnt = 0;
	// numberOfSessions*numberOfCalibrationTags
	for (int k = 0; k < numberOfSessions*numberOfCalibrationTags; k++) {
		int iterator = 0;
		cnt++;
		if (cnt>numberOfCalibrationTags) {
			iterator_start += incr;
			iterator_stop = iterator_start + incr - 1;
			cnt = 0;
		}

		iterator_start_label = numberOfFlash*numberOfTag*(k);
		iterator_stop_label = numberOfFlash*numberOfTag*(k+1);

		cout << "INIZIO " << iterator_start << endl;
		cout << "FINE " << iterator_stop << endl;

		name = outputFilesRoot + "LDA/concBuffer.txt";
		fileTempBuffer.open(name.c_str()); // SLB
		//fileTempBuffer.open(name);

		int i = 0;
		int j = 0;
		int start = iterator_start + (numberOfFlash*numberOfTag*(k%numberOfCalibrationTags));
		int stop = start + (numberOfFlash*numberOfTag) - 1;

		if (fileTempBuffer.is_open()) {
			while (getline(fileTempBuffer, line)) {
				if (iterator <= stop && start <= iterator && iterator<iterator_stop) {
					//std::string::size_type sz;     // alias of size_t
					float val = ::atof(line.c_str());
					tempBuffer[i][j] = val;
					j++;
					if (j == numberOfFlash*numberOfTag) {
						j = 0;
						i++;
					}
				}

				if (iterator > stop && iterator<iterator_stop) {
					start = stop + (numberOfFlash*numberOfTag*(numberOfCalibrationTags - 1)) + 1;
					stop = start + (numberOfFlash*numberOfTag) - 1;
				}
				iterator++;
			}
		}
		
		cout << "ITERATOR " << iterator << endl;
		name = outputFilesRoot + "LDA/concLabel.txt";
		fileTempLabel.open(name.c_str()); // SLB
		//fileTempLabel.open(name);
		iterator = 0;
		i = 0;

		if (fileTempLabel.is_open()) {
			while (getline(fileTempLabel, line)) {
				if (iterator < iterator_stop_label && iterator_start_label <= iterator) {
					//std::string::size_type sz;     // alias of size_t
					int val = ::atoi(line.c_str());
					tempLabel[i] = val;
					i++;
				}
				iterator++;
			}
		}

		name = outputFilesRoot + "LDA/concStimuli.txt";
		fileTempStimuli.open(name.c_str()); // SLB
		//fileTempStimuli.open(name);
		iterator = 0;
		i = 0;

		if (fileTempStimuli.is_open()) {
			while (getline(fileTempStimuli, line)) {
				if (iterator < iterator_stop_label && iterator_start_label <= iterator) {
					//std::string::size_type sz;     // alias of size_t
					int val = ::atoi(line.c_str());
					tempStimuli[i] = val;
					i++;
				}
				iterator++;
			}
		}
							
		classification = new Classification(tempBuffer, tempStimuli, 1, numberOfTag, numberOfFlash,
		numberOfChannels, sampledWindow, numberOfFlash*numberOfTag, sampleRate, sampledWindow*sampleRate, flashingMode, username);

		int *predizione = classification->flashVector();

		cout << "Target[" << k << "]: " << target[k] << endl;
		
		if (flashingMode) {
			int j = 0;
			for (int i=0; i<numberOfFlash * 2; i+=2) {
				cout << "Predizione[" << j << "]: " << predizione[i] << " - " << predizione[i + 1] << endl;
				j++;
			}
			j = 0;
			for (int i=0; i<numberOfFlash*2; i+=2) {
				cout << mappingNumbers(predizione[i], predizione[i + 1]) << endl;
				if (mappingNumbers(predizione[i],predizione[i+1]) == target[k]) {
					index[k] = j;
					break;
				}
				j++;
			}
			j = 0;
			for (int i=0; i<numberOfFlash * 2; i += 2) {
				if (mappingNumbers(predizione[i], predizione[i + 1]) == target[k]) {
					percentuale[j]++;
				}
				j++;
			}
		} else {
			for (int i=0; i<numberOfFlash; i++) {
				cout << "Predizione[" << i << "]: " << predizione[i] << endl;
			}
			for (int i=0; i<numberOfFlash; i++)
				if (predizione[i] == target[k]) {
					index[k] = i;
					break;
				}

			for (int i=0; i<numberOfFlash; i++)
				if (predizione[i] == target[k]) {
					percentuale[i]++;
				}
		}

		fileTempBuffer.close();
		fileTempLabel.close();
		fileTempStimuli.close();
	}

	int maxIndex = 0;
	int maxScore = index[0];
	for (int k = 0; k < numberOfSessions*numberOfCalibrationTags; k++) {
		if (index[k] > maxScore) {
			maxIndex = k;
			maxScore = index[k];
		}
	}
	cout << endl << endl;
	ofstream flash;
	flash.open((outputFilesRoot + "/flash.txt").c_str()); // SLB
	//flash.open(outputFilesRoot + "/flash.txt");
	bool perc100 = false;
	for (int k = 0; k < numberOfFlash; k++) {
		percentuale[k] /= (numberOfCalibrationTags*numberOfSessions);
		percentuale[k] *= 100;
		cout << "Percentuale[" << k << "]: " << percentuale[k] << endl;
		flash << percentuale[k] << endl;
		if (percentuale[k] >= 99)
			perc100 = true;
	}

	flash.close();
	return (perc100 ? maxScore + 1 : numberOfFlash);
}

int OfflineSpeller::calculateNumberOfFlashOnline(int targetP) {

	ifstream fileTarget;
	ifstream fileTempBuffer;
	ifstream fileTempStimuli;
	ifstream fileTempLabel;

	string line;
	string name;

	float **tempBuffer;
	int *tempLabel;
	int *tempStimuli;

	tempBuffer = new float*[(sampledWindow*numberOfChannels)+1];
	for (int i=0; i<(sampledWindow*numberOfChannels)+1; i++)
		tempBuffer[i] = new float[numberOfFlash*numberOfTag];

	tempLabel = new int[numberOfFlash*numberOfTag];
	tempStimuli = new int[numberOfFlash*numberOfTag];

	int *index = new int[numberOfSessions];

	int iterator_start_label = 0;
	int iterator_stop_label = 0;

	int iterator_start = 0;
	int incr = (numberOfChannels*sampledWindow*numberOfFlash*numberOfTag*(numberOfCalibrationTags));
	int iterator_stop = (numberOfChannels*sampledWindow*numberOfFlash*numberOfTag*(numberOfCalibrationTags)) - 1;

	float *percentuale = new float[numberOfFlash];
	for (int g = 0; g < numberOfFlash; g++)
		percentuale[g] = 0;


	int cnt = 0;
	// numberOfSessions*numberOfCalibrationTags
	for (int k = 0; k < numberOfSessions*numberOfCalibrationTags; k++) {
		int iterator = 0;
		cnt++;
		if (cnt>numberOfCalibrationTags) {
			iterator_start += incr;
			iterator_stop = iterator_start + incr - 1;
			cnt = 0;
		}

		iterator_start_label = numberOfFlash*numberOfTag*(k);
		iterator_stop_label = numberOfFlash*numberOfTag*(k + 1);

		cout << "START " << iterator_start << endl;
		cout << "END " << iterator_stop << endl;

		name = outputFilesRoot + "LDA/bufferSpeller.txt";
		fileTempBuffer.open(name.c_str()); // SLB
		//fileTempBuffer.open(name);

		int i = 0;
		int j = 0;
		
		if (fileTempBuffer.is_open()) {
			while (getline(fileTempBuffer, line)) {
					//std::string::size_type sz;     // alias of size_t
					float val = ::atof(line.c_str());
					tempBuffer[i][j] = val; // SLB crashes using the speller from symbolic mode
					j++;
					if (j == numberOfFlash*numberOfTag) {
						j = 0;
						i++;
					}
			}
		}

		fileTempBuffer.close();
		/*cout << "ITERATOR " << iterator << endl;
		name = "./OutputFileLDA/tempLabel.txt";
		fileTempLabel.open(name);
		iterator = 0;
		i = 0;

		if (fileTempLabel.is_open()) {
			while (getline(fileTempLabel, line)) {
				if (iterator < iterator_stop_label && iterator_start_label <= iterator) {
					std::string::size_type sz;     // alias of size_t
					int val = ::atoi(line.c_str());
					tempLabel[i] = val;
					i++;
				}
				iterator++;
			}
		}
		*/
		name = outputFilesRoot + "LDA/stimuliSpeller.txt";
		fileTempStimuli.open(name.c_str()); // SLB
		//fileTempStimuli.open(name);
		iterator = 0;
		i = 0;

		if (fileTempStimuli.is_open()) {
			while (getline(fileTempStimuli, line)) {
					//std::string::size_type sz;     // alias of size_t
					int val = ::atoi(line.c_str());
					tempStimuli[i] = val;
					i++;
			}
		}

		fileTempStimuli.close();

		/// SLB TODO CHECK crash
		/* SLB absent in originalcd
		pugi::xml_document doc;
		//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
		pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
		
		if (result) {
			numberOfFlash = atoi(doc.child("num_flash").first_child().value());
			// SLB TODO fixes symbolic speller crash at size 3*3 but introduces a further crash upon BACK and Speller res
			rowsNumber = atoi(doc.child("row_num_interface").first_child().value());
			numberOfTag	= (flashingMode) ? rowsNumber*2 : rowsNumber*rowsNumber;
			//
		}
		*/
		///

		classification = new Classification(tempBuffer, tempStimuli, 1, numberOfTag, numberOfFlash,
			numberOfChannels, sampledWindow, numberOfFlash*numberOfTag, sampleRate, sampledWindow*sampleRate, flashingMode, username);

		/* SLB
		if (result) {
			// SLB TODO fixes symbolic speller crash at size 3*3 but introduces a further crash upon BACK and Speller res
			rowsNumber = atoi(doc.child("row_num_interface").first_child().value());
			numberOfTag	= (flashingMode) ? rowsNumber*2 : rowsNumber*rowsNumber;
			//
		}
		*/

		int *predizione = classification->flashVector();

		cout << "Target: " << targetP << endl;

		if (flashingMode) {
			for (int i=0; i<numberOfFlash*2; i+=2) {
				cout << "Predizione[" << i << "]: " << predizione[i] << " - " << predizione[i+1] << endl;
			}
			int j = 0;
			for (int i=0; i<numberOfFlash*2; i+=2) {
				if (mappingNumbers(predizione[i], predizione[i+1]) == targetP) {
					index[k] = j;
					break;
				}
				j++;
			}
			j = 0;
			for (int i=0; i<numberOfFlash*2; i+=2) {
				if (mappingNumbers(predizione[i], predizione[i+1]) == targetP) {
					percentuale[j]++;
				}
				j++;
			}
		}
		else {
			for (int i=0; i<numberOfFlash; i++) {
				cout << "Predizione[" << i << "]: " << predizione[i] << endl;
			}
			for (int i=0; i<numberOfFlash; i++) {
				if (predizione[i] == targetP) {
					index[k] = i;
					break;
				}
			}
			for (int i=0; i<numberOfFlash; i++) {
				if (predizione[i] == targetP) {
					percentuale[i]++;
				}
			}
		}
	}

	int maxIndex = 0;
	int maxScore = index[0];
	for (int k=0; k < numberOfSessions*numberOfCalibrationTags; k++) {
		if (index[k] > maxScore) {
			maxIndex = k;
			maxScore = index[k];
		}
	}

	//cout << endl << endl; // SLB commented
	ofstream flash;
	flash.open((outputFilesRoot + "/flash.txt").c_str()); // SLB
	//flash.open(outputFilesRoot + "/flash.txt");
	bool perc100 = false;
	for (int k=0; k < numberOfFlash; k++) {
		percentuale[k] /= (numberOfCalibrationTags*numberOfSessions);
		percentuale[k] *= 100;
		cout << "Percentuale[" << k << "]: " << percentuale[k] << endl;
		flash << percentuale[k] << endl;
		if (percentuale[k] >= 99)
			perc100 = true;
	}

	flash.close();
	return (perc100 ? maxScore+1 : numberOfFlash);
}