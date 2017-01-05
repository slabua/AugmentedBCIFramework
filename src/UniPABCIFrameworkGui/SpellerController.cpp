#include "SpellerController.h"
#include "Classification.h"
#include "rootpaths.h" // SLB
#include "pugixml.hpp"
#include <vector>

using std::cout;
using std::endl;


SpellerController::SpellerController(bool p_mode, int p_size, int numFlash, int numChannel, int p_windowLen, int p_sampleRate, string p_user) {

	flashingMode		= p_mode;
	size				= p_size;
	numberOfFlash		= numFlash;
	numberOfChannels	= numChannel;
	windowLen			= p_windowLen;
	sampleRate			= p_sampleRate;
	username			= p_user;
	n_samples			= windowLen / sampleRate;
	tags				= (flashingMode) ? size*2 : size*size;
	n_trials			= tags * numberOfFlash;

	P300SpellerBuffer = new float*[numberOfChannels*windowLen];
	for (int i=0; i<numberOfChannels*windowLen; i++)
		P300SpellerBuffer[i] = new float[tags*numberOfFlash];

	int size = (int)(numberOfChannels*windowLen / sampleRate);
	P300SpellerBufferDS = new float*[size];
	for (int i=0; i<size; i++)
		P300SpellerBufferDS[i] = new float[tags*numberOfFlash];

	P300SpellerBufferReshaped = new float*[numberOfChannels];
	for (int i=0; i<numberOfChannels; i++)
		P300SpellerBufferReshaped[i] = new float[numberOfFlash*windowLen*tags];

	openBuffer = new bool[tags*numberOfFlash];
	buffersSize = new int[tags*numberOfFlash];
	stimuli = new int[tags*numberOfFlash];
	for (int i=0; i<tags*numberOfFlash; i++) {
		openBuffer[i] = false;
		buffersSize[i] = 0;
		stimuli[i] = 0;
	}

	// SLB
	buffered_tag = 0;
	buffered_tag_flag = false;
}

void SpellerController::fillBuffer(float *receivedData) {

	// SLB
	int channels = numberOfChannels + number_of_channels_plus;

	if (receivedData[channels + 1] > 0) {
		for (int i=0; i<numberOfFlash*tags; i++) {
			if (buffersSize[i] == 0 && openBuffer[i] == false) {
				openBuffer[i] = true;
				stimuli[i] = receivedData[channels + 1];
				// SLB
				//cout << stimuli[i] << endl;			///////////////////////////
				//
				break;
			}
		}
	}

	for (int i=0; i<numberOfFlash*tags; i++) {
		if (openBuffer[i]) {
			for (int j = 0; j < numberOfChannels; j++) 
				P300SpellerBuffer[j + buffersSize[i]][i] = receivedData[j];
			//buffersSize[i] = buffersSize[i] + numberOfChannels;
			buffersSize[i] += numberOfChannels; // SLB
			if (buffersSize[i] >= windowLen*numberOfChannels)
				openBuffer[i] = false;
		}
	}

	if (buffersSize[tags*numberOfFlash-1] >= windowLen*numberOfChannels)
		_finishedWriteBuffer.SetEvent();

}

void SpellerController::downSampling() {
	int index = 0;
	//for (int i=0; i<windowLen*numberOfChannels; i = i + numberOfChannels*sampleRate) {
	for (int i=0; i<windowLen*numberOfChannels; i += (numberOfChannels*sampleRate)) {
		for (int j = 0; j < numberOfChannels; j++) {
			for (int k = 0; k < tags*numberOfFlash; k++)
				P300SpellerBufferDS[index][k] = P300SpellerBuffer[i + j][k];
			index++;
		}
	}
}

void SpellerController::nextPrediction() {

	for (int i=0; i<tags*numberOfFlash; i++) {
		openBuffer[i] = false;
		buffersSize[i] = 0;
		stimuli[i] = 0;
	}

}

void SpellerController::openBinFile() {

	time_t now;
	char* timeNow = new char[12];

	//initialize file stream where received data should be written to

	string rootPath = binFilesRoot + "/Spell_"; // SLB
	string x		= "x";
	string und		= "_";
	string number	= std::to_string(size);
	
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
void SpellerController::readConfiguration() {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str());
	if (result) {
		samplerate		= atoi(doc.child("samplerate").first_child().value());
		timeflash		= atoi(doc.child("time_flash").first_child().value());

		number_of_channels_plus = atoi(doc.child("num_ch_plus").first_child().value());

	}

}

// SLB
void SpellerController::updateConfiguration(string rootPath) {

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
void SpellerController::writeBinFile(float* receivedData) {

	// SLB
	int channels = numberOfChannels + number_of_channels_plus;

	// SLB //The index of the current calibration tag is added to the second last pseudo-channel
	receivedData[channels] = FILETYPE_SPELLING; // Writes constantly -1 as there's always only 1 target per spelling session.
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

void SpellerController::closeBinFile() {

	outputFile.Close();
	cout << "Speller binary file closed." << endl; // SLB
}

CEvent& SpellerController::getFinishedWriteBufferEvent() {

	return _finishedWriteBuffer;
}

void SpellerController::reshape() { 

	int index = 0; 
	int k;
	for (int i=0; i<numberOfChannels; i++) {
		index = 0; 
		k = i;
		for (; index < (tags*numberOfFlash*windowLen/sampleRate); k += numberOfChannels) {
			for (int j = 0; j < tags*numberOfFlash; j++) {
				P300SpellerBufferReshaped[i][index] = P300SpellerBufferDS[k][j];
				index++;
			}
		}
	}
}

void SpellerController::invReshape() {
	//int index[4] = { 0, 0, 0, 0 };
	// SLB
	std::vector <int> index;
	for (int i=0; i<numberOfChannels; i++) {
		index.push_back(0);
	}
	//
	for (int i=0; i<windowLen; i++) {
		for (int j = 0; j < tags*numberOfFlash; j++) {
			P300SpellerBufferDS[i][j] = P300SpellerBufferReshaped[i % numberOfChannels][index[i % numberOfChannels]];
			index[i % numberOfChannels]++;
		}
	}
}

/* SLB TODO to remove
void SpellerController::printFile() {

	ofstream test;
	test.open("F:/Utenti/Walter/Desktop/buffer.txt"); // La matrice è (4*50)X(8*9) => 200X72
													  // deve diventare 4X(50*8*9) => 4X3600
	for (int j = 0; j < tags*numberOfFlash; j++) {
		for (int i=0; i<numberOfChannels*windowLen; i++)
			test << P300SpellerBuffer[i][j] << endl;
		test << endl << endl;
	}
	test.close();

}
*/

int* SpellerController::prediction() {

	downSampling();

	int n_runs = 1;
	Classification* classifier = new Classification(P300SpellerBufferDS,stimuli,n_runs,tags,numberOfFlash,numberOfChannels,n_samples,n_trials,sampleRate,windowLen,flashingMode,username);
	int* predicted = classifier->classify();

	//cout << predicted[0] << endl;
	cout << predicted[0]; // SLB
	if (predicted[1] != -1)
		cout << " - " << predicted[1];
	cout << endl;

	return predicted;

}
