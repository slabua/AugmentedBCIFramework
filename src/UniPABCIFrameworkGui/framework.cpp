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

#include "Gui.h"
#include "P300Framework.h"
#include "pugixml.hpp"

// SLB QT4
#include <QtGui/QApplication>
//#include <QtCore/QThread>
//
/* SLB QT4
#include <Qt/qapplication.h>
#include <Qt/qthread.h>
*/
/* SLB QT5
#include <QtWidgets\qapplication.h>
#include <QtCore\qthread.h>
*/
// SLB
#include "rootpaths.h"
//
//#include "engine.h"

#define SLBDEBUG			FALSE // SLB

#pragma comment(lib, "gUSBamp.lib")
#pragma comment(lib, "Ws2_32.lib")

using std::cout;
using std::endl;
using std::string;
using std::ifstream;

int args = 0;
char **argv2;

Gui *gui;

string padding = ""; // "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"; // SLB

bool classificationComplete		= false;
bool classificatorReady			= false;
///bool flag						= false;	// SLB not used in originalcd
bool optFlash					= false;
int n_flash_online				= 0;		// SLB moved here as global
//float flash_precision			= 0;		// SLB
bool deviceOpened				= false;	// SLB
bool spellerInterfaceStarted	= false;	// SLB


// SLB
void readConfiguration() {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str());

	if (result)
		optFlash = (bool) atoi(doc.child("optimization").first_child().value());

	else
		cout << "Could not find configuration file." << endl;

}

// SLB
void updateConfiguration(string flash_precision) {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str());

	if (result) {

		pugi::xml_node node = doc.child("flash_precision");
		//node.first_child().set_value(std::to_string(flash_precision).c_str());
		node.first_child().set_value(flash_precision.c_str());

		doc.save_file((configFilesRoot + "/configuration.xml").c_str());
	}
	else
		cout << "Could not find configuration file." << endl;

}

string getFlash() {

	//string line; // SLB moved below
	//string is;
	//int i=0; // SLB moved below
	string n_flash;
	ifstream flash;
	flash.open((outputFilesRoot + "/flash.txt").c_str()); // SLB
	//flash.open(outputFilesRoot + "/flash.txt");
	if (flash.is_open()) {
		string line;	// SLB
		int i=0;		// SLB
		while (getline(flash, line)) {
			i++;
			///std::string::size_type sz;     // alias of size_t
			n_flash += std::to_string(i) + "\t" + line + "%\n"; // SLB was "	"

			// SLB
			if (i == n_flash_online) {
				//float flash_precision = atof(line.c_str());
				//updateConfiguration(flash_precision);
				updateConfiguration(line);
			}
		}
	}
	flash.close();

	return n_flash;
}

void setNewFlashConfig(int flash) {

	pugi::xml_document doc;
	//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	
	if (result) {
		pugi::xml_node node = doc.child("num_flash");
		node.first_child().set_value(std::to_string(flash).c_str()); // SLB

		//doc.save_file("./configuration.xml");
		doc.save_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	}
}

unsigned int initGui(LPVOID params) {

	//QApplication::setGraphicsSystem("opengl"); // SLB opengl wide
	//QApplication::setDesktopSettingsAware(false);
	//QApplication::setStyle("cleanlooks"); // plastique windows motif cde cleanlooks windowsxp windowsvista macintosh // ./myapplication -style motif

	// SLB use worker instead
	qRegisterMetaType< string >("string");
	qRegisterMetaType< QTextCursor >("QTextCursor");
	//

	QApplication app(args, argv2);
	
	gui = new Gui();
	gui->show();
	app.exec();
	
	return 0xdead;
	//return 0; // SLB
}

// SLB
void initDevice(P300Framework* fw) {
	
	if (!deviceOpened) {
		deviceOpened = true;
		if (!SLBDEBUG)
			fw->thread_initialization(); // SLB Locks if no amplifier is connected
	}
}

// SLB
void startInterface(P300Framework* fw) {
	if (!spellerInterfaceStarted) {
		spellerInterfaceStarted = true;
		fw->sendCommandToInterface(650);
		Sleep(1000);
	}
}

// SLB TODO remove, redundant
/*
int getNumberFlashFromConfig() {

	pugi::xml_document doc;
	//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	if (result) 
		 return atoi(doc.child("num_flash").first_child().value());
}
*/

