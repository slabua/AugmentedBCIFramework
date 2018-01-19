/*****************************************************************************
* 
* UniPA BCI Framework: an Augmented BCI Framework
* 
* Copyright (C) 2016-2017 Salvatore La Bua (slabua@gmail.com)
* RoboticsLab, Department of Industrial and Digital Innovation (DIID),
* Universita'  degli Studi di Palermo, V. delle Scienze, Palermo, Italy.
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

#include "P300Framework.h"
#include "clientTCP.h"
#include "pugixml.hpp"
#include "rootpaths.h" // SLB

using std::cout;
using std::endl;


//P300Framework::P300Framework() {
P300Framework::P300Framework(Gui* gui) {

	guiObject = gui; // SLB

	readConfiguration();

	counter_stimuli			= 1;
	sampleRate				= 4;
	//number_of_channels	= 4;	// SLB variable is read from configuration file
	seconds_running			= 10000;
	buffer_size				= 9000;
	number_of_scan			= 1;
	buffer_size_second		= 10;
	//sample_rate_hz		= 256;	// sampleRate // SLB variable is read from configuration file
	//ocular_filter			= 60;
	LPSTR _slaveSerials[]	= { "" };

	controller = new CalibrationController(number_of_channels-number_of_channels_plus,window_len,number_of_calibration_tag,number_of_flash,number_of_tags,sampleRate,flashingMode,number_of_row_interface,username);
	controller->setIdTag(target_of_calibration);

	listener = new InterfaceListener();

	module = new P300SignalAcquisition(_slaveSerials,0,"UA-2006.02.04",buffer_size_second,sample_rate_hz,number_of_channels,number_of_scan);
	
	int allocatedChannels = module->getNumDevices() * number_of_scan * number_of_channels + SERVICE_CHANNELS;
	receivedData = new float[allocatedChannels];
	// SLB
	for (int i = 0; i < allocatedChannels; i++) {
		receivedData[i] = 0;
	}
	
	// SLB
	if (bioHybridMode) {
		featuresExtractor	= new FeatureExtraction();
		engineenabled		= featuresExtractor->InitialiseEngine();
	}
	actionSelector			= new ActionSelection();
	//selectorenabled		= actionSelector->InitialiseEngine();
	responseModulator		= new ResponseModulation();
	//
	baselineAcquisitionStarted = false;	// SLB
}

void P300Framework::readConfiguration() {

	pugi::xml_document doc;
	//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	
	if (result) {
		username					= doc.child("name").first_child().value();				
		window_len					= atoi(doc.child("window").first_child().value());
		number_of_calibration_tag	= strlen(doc.child("string").first_child().value());
		int mode					= atoi(doc.child("mode").first_child().value());
		number_of_row_interface		= atoi(doc.child("row_num_interface").first_child().value());
		number_of_flash				= atoi(doc.child("num_flash").first_child().value());
		//number_of_channels			= atoi(doc.child("num_ch").first_child().value());				// SLB
		
		int num_ch					= atoi(doc.child("num_ch").first_child().value());					// SLB
		number_of_channels_plus		= atoi(doc.child("num_ch_plus").first_child().value());				// SLB
		number_of_channels			= num_ch + number_of_channels_plus;									// SLB

		signalGain					= atoi(doc.child("signalgain").first_child().value());				// SLB
		
		sample_rate_hz				= atoi(doc.child("samplerate").first_child().value());				// SLB
		bioHybridMode				= (bool) atoi(doc.child("biohybridmode").first_child().value());	// SLB
		
		if (mode == 0) { // modalità flash square
			flashingMode = false;
		}
		//else { //rows-cols
		else if (mode == 1) { // rows-cols // SLB
			flashingMode = true;
		}
		number_of_tags	= (flashingMode) ? number_of_row_interface*2 : number_of_row_interface*number_of_row_interface;
		
		target_of_calibration		= new int[number_of_calibration_tag];
		pugi::xml_node tmp=doc.child("target");
		int i=0;
		for (pugi::xml_node tool = tmp.child("t"); tool; tool = tool.next_sibling("t")) {
			target_of_calibration[i] = atoi(tool.first_child().value());
			i++;
		}

		tobiienabled				= (bool) atoi(doc.child("tobiienabled").first_child().value());

	}
	else
		cout << "Could not find configuration file." << endl;
};

void P300Framework::thread_initialization() {

	try	{
		cout << "Opening and initializing devices..." << "\n";

		module->OpenAndInitDevice();
		
		_networkTagThread = AfxBeginThread(listener->ListenNetwork, NULL, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED);
		_networkTagThread->ResumeThread();

		listener->startAcquisition();

	}
	catch (string& exception) {
		/* SLB Locks the gui
		cout << exception << "\n\nPress enter to exit...";
		getchar();
		return;
		*/
	}
}

// SLB
void P300Framework::thresholdAndScale() {

	for (int i=0; i <number_of_channels; i++) {
		if (receivedData[i] >  ocular_filter)
			receivedData[i] =  ocular_filter;
		if (receivedData[i] < -ocular_filter)
			receivedData[i] = -ocular_filter;
	}

	signalGainScale();
}

// SLB
void P300Framework::signalGainScale() {

	for (int i=0; i <number_of_channels; i++) {
		receivedData[i] *= signalGain;
	}
}

void P300Framework::calibration() {

	int errorCode;
	string errorMessage;

	module->StartAcquisition();
	controller->openBinFile();

	try {
		//in attesa dell'evento che indica l'avvio dell'interfaccia (cod 100)
		WaitForSingleObject(listener->getStartedCalibrationEvent().m_hObject, INFINITE);

		//se arriva l'evento di avvio interfaccia, può partire il thread di acquisizione dei dati
		_dataAcquisitionThread = AfxBeginThread(module->DoAcquisition, NULL, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED);
		_dataAcquisitionThread->ResumeThread();
		
		cout << "Started!\n";
		//while continua finchè non arriva evento che indica la fine della scrittura sui buffer dei dati acquisiti
		while (!WaitForSingleObject(controller->getFinishStoreAllSessionDataEvent().m_hObject, 0) == WAIT_OBJECT_0) {
			//quando arriva evento di inizio stimolo, si resetta il buffer 
			if (WaitForSingleObject(listener->getCallNextStimulus().m_hObject, 0) == WAIT_OBJECT_0) {
				module->resetBuffer();
				listener->setStartedInterface(true);
			}
			//scrittura su receivedData dei dati acquisiti dalla bci
			if (!module->ReadData(receivedData, &errorCode, &errorMessage)) {
				if (errorCode == 2)
					continue;
				else {
					cout << "ERROR" << endl;
					throw errorMessage;
				}
			}

			//aggiunzione del canale tempo a receivedData // SLB never used?
			receivedData[number_of_channels] = 0;

			//se arriva evento che indica l'arrivo di un nuovo tag dall'interfaccia, lo si aggiunge al canale tag di receivedData
			if (WaitForSingleObject(listener->getSymbolTagEvent().m_hObject, 0) == WAIT_OBJECT_0) {
				receivedData[number_of_channels + 1] = listener->getSymbolTag();
			}
			else {
				listener->getSymbolTagEvent().ResetEvent();
				receivedData[number_of_channels + 1] = 0;  //qua andrà il tag ottenuto dall'interfaccia
			}

			receivedData[number_of_channels + 2] = 0;

			thresholdAndScale(); // SLB

			//in contemporanea alla ricezione dei dati si avvia la scrittura dei buffer
			if (listener->isStartedInterface()) {
				
				//applicazione del filtraggio spaziale e scrittura sui buffer
				controller->fillBuffer(receivedData);
				controller->writeBinFile(receivedData);

				guiObject->setReceivedData(receivedData);

				// nel momento in cui arriva evento di fine scrittura del buffer per il target corrente invio comando di riavvio all'interfaccia
				if (WaitForSingleObject(controller->getFinishStoreSingleTargetDataEvent().m_hObject, 0) == WAIT_OBJECT_0) {
					if (counter_stimuli < number_of_calibration_tag) {
						counter_stimuli++; // SLB moved here
						if (flashingMode) {
							///counter_stimuli++;
							controller->sendToInterface(500);			//comando di riavvio per flashRC
						}
						else {
							///counter_stimuli++;
							controller->sendToInterface(600);			//comando di riavvio per flashSquare
						}
					}
				}

				//nel momento in cui arriva evento di fine calibrazione dall'interfaccia, si stoppa thread di ricezione tag
				if (WaitForSingleObject(listener->getFinishCalibrationEvent().m_hObject, 0) == WAIT_OBJECT_0) {
					//listener->stopAcquisition();
					cout << "Ho chiuso il thread di InterfaceListener" << endl;
				}
			}
		}

		//stop del thread di acquisizione dei dati dalla bci
		//module->StopAcquisition();
		cout << "Ho chiuso il thread di P300SignalAcquisition" << endl;
		controller->closeBinFile();
		//close devices
		//module->CloseDevices();
		Sleep(2000);
		_dataAcquisitionThread->SuspendThread();

		//return? // SLB TEST

	}
	catch (string& exception) {
		//an exception occured
		cout << "\t" << exception << "\n";
	}
}