int main(int argc, char *argv[]) {
	
	// SLB
	//args	= argc;
	//argv2	= argv;
	
	CWinThread *guiThread = AfxBeginThread(initGui, NULL, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED);
	gui->check_folder(configFilesRoot);			// SLB
	guiThread->ResumeThread();
	//CWinThread *guiThread = AfxBeginThread(initGui, NULL, THREAD_PRIORITY_ABOVE_NORMAL, 0, 0); // SLB
	Sleep(1000); // 2000

	gui->check_folder(binFilesRoot);			// SLB
	gui->check_folder(outputFilesRoot);			// SLB
	gui->check_folder(outputFilesRoot + "LDA");	// SLB
	gui->check_folder(logFilesRoot);			// SLB

	// SLB
	//gui->updateStamp(padding);

	readConfiguration(); // SLB
	Sleep(1000);

	// SLB TEST
	
	//---------

	while (gui->isRunning()) {}

	//P300Framework* framework = new P300Framework();
	P300Framework* framework = new P300Framework(gui); // SLB
	//framework->sendCommandToInterface(650); ////////////////////////////// SLB moved below for each calib/spell event

	// SLB Solves an assert error.
	char *initialStamp = "System set-up completed!\nStarting interface...\n";
	gui->updateStamp(initialStamp);
	
	//int n_flash_online = 0; // SLB moved to global
	
	//bool flag = false; // SLB
	//n_flash_online = 0; // SLB redundant
	char x;
	while (true) {
		WaitForSingleObject(gui->getFinishInitializationEvent().m_hObject, INFINITE);
		
		initDevice(framework); // SLB
		
		if (WaitForSingleObject(gui->getSignalCheckStartEvent().m_hObject, 0)			== WAIT_OBJECT_0) { // SLB
			x = 't';
		}
		if (WaitForSingleObject(gui->getGameSessionStartEvent().m_hObject, 0)			== WAIT_OBJECT_0) { // SLB
			x = 'g';
		}
		if (WaitForSingleObject(gui->getBaselineAcquisitionStartEvent().m_hObject, 0)	== WAIT_OBJECT_0) { // SLB
			x = 'b';
		}
		if (WaitForSingleObject(gui->getCalibrationStartEvent().m_hObject, 0)			== WAIT_OBJECT_0) {
			x = 'c';
		}
		if (WaitForSingleObject(gui->getSpellerStartEvent().m_hObject, 0)				== WAIT_OBJECT_0) {
			x = 's';
		}
		if (WaitForSingleObject(gui->getSpellerFileStartEvent().m_hObject, 0)			== WAIT_OBJECT_0) {
			x = 'f';
		}
		if (WaitForSingleObject(gui->getFlashAnalysisStartEvent().m_hObject, 0)			== WAIT_OBJECT_0) {
			x = 'a';
		}
		if (WaitForSingleObject(gui->getOnlineFlashAnalysisStartEvent().m_hObject, 0)	== WAIT_OBJECT_0) {
			x = 'o';
		}
		if (WaitForSingleObject(gui->getCalibrationAnalysisStartEvent().m_hObject, 0)	== WAIT_OBJECT_0) {
			x = 'r';
		}
		if (WaitForSingleObject(gui->getKillEvent().m_hObject, 0)						== WAIT_OBJECT_0) { // SLB
			x = 'x';
		}
		switch (x) {
			// SLB
			case 't':
				gui->updateStamp("Acquiring channel signals...\n");
				framework->signalCheck();
				gui->updateStamp("Channel signals acquisition stopped.\n");
				break;
			// SLB
			case 'g':
				//gui->updateStamp("Starting Game Session...\n");
				while (!gui->isGameRunStarted()) {}
				if (gui->isGameUseBCI()) // CHECK
					framework->gameSession();
				//gui->updateStamp("Channel signals acquisition stopped.\n");
				break;
			// SLB
			case 'b':
				gui->setCurrentRunDone(false);
				gui->updateStamp("Start baseline acquisition\n");
				
				while (!gui->isAcqReady() && !gui->isAcqDone()) {}
				if (!gui->isAcqDone()) {
					framework->baselineAcquisitionInit();
					framework->baselineAcquisition(EYESCLOSED, gui->getBaselineAcqDuration());
					///Sleep(gui->getBaselineAcqDuration() * 1000);
					gui->setEyesAcquisitionStatus(CLOSEDEYES_STOP);
				}

				while (!gui->isAcqReady() && !gui->isAcqDone()) {}
				if (!gui->isAcqDone()) {
					framework->baselineAcquisition(EYESOPEN, gui->getBaselineAcqDuration());
					///Sleep(gui->getBaselineAcqDuration() * 1000);
				}
				
				framework->baselineAcquisitionTerm();
				
				if (!gui->isAcqDone()) {
					gui->updateStamp("End baseline acquisition\n");
					//gui->updateStamp("Starting baseline analysis...");
					///framework->BaselineAnalysis(); // SLB To enable once the matlab function will be written.
					//gui->updateStamp("Baseline analysis complete");
				}
				else
					gui->updateStamp("Baseline acquisition interrupted\n");

				gui->updateStamp("Ready for P300 calibration\n");
				gui->setCurrentRunDone(true);
				///Sleep(500);
				break;
			//
			case 'c':						//calibrazione
				startInterface(framework); // SLB
				gui->updateStamp("Start calibration\n");
				gui->updateStamp("Press s into P300 interface to start\n");
				framework->calibration();
				gui->updateStamp("End calibration");
				framework->runModules(); // SLB
				framework->resetCalibration();
				gui->initReceivedData(); // SLB
				gui->updateStamp("Ready for next calibration or recognition");
				break;
			case 's':
				///if (!classificationComplete && !flag) {
				if (!classificationComplete) { // SLB from originalcd
					gui->updateStamp("Start Bayesian Linear Discriminant Analysis\n");
					framework->ldaClassification();
					gui->updateStamp("Bayesian Linear Discriminant Analysis complete!\n");
					classificationComplete = true;
					framework->sendCommandToInterface(700);
				}
				gui->updateStamp("Online recognition start");
				gui->updateStamp("Press s into P300 interface to start\n");
				//framework->spelling(getNumberFlashFromConfig());
				framework->spelling(); // SLB
				//////////////////////
				gui->updateStamp("Start analysis of number of flash\n");
				n_flash_online = framework->offlineSpellerOnStart(framework->getInputNewFlash());
				gui->updateStamp("Flash number | Precision");
				gui->updateStamp(getFlash());
				gui->updateStamp("Optimal number of flash: " + std::to_string(n_flash_online));
				//////////////////////////////////
				framework->runModules(); // SLB
				framework->resetSpeller();
				gui->initReceivedData(); // SLB
				break;
			case 'f':						//spelling da file senza calibrazione
				startInterface(framework); // SLB
				//if (!classificatorReady && !flag) {
				if (!classificatorReady) { // SLB from originalcd
					gui->updateStamp("Loading of classificator complete!\n");
					framework->sendCommandToInterface(700);
					classificatorReady = true;
				}
				gui->updateStamp("Online recognition start");
				gui->updateStamp("Press s into P300 interface to start\n");
				//framework->spelling(getNumberFlashFromConfig());
				framework->spelling(); // SLB
				//////////////////////
				gui->updateStamp("Start analysis of number of flash\n");
				n_flash_online = framework->offlineSpellerOnStart(framework->getInputNewFlash());
				gui->updateStamp("Flash number | Precision");
				gui->updateStamp(getFlash());
				if (optFlash) // SLB
					setNewFlashConfig(n_flash_online);
				gui->updateStamp("Optimal number of flash: " + std::to_string(n_flash_online));
				//////////////////////////////////
				framework->runModules(); // SLB
				framework->resetSpeller();
				gui->initReceivedData(); // SLB
				break;
			case 'o':
				gui->updateStamp("Start analysis of number of flash\n");
				n_flash_online = framework->offlineSpellerOnStart(framework->getInputNewFlash());
				gui->updateStamp("Flash number | Precision");
				gui->updateStamp(getFlash());
				if (optFlash) // SLB
					setNewFlashConfig(n_flash_online);
				gui->updateStamp("Optimal number of flash: " + std::to_string(n_flash_online));
				break;
			case 'r':
				gui->updateStamp("Start analysis of calibrations\n");
				n_flash_online = framework->CalibAnalysis();
				gui->updateStamp("Flash number | Precision");
				gui->updateStamp(getFlash());
				///flag = true; // SLB not used in originalcd
				break;
			case 'a':
				gui->updateStamp("Start Bayesian Linear Discriminant Analysis\n");
				framework->ldaClassification();
				gui->updateStamp("Bayesian Linear Discriminant Analysis complete!\n");
				gui->updateStamp("Start analysis of number of flash\n");
				//int n_flash_star = framework->offlineSpellerStart(); // SLB CHECK
				n_flash_online = framework->offlineSpellerStart(); // SLB CHECK
				gui->updateStamp("Flash number | Precision");
				gui->updateStamp(getFlash());
				if (optFlash) // SLB
					setNewFlashConfig(n_flash_online);
					//setNewFlashConfig(n_flash_star);
				//gui->updateStamp("Optimal number of flash: "+to_string(n_flash_star)); // SLB CHECK
				gui->updateStamp("Optimal number of flash: " + std::to_string(framework->offlineSpellerStart()));
				break;
			case 'x': // SLB
				gui->updateStamp("Stopping framework...");
				framework->sendCommandToInterface(000);
				//gui->close();
				//Sleep(1000);
				//exit(0);
				//flag = true;
				//break;
				return 0;
		}
		/*
		if (flag) // SLB
			break;
		*/
	}

	//system("PAUSE");
	return 0;
}