void P300Framework::ldaClassification() {

	cout << "Starting BLDA" << endl;
	controller->LDAelaboration();
	cout << "BLDA completed" << endl;	
}

int P300Framework::offlineSpellerStart() {

	int n_session = controller->getNumberOfSession();
	offspeller = new OfflineSpeller(number_of_flash,number_of_tags,number_of_calibration_tag,number_of_channels-number_of_channels_plus,(window_len/sampleRate),n_session,sampleRate,flashingMode,number_of_row_interface,username);
	//offspeller->configBuffer(indexCalib);
	int n_flash = offspeller->calculateNumberOfFlash();
	return n_flash;
}

void P300Framework::resetCalibration() {

	counter_stimuli = 1;
	controller->nextCalibration();
}

void P300Framework::resetSpeller() {

	//speller->nextPrediction();
	//delete speller;
	listener->setStartedInterface(false);
}

int P300Framework::getNumberOfCalibrations() {

	return controller->getNumberOfCalibration();
}

/*
void P300Framework::createStatisticsXMLfile() {

	std::string const XML_FILE_PATH = "XMLstatistics/"+username+".xml";

    // Generate new XML document within memory
    pugi::xml_document doc;
    // Alternatively store as shared pointer if tree shall be used for longer
    // time or multiple client calls:
    // std::shared_ptr<pugi::xml_document> spDoc = std::make_shared<pugi::xml_document>();
    // Generate XML declaration
	pugi::xml_node declarationNode = doc.append_child(pugi::node_declaration);
    declarationNode.append_attribute("version")    = "1.0";
    declarationNode.append_attribute("encoding")   = "ISO-8859-1";
    declarationNode.append_attribute("standalone") = "yes";
    // A valid XML doc must contain a single root node of any name

	time_t now;
	char* timeNow = new char[12];
	now = time(NULL);
	strftime(timeNow, 80, "%Y_%m_%d_%H_%M_%S", gmtime(&now));

	pugi::xml_node root = doc.append_child("MyRoot");
	root.append_child(pugi::node_pcdata).set_value(timeNow);

    // Save XML tree to file.
    // Remark: second optional param is indent string to be used;
    // default indentation is tab character.
   bool saveSucceeded = doc.save_file(XML_FILE_PATH.c_str(), PUGIXML_TEXT("  "));
}
*/

//void P300Framework::spelling(int n_flash) {
void P300Framework::spelling() {

	int errorCode;
	string errorMessage;

	//createStatisticsXMLfile();

	try {
		//in attesa dell'evento che indica l'avvio dell'interfaccia (cod 100)
		WaitForSingleObject(listener->getStartedCalibrationEvent().m_hObject, INFINITE); 
		
		// SLB Redundant?
		pugi::xml_document doc;
		//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
		pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
		if (result) {
			//n_flash = atoi(doc.child("num_flash").first_child().value());
			number_of_flash			= atoi(doc.child("num_flash").first_child().value()); // SLB
			number_of_row_interface	= atoi(doc.child("row_num_interface").first_child().value());
		}
		//
		//cout << number_of_flash << endl;
		//cout << number_of_row_interface << endl;

		//speller = new SpellerController(flashingMode,number_of_row_interface,n_flash,number_of_channels,window_len,sampleRate,username);
		speller = new SpellerController(flashingMode,number_of_row_interface,number_of_flash,number_of_channels-number_of_channels_plus,window_len,sampleRate,username); // SLB

		module->StartAcquisition();
		speller->openBinFile();

		//se arriva l'evento di avvio interfaccia, può partire il thread di acquisizione dei dati
		_dataAcquisitionThread = AfxBeginThread(module->DoAcquisition, NULL, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED);
		_dataAcquisitionThread->ResumeThread();

		cout << "Started!\n";
		while (! WaitForSingleObject(speller->getFinishedWriteBufferEvent().m_hObject, 0) == WAIT_OBJECT_0) {
			//quando arriva evento di inizio stimolo, si resetta il buffer
			if (WaitForSingleObject(listener->getCallNextStimulus().m_hObject, 0) == WAIT_OBJECT_0) {
				module->resetBuffer();
				listener->setStartedInterface(true);
			}
			//scrittura su receivedData dei dati acquisiti dalla bci
			if (!module->ReadData(receivedData, &errorCode, &errorMessage)) {
				if (errorCode == 2) {
					continue;
				}
				else {
					cout << "ERROR" << endl;
					throw errorMessage;
				}
			}

			//aggiunzione del canale tempo a receivedData
			receivedData[number_of_channels] = 0;

			//se arriva evento che indica l'arrivo di un nuovo tag dall'interfaccia, lo si aggiunge al canale tag di receivedData
			if (WaitForSingleObject(listener->getSymbolTagEvent().m_hObject, 0) == WAIT_OBJECT_0) {
				receivedData[number_of_channels + 1] = listener->getSymbolTag();
			}
			else {
				listener->getSymbolTagEvent().ResetEvent();
				receivedData[number_of_channels + 1] = 0;  //qua andrà il tag ottenuto dall'interfaccia
			}

			receivedData[number_of_channels + 2] = 0;

			thresholdAndScale(); // SLB

			//in contemporanea alla ricezione dei dati si avvia la scrittura dei buffer
			if (listener->isStartedInterface()) {
				
				//applicazione del filtraggio spaziale e scrittura sui buffer
				speller->fillBuffer(receivedData);
				speller->writeBinFile(receivedData);

				guiObject->setReceivedData(receivedData);
				
				//nel momento in cui arriva evento di fine calibrazione dall'interfaccia, si stoppa thread di ricezione tag
				if (WaitForSingleObject(listener->getFinishCalibrationEvent().m_hObject, 0) == WAIT_OBJECT_0) {
					//listener->stopAcquisition();
					cout << "Ho chiuso il thread di InterfaceListener" << endl;
				}
			}
		}

		//stop del thread di acquisizione dei dati dalla bci
		//module->StopAcquisition(); 
		//speller->printFile();
		cout << "Ho chiuso il thread di P300SignalAcquisition" << endl;
		speller->closeBinFile();
		_dataAcquisitionThread->SuspendThread();
		cout << "Tag predicted: ";
		int* tagpredicted = speller->prediction();
		sendTagtoInterface(tagpredicted);
		writeTagPredicted(tagpredicted);

		return;
	}
	catch (string& exception) {
		//an exception occured
		cout << "\t" << exception << "\n";
	}
}

void P300Framework::sendCommandToInterface(int com) {

	clientTCP *conn = new clientTCP;
	conn->sendCommand(std::to_string(com).c_str()); // SLB
	delete conn;
}

void P300Framework::sendTagtoInterface(int* predictedTag) {

	int com = predictedTag[0];
	string s = std::to_string(com);
	
	if (predictedTag[1] != -1) {
		int com2 = predictedTag[1];
		string s2 = std::to_string(com2);
		s += ("-"+s2);
	}

	clientTCP *conn = new clientTCP;
	conn->sendCommand(s.c_str()); // SLB
	delete conn;
}

void P300Framework::writeTagPredicted(int* tag) {

	pugi::xml_document doc;
	//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	
	int index = 0;

	if (tag[1] == -1) 
		index = tag[0];
	else {
		int i = tag[0] - 1;
		int j = tag[1] - 1;
		index = ((i*number_of_row_interface) + (j-number_of_row_interface))+1;
	}

	/* SLB
	string s = to_string(index);
	const char *p = s.c_str();
	*/

	if (result) {
		pugi::xml_node node = doc.child("flash_analysis");
		if (node.first_child().empty()) {
			//node.append_child(pugi::node_pcdata).set_value(p);
			node.append_child(pugi::node_pcdata).set_value(std::to_string(index).c_str()); // SLB
		}
		else {
			//node.first_child().set_value(p);
			node.first_child().set_value(std::to_string(index).c_str()); // SLB
		}

		//doc.save_file("./configuration.xml");
		doc.save_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	}

}

int P300Framework::offlineSpellerOnStart(int tag) {

	offspeller = new OfflineSpeller(number_of_flash,number_of_tags,1,number_of_channels-number_of_channels_plus,(window_len/sampleRate),1,sampleRate,flashingMode,number_of_row_interface,username);
	int n_flash = offspeller->calculateNumberOfFlashOnline(tag);
	return n_flash;
}

int P300Framework::CalibAnalysis() {

	pugi::xml_document doc;
	//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB

	pugi::xml_node tmp = doc.child("calibration_analysis");
	
	int n_flash = 0;
	if (tmp.first_child().empty()) {							//se clicco su analisi calibrazioni con campo testo vuoto
		int n_session = controller->getNumberOfSession();
		ldaClassification();
		offspeller = new OfflineSpeller(number_of_flash,number_of_tags,number_of_calibration_tag,number_of_channels-number_of_channels_plus,(window_len/sampleRate),n_session,sampleRate,flashingMode,number_of_row_interface,username);
		n_flash = offspeller->calculateNumberOfFlash();
	}
	else { 
		int nc = controller->concCalibrations();
		controller->LDAcalibAnalysis(nc);
		offspeller = new OfflineSpeller(number_of_flash,number_of_tags,number_of_calibration_tag,number_of_channels-number_of_channels_plus,(window_len/sampleRate),nc,sampleRate,flashingMode,number_of_row_interface,username);
		n_flash = offspeller->calculateNumberOfFlashCalibAnalisys();
	}
	return n_flash;
}

int P300Framework::getInputNewFlash() {

	pugi::xml_document doc;
	//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB

	int tagPredicted = 0;
	if (result) {
		tagPredicted = atoi(doc.child("flash_analysis").first_child().value());
	}

	return tagPredicted;
}

// SLB
void P300Framework::signalCheck() {

	int errorCode;
	string errorMessage;

	module->StartAcquisition();

	try {
		_dataAcquisitionThread = AfxBeginThread(module->DoAcquisition, NULL, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED);
		_dataAcquisitionThread->ResumeThread();
		
		cout << "\nAcquiring channel signals...\n";
		
		clock_t startTime	= clock();
		clock_t currentTime	= startTime;
		int aSec			= 0;
		int bSec			= 0;
		int duration		= 10 * CLOCKS_PER_SEC;

		//while ((currentTime - startTime) < duration) {
		while (!guiObject->isCurrentRunDone()) {

			currentTime = clock();
			bSec = (currentTime - startTime) / CLOCKS_PER_SEC;

			if (aSec != bSec) {
				aSec = bSec;
				cout << "Elapsed time: ";
				if (aSec < 10)
					cout << " ";
				cout << aSec << " sec" << endl;
			}
			
			if (!module->ReadData(receivedData, &errorCode, &errorMessage)) {
				if (errorCode == 2)
					continue;
				else {
					cout << "ERROR" << endl;
					throw errorMessage;
				}
			}
			
			receivedData[number_of_channels]	 = 0;
			receivedData[number_of_channels + 1] = 0;
			receivedData[number_of_channels + 2] = 0;
			
			thresholdAndScale(); // SLB
			
			guiObject->setReceivedData(receivedData);

		}

		cout << "Channel signals acquisition stopped.\n\n";

		_dataAcquisitionThread->SuspendThread();
		cout << "P300SignalAcquisition thread closed.\n" << endl;
	
	}
	catch (string& exception) {
		cout << "\t" << exception << "\n";
	}
}

// SLB
void P300Framework::gameSession() {

	bool trialEndReached = false;
	bool trialClosedFile = false;

	// SLB TEST tobii instance


	while (guiObject->isGameRunDone()) {}

	int errorCode;
	string errorMessage;

	module->StartAcquisition();
	controller->openGameBinFile();
	controller->openGameTrialBinFile();

	try {
		_dataAcquisitionThread = AfxBeginThread(module->DoAcquisition, NULL, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED);
		_dataAcquisitionThread->ResumeThread();
		
		cout << "\nAcquiring signals for the game session...\n";

		clock_t startTime	= clock();
		clock_t currentTime	= startTime;
		int aSec			= 0;
		int bSec			= 0;
		int duration		= 10 * CLOCKS_PER_SEC;

		while (!guiObject->isGameRunDone()) {

			currentTime = clock();
			bSec = (currentTime - startTime) / CLOCKS_PER_SEC;

			if (aSec != bSec) {
				aSec = bSec;
				cout << "Elapsed time: ";
				if (aSec < 10)
					cout << " ";
				cout << aSec << " sec" << endl;
			}
			
			if (!module->ReadData(receivedData, &errorCode, &errorMessage)) {
				if (errorCode == 2)
					continue;
				else {
					cout << "ERROR" << endl;
					throw errorMessage;
				}
			}
			
			thresholdAndScale(); // SLB
			
			int rDataN = (int) guiObject->getReceivedDataN();
			int rDataN1 = (int) guiObject->getReceivedDataN1();
			
			receivedData[number_of_channels]			= (!trialEndReached) ? rDataN : FILETYPE_GAME;
			receivedData[number_of_channels + 1]		= rDataN1;
			
			if (tobiienabled)
				receivedData[number_of_channels + 2]	= (guiObject->gameEyeT->getUserTracked() ? GAMEUSER_LOOKING : GAMEUSER_NOTLOOKING);
			else
				receivedData[number_of_channels + 2]	= GAMETOBII_DISABLED;
			
			//cout << receivedData[number_of_channels + 2] << endl; //////////////

			controller->writeRawBinFile(receivedData);
			if (!trialClosedFile && (rDataN1 || (!rDataN1 && rDataN != FILETYPE_GAME)))
				controller->writeRawTrialBinFile(receivedData);
			
			guiObject->setReceivedData(receivedData);

			//(rDataN1) ? trialEndReached = true : trialEndReached = false;
			//if (rDataN != FILETYPE_GAME) {
			if ((rDataN == GAMEMODE_NEXT) || (rDataN == GAMEMODE_SKIP)) {
				trialEndReached = true;
				if (!trialClosedFile) {
					controller->closeGameTrialBinFile();
					trialClosedFile = true;
					///runModulesGame(); // SLB TODO implement first the matlab function to handle game-type signals (id -3)
				}
			}
			else {
				trialEndReached = false;
				if (trialClosedFile) {
					controller->openGameTrialBinFile();
					trialClosedFile = false;
				}
			}

		}

		cout << "Channel signals acquisition stopped.\n\n";

		controller->closeBinFile();

		_dataAcquisitionThread->SuspendThread();
		cout << "P300SignalAcquisition thread closed.\n" << endl;
	
	}
	catch (string& exception) {
		cout << "\t" << exception << "\n";
	}
}

// SLB
void P300Framework::baselineAcquisitionInit() {

	controller->closeBaselineBinFile();
	controller->openBaselineBinFile();
	
}

// SLB
void P300Framework::baselineAcquisition(int eyesOpen, int seconds) {
	
	int errorCode;
	string errorMessage;

	module->StartAcquisition();

	try {
		//while (!baselineAcquisitionStarted) {}

		_dataAcquisitionThread = AfxBeginThread(module->DoAcquisition, NULL, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED);

		_dataAcquisitionThread->ResumeThread();
		//Sleep(1000);

		cout << "\nBaseline acquisition started!\n";
		
		clock_t startTime	= clock();
		clock_t currentTime	= startTime;
		int aSec			= 0;
		int bSec			= 0;
		int duration		= seconds * CLOCKS_PER_SEC;

		while ((currentTime - startTime) < duration) {

			currentTime = clock();
			bSec = (currentTime - startTime) / CLOCKS_PER_SEC;
			/*
			if (!(currentTime % CLOCKS_PER_SEC))
				cout << "Elapsed time: " << currentTime / CLOCKS_PER_SEC << endl;
			*/
			if (aSec != bSec) {
				aSec = bSec;
				cout << "Elapsed time: ";
				if (aSec < 10)
					cout << " ";
				cout << aSec << " sec" << endl;
			}

			if (!module->ReadData(receivedData, &errorCode, &errorMessage)) {
				if (errorCode == 2)
					continue;
				else {
					cout << "ERROR" << endl;
					throw errorMessage;
				}
			}
			
			receivedData[number_of_channels]	 = FILETYPE_BASELINE; // id for baseline signal
			receivedData[number_of_channels + 1] = eyesOpen;
			receivedData[number_of_channels + 2] = 0;
			
			thresholdAndScale(); // SLB
			
			controller->writeRawBinFile(receivedData);

			guiObject->setReceivedData(receivedData);

		}

		guiObject->playSound("notification");
		cout << "Baseline acquisition stopped.\n\n";

		//Sleep(1000);
		_dataAcquisitionThread->SuspendThread();
		cout << "P300SignalAcquisition thread closed.\n" << endl;
	
	}
	catch (string& exception) {
		cout << "\t" << exception << "\n";
	}
}

// SLB
void P300Framework::baselineAcquisitionTerm() {
	
	controller->closeBaselineBinFile();
	//Sleep(2000);
	//_dataAcquisitionThread->SuspendThread();

}

// SLB
void P300Framework::BaselineAnalysis() {
	
	if (bioHybridMode && engineenabled)
		featuresExtractor->BaselineAnalysis();
	
	//else if (bioHybridMode && !engineenabled)
	//	cout << "Matlab engine could not be started. (FeatureExtraction/P300Framework)" << endl;

}

// SLB
void P300Framework::ExtractFeatures() {
	
	if (bioHybridMode && engineenabled)
		featuresExtractor->Process();

	//else if (bioHybridMode && !engineenabled)
	//	cout << "Matlab engine could not be started. (FeatureExtraction/P300Framework)" << endl;

}

// SLB
void P300Framework::ActionSelect() {
	
	//if (selectorenabled)
		actionSelector->Process();

}

// SLB
void P300Framework::ModulateResponse() {
	
	responseModulator->Process();

}

// SLB
string P300Framework::getResponse() {
	
	return responseModulator->getResult();

}

// SLB
void P300Framework::setbaselineAcquisitionStarted(bool value) {

	baselineAcquisitionStarted = value;

}

// SLB
void P300Framework::runModules() {

	ExtractFeatures();
	ActionSelect();
	ModulateResponse();

	guiObject->setIndexSliders();
	guiObject->setPlotBars();
	guiObject->updateStamp(getResponse());

}

// SLB
void P300Framework::runModulesGame() {

	ExtractFeatures();

	guiObject->setIndexSliders();
	guiObject->setPlotBars();

}