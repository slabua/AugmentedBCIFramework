/*****************************************************************************
* 
* UniPA BCI Framework: an Augmented BCI Framework
* 
* Copyright (C) 2016-2017 Salvatore La Bua (slabua@gmail.com)
* RoboticsLab, Department of Industrial and Digital Innovation (DIID),
* Universita'� degli Studi di Palermo, V. delle Scienze, Palermo, Italy.
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

#include "GUI.h"
#include <ws2tcpip.h>
#include "document.h"
#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "DecisionTree.h"
#include "RobotController.h"
#include "rootpaths.h" //SLB
#include "pugixml.hpp"
#include "TobiiTracker.h"

using std::cout;
using std::endl;
using std::pair;
using std::ifstream;
using std::random_shuffle;

int window; // SLB moved to global to get the handle for ultimate window destruction
TobiiTracker* EyeT;

// -----------------------------------------------------------------------------
// ------------------- Funzioni relative ai quadrati ---------------------------
// -----------------------------------------------------------------------------
void initCounterSquare() {
	counterSquares = new int[squaresNumber];
	for (int i=0; i<squaresNumber; i++)
		counterSquares[i] = 0;
}

void initCounterRow() {
	counterRows = new int[squaresNumber];
	for (int i=0; i<rowsNumber; i++)
		counterRows[i] = 0;
}

void initCounterColumn() {
	counterColumns = new int[squaresNumber];
	for (int i=0; i<rowsNumber; i++)
		counterColumns[i] = 0;
}

// SLB
void initTobii() {

	if (tobiienabled) {
		cout << "Enabling eye tracker..." << endl;
		
		EyeT = new TobiiTracker(g_Width, g_Height, text_height);
		
		if (EyeT->Initialize()) {
			cout << "Eye tracker enabled." << endl;
		}
		else {
			cout << "Eye tracker not available." << endl;
			EyeT->setRunning(false);
			tobiienabled = false;
		}
		
	}

}

void initParameterVectors() {

	// SLB new rendering pipeline, generating texture tiles
	int width		= g_Width;
	int height		= g_Height-(text_height*2);
	int shorterSide	= min(width, height);
	int longerSide	= max(width, height);

	float square_size_full	= shorterSide / rowsNumber;
	size					= (square_size_full / 100) * square2pad_ratio;
	int pad_size			= square_size_full - size;
	int halfpad				= pad_size / 2;

	int larger_offset = (longerSide-shorterSide) / 2;
	int x_offset;
	int y_offset;

	if (height <= width) {
		x_offset = larger_offset+halfpad;
		y_offset = halfpad+text_height;
	}
	else {
		x_offset = halfpad;
		y_offset = larger_offset+halfpad+text_height;
	}

	for (int r=0, index=0; r<rowsNumber; r++) {
		for (int c=0; c<rowsNumber; c++, index++) {
			u_coor[index] = x_offset;
			x_offset += (pad_size+size);
		}
		(height <= width) ? x_offset = larger_offset+halfpad : x_offset = halfpad;
	}
	for (int r=0, index=(rowsNumber*rowsNumber-1); r<rowsNumber; r++) {
		for (int c=0; c<rowsNumber; c++, index--) {
			v_coor[index] = y_offset;
		}
		y_offset += (pad_size+size);
	}
	//

}

void setColorMap() {
  colorMap.insert(pair<string, string>(WATER,	water));
  colorMap.insert(pair<string, string>(GREEN,	green));
  colorMap.insert(pair<string, string>(RED,		red));
  colorMap.insert(pair<string, string>(CYAN,	cyan));
  colorMap.insert(pair<string, string>(BLUE,	blue));
  colorMap.insert(pair<string, string>(YELLOW,	yellow));
  colorMap.insert(pair<string, string>(WHITE,	white));
  colorMap.insert(pair<string, string>(BLACK,	black));
  colorMap.insert(pair<string, string>(PURPLE,	purple));
  colorMap.insert(pair<string, string>(ORANGE,	orange));
}

void initPaths() {
	for (int i=0; i<squaresNumber; i++) {
		tpnVector[i] = interface->getTexturePathName() + mapTexture[i] + ".bmp"; // SLB
		tpnfVector[i] = interface->getTexturePathName() + textureFlash + ".bmp";
	}
}

void initSquare() {
	for (int i=0; i<squaresNumber; i++) {
		showSquare[i] = false;
		squaresVector[i] = new P300Square(i, u_coor[i], v_coor[i], size, tpnVector[i], tpnfVector[i]);
	}
}

void loadSquare() {
	for (int i=0; i<squaresNumber; i++)
		squaresVector[i]->setTexture(showSquare[i]);
}

void initRows() {
	for (int i=0; i<rowsNumber; i++) {
		showRow[i] = false;
		showColumn[i] = false;
		rowsVector[i] = new P300Row(rowsNumber);
		for (int j = 0; j < rowsNumber; j++)
			rowsVector[i]->addSquare(squaresVector[(i*squaresNumber) + j]);
	}
}

void initInterface() {
	for (int i=0; i<rowsNumber; i++)
		interface->addRow(rowsVector[i]);
}

// SLB
void getMemoryInformation() {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/memory.xml").c_str()); // SLB

	if (result)	{
		memoryInformation.insert(pair <int, bool> (FINESTRA,	(bool) atoi(doc.child("window").first_child().value())));
		memoryInformation.insert(pair <int, bool> (PORTA,		(bool) atoi(doc.child("door").first_child().value())));
		memoryInformation.insert(pair <int, bool> (TV,			(bool) atoi(doc.child("tv").first_child().value())));
		memoryInformation.insert(pair <int, bool> (RADIO,		(bool) atoi(doc.child("radio").first_child().value())));
		memoryInformation.insert(pair <int, bool> (LUCE,		(bool) atoi(doc.child("light").first_child().value())));
	}
	else {
		std::cout << "XML  parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
		std::cout << "Error description: " << result.description() << "\n";
		std::cout << "Error offset: " << result.offset << " (error at [..." << (result.offset) << "]\n\n";
	}

}

void initMapTexture() {
	
	if (!symbolicTexture && !customPalette) {
		for (int i=0; i<squaresNumber; i++) {
			string path = "speller/W" + std::to_string(i + 1);
			string name;
			mapTexture.insert(pair <int, string> (i, path));
			char tmp = ' ';
			if (i <= 25) {
				tmp = 'A' + i;
			} else {
				tmp = '0' + i - 26;
			}
			// SLB TODO check
			std::stringstream ss;
			ss << tmp;
			ss >> name;
			mapTextureName.insert(pair <int, string> (i, name));
		}
		//IMPOSTO LO 0
		mapTexture[30]="speller/ss";
		mapTextureName[30]="0";
	}
	else if (customPalette) {
		
		map<string,double*> p;
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB

		pugi::xml_node tmp=doc.child("texture");
		
		int i=0;
		for (pugi::xml_node tool = tmp.child("p"); tool; tool = tool.next_sibling("p")) {
			
			if (i == rowsNumber*rowsNumber)
				break;
			
			string name = tool.first_child().value();
			
			mapTexture.insert(pair<int, string>(i, name));      
			mapTextureName.insert(pair<int, string>(i, name));

			i++;
		}	
      
	}
	else {
		idSymbolicTexture = getOnlineParameters(); // SLB Was disabled, moved here

		getMemoryInformation(); // SLB
		
		mapTexture.insert(pair<int, string>(CIBO, "eat"));
		mapTextureName.insert(pair<int, string>(CIBO, "EAT"));
		mapTexture.insert(pair<int, string>(BERE, "drink"));
		mapTextureName.insert(pair<int, string>(BERE, "DRINK"));
		mapTexture.insert(pair<int, string>(ASSISTENZA, "assistance"));
		mapTextureName.insert(pair<int, string>(ASSISTENZA, "HELP"));
		mapTexture.insert(pair<int, string>(LETTO, "bed"));
		mapTextureName.insert(pair<int, string>(LETTO, "BED POSITION"));
		mapTexture.insert(pair<int, string>(TELEFONO, "telephone"));
		mapTextureName.insert(pair<int, string>(TELEFONO, "TELEPHONE"));
		mapTexture.insert(pair<int, string>(AMBULANZA, "ambulance"));
		mapTextureName.insert(pair<int, string>(AMBULANZA, "AMBULANCE"));
		mapTexture.insert(pair<int, string>(EMOZIONI, "emotion"));
		mapTextureName.insert(pair<int, string>(EMOZIONI, "EMOTION"));
		mapTexture.insert(pair<int, string>(SPELLER, "speller"));
		mapTextureName.insert(pair<int, string>(SPELLER, "SPELLER"));
		mapTexture.insert(pair<int, string>(CHIUDI, "exit"));
		mapTextureName.insert(pair<int, string>(CHIUDI, "BYE BYE"));
		mapTexture.insert(pair<int, string>(RADIO, "radio"));
		mapTextureName.insert(pair<int, string>(RADIO, "RADIO"));
		mapTexture.insert(pair<int, string>(TV, "tv"));
		mapTextureName.insert(pair<int, string>(TV, "TV"));
		mapTexture.insert(pair<int, string>(DORMIRE, "asleep"));
		mapTextureName.insert(pair<int, string>(DORMIRE, "ASLEEP"));

		// SLB
		setTexturesFromMemory();

		bool isHot = false; //false=freddo, true=caldo

		switch (idSymbolicTexture) {
			case 1:
				isHot = true;
				mapTexture.insert(pair<int, string>(TEMP, "hot"));
				mapTextureName.insert(pair<int, string>(TEMP, "HOT"));

				break;
			case 2:
				isHot = true;
				mapTexture.insert(pair<int, string>(TEMP, "hot"));
				mapTextureName.insert(pair<int, string>(TEMP, "HOT"));

				break;
			case 3:
				isHot = true;
				mapTexture.insert(pair<int, string>(TEMP, "hot"));
				mapTextureName.insert(pair<int, string>(TEMP, "HOT"));

				break;
			case 4:
				isHot = true;
				mapTexture.insert(pair<int, string>(TEMP, "hot"));
				mapTextureName.insert(pair<int, string>(TEMP, "HOT"));

				break;
			case 5:
				isHot = false;
				mapTexture.insert(pair<int, string>(TEMP, "cold"));
				mapTextureName.insert(pair<int, string>(TEMP, "COLD"));

				break;
			case 6:
				isHot = false;
				mapTexture.insert(pair<int, string>(TEMP, "cold"));
				mapTextureName.insert(pair<int, string>(TEMP, "COLD"));

				break;
		}

		mapFreddo.insert(pair<int, string>(0, "heater"));
		mapFreddoName.insert(pair<int, string>(0, "HEATER"));
		mapFreddo.insert(pair<int, string>(1, "23_cold"));
		mapFreddoName.insert(pair<int, string>(1, "23 DEGREES"));
		mapFreddo.insert(pair<int, string>(2, "25_cold"));
		mapFreddoName.insert(pair<int, string>(2, "25 DEGREES"));
		mapFreddo.insert(pair<int, string>(3, "28_cold"));
		mapFreddoName.insert(pair<int, string>(3, "28 DEGREES"));
		mapFreddo.insert(pair<int, string>(4, "30_cold"));
		mapFreddoName.insert(pair<int, string>(4, "30 DEGREES"));
		mapFreddo.insert(pair<int, string>(5, "blanket"));
		mapFreddoName.insert(pair<int, string>(5, "BLANKET"));
		mapFreddo.insert(pair<int, string>(6, "back"));
		mapFreddoName.insert(pair<int, string>(6, "BACK"));
		mapFreddo.insert(pair<int, string>(7, "tea"));
		mapFreddoName.insert(pair<int, string>(7, "HOT DRINK"));
		mapFreddo.insert(pair<int, string>(8, "exit"));
		mapFreddoName.insert(pair<int, string>(8, "CLOSE"));

		mapCaldo.insert(pair<int, string>(0, "fan"));
		mapCaldoName.insert(pair<int, string>(0, "FAN"));
		mapCaldo.insert(pair<int, string>(1, "16_hot"));
		mapCaldoName.insert(pair<int, string>(1, "16 DEGREES"));
		mapCaldo.insert(pair<int, string>(2, "19_hot"));
		mapCaldoName.insert(pair<int, string>(2, "19 DEGREES"));
		mapCaldo.insert(pair<int, string>(3, "21_hot"));
		mapCaldoName.insert(pair<int, string>(3, "21 DEGREES"));
		mapCaldo.insert(pair<int, string>(4, "23_hot"));
		mapCaldoName.insert(pair<int, string>(4, "23 DEGREES"));
		mapCaldo.insert(pair<int, string>(5, "25_hot"));
		mapCaldoName.insert(pair<int, string>(5, "25 DEGREES"));
		mapCaldo.insert(pair<int, string>(6, "back"));
		mapCaldoName.insert(pair<int, string>(6, "BACK"));
		mapCaldo.insert(pair<int, string>(7, "cold_drink"));
		mapCaldoName.insert(pair<int, string>(7, "COLD DRINK"));
		mapCaldo.insert(pair<int, string>(8, "exit"));
		mapCaldoName.insert(pair<int, string>(8, "CLOSE"));

		mapBere.insert(pair<int, string>(0, "water"));
		mapBereName.insert(pair<int, string>(0, "WATER"));
		mapBere.insert(pair<int, string>(1, "milk"));
		mapBereName.insert(pair<int, string>(1, "MILK"));
		mapBere.insert(pair<int, string>(2, "juice"));
		mapBereName.insert(pair<int, string>(2, "JUICE"));
		mapBere.insert(pair<int, string>(3, "beer"));
		mapBereName.insert(pair<int, string>(3, "BEER"));
		mapBere.insert(pair<int, string>(4, "drink"));
		mapBereName.insert(pair<int, string>(4, "DRINK"));
		mapBere.insert(pair<int, string>(5, "tea"));
		mapBereName.insert(pair<int, string>(5, "TEA"));
		mapBere.insert(pair<int, string>(6, "back"));
		mapBereName.insert(pair<int, string>(6, "BACK"));
		mapBere.insert(pair<int, string>(7, "coffee"));
		mapBereName.insert(pair<int, string>(7, "COFFEE"));
		mapBere.insert(pair<int, string>(8, "exit"));
		mapBereName.insert(pair<int, string>(8, "CLOSE"));

		mapCibo.insert(pair<int, string>(0, "breakfast"));
		mapCiboName.insert(pair<int, string>(0, "BREAKFAST"));
		mapCibo.insert(pair<int, string>(1, "pasta"));
		mapCiboName.insert(pair<int, string>(1, "PASTA"));
		mapCibo.insert(pair<int, string>(2, "pizza"));
		mapCiboName.insert(pair<int, string>(2, "PIZZA"));
		mapCibo.insert(pair<int, string>(3, "salad"));
		mapCiboName.insert(pair<int, string>(3, "SALAD"));
		mapCibo.insert(pair<int, string>(4, "sandwich"));
		mapCiboName.insert(pair<int, string>(4, "SANDWICH"));
		mapCibo.insert(pair<int, string>(5, "fish"));
		mapCiboName.insert(pair<int, string>(5, "FISH"));
		mapCibo.insert(pair<int, string>(6, "back"));
		mapCiboName.insert(pair<int, string>(6, "BACK"));
		mapCibo.insert(pair<int, string>(7, "steak"));
		mapCiboName.insert(pair<int, string>(7, "STEAK"));
		mapCibo.insert(pair<int, string>(8, "exit"));
		mapCiboName.insert(pair<int, string>(8, "CLOSE"));

		mapLetto.insert(pair<int, string>(0, "position0"));
		mapLettoName.insert(pair<int, string>(0, "POSITION  0"));
		mapLetto.insert(pair<int, string>(1, "POSITION10"));
		mapLettoName.insert(pair<int, string>(1, "POSITION 10"));
		mapLetto.insert(pair<int, string>(2, "position20"));
		mapLettoName.insert(pair<int, string>(2, "POSITION 20"));
		mapLetto.insert(pair<int, string>(3, "position30"));
		mapLettoName.insert(pair<int, string>(3, "POSITION 30"));
		mapLetto.insert(pair<int, string>(4, "position45"));
		mapLettoName.insert(pair<int, string>(4, "POSITION 45"));
		mapLetto.insert(pair<int, string>(5, "sit"));
		mapLettoName.insert(pair<int, string>(5, "POSITION 90 (SAT)")); // SLB was SAT
		mapLetto.insert(pair<int, string>(6, "back"));
		mapLettoName.insert(pair<int, string>(6, "BACK"));
		mapLetto.insert(pair<int, string>(7, "massage"));
		mapLettoName.insert(pair<int, string>(7, "MASSAGE"));
		mapLetto.insert(pair<int, string>(8, "exit"));
		mapLettoName.insert(pair<int, string>(8, "CLOSE"));
		
		mapEmozioni.insert(pair<int, string>(0, "yes"));
		mapEmozioniName.insert(pair<int, string>(0, "YES"));
		mapEmozioni.insert(pair<int, string>(1, "no"));
		mapEmozioniName.insert(pair<int, string>(1, "NO"));
		mapEmozioni.insert(pair<int, string>(2, "happy"));
		mapEmozioniName.insert(pair<int, string>(2, "HAPPY"));
		mapEmozioni.insert(pair<int, string>(3, "sad"));
		mapEmozioniName.insert(pair<int, string>(3, "SAD"));
		mapEmozioni.insert(pair<int, string>(4, "angry"));
		mapEmozioniName.insert(pair<int, string>(4, "ANGRY"));
		mapEmozioni.insert(pair<int, string>(5, "tired"));
		mapEmozioniName.insert(pair<int, string>(5, "TIRED"));
		mapEmozioni.insert(pair<int, string>(6, "back"));
		mapEmozioniName.insert(pair<int, string>(6, "BACK"));
		mapEmozioni.insert(pair<int, string>(7, "hug"));
		mapEmozioniName.insert(pair<int, string>(7, "HUG ME"));
		mapEmozioni.insert(pair<int, string>(8, "exit"));
		mapEmozioniName.insert(pair<int, string>(8, "CLOSE"));

		mapTv.insert(pair<int, string>(0, "sport"));
		mapTvName.insert(pair<int, string>(0, "SPORT"));
		mapTv.insert(pair<int, string>(1, "cinema"));
		mapTvName.insert(pair<int, string>(1, "CINEMA"));
		mapTv.insert(pair<int, string>(2, "documentary"));
		mapTvName.insert(pair<int, string>(2, "DOCUMENTARY"));
		mapTv.insert(pair<int, string>(3, "animation"));
		mapTvName.insert(pair<int, string>(3, "ANIMATION"));
		mapTv.insert(pair<int, string>(4, "previous_channel"));
		mapTvName.insert(pair<int, string>(4, "PREVIOUS CHANNEL"));
		mapTv.insert(pair<int, string>(5, "next_channel"));
		mapTvName.insert(pair<int, string>(5, "NEXT CHANNEL"));
		mapTv.insert(pair<int, string>(6, "back"));
		mapTvName.insert(pair<int, string>(6, "BACK"));
		mapTv.insert(pair<int, string>(7, "tvOff"));
		mapTvName.insert(pair<int, string>(7, "TV OFF"));
		mapTv.insert(pair<int, string>(8, "exit"));
		mapTvName.insert(pair<int, string>(8, "CLOSE"));

		mapTelefono.insert(pair<int, string>(0, "answer"));
		mapTelefonoName.insert(pair<int, string>(0, "ANSWER THE PHONE"));
		mapTelefono.insert(pair<int, string>(1, "call_1"));
		mapTelefonoName.insert(pair<int, string>(1, "FAVORITE N. 1"));
		mapTelefono.insert(pair<int, string>(2, "call_2"));
		mapTelefonoName.insert(pair<int, string>(2, "FAVORITE N. 2"));
		mapTelefono.insert(pair<int, string>(3, "call_3"));
		mapTelefonoName.insert(pair<int, string>(3, "FAVORITE N. 3"));
		mapTelefono.insert(pair<int, string>(4, "call_4"));
		mapTelefonoName.insert(pair<int, string>(4, "FAVORITE N. 4"));
		mapTelefono.insert(pair<int, string>(5, "police"));
		mapTelefonoName.insert(pair<int, string>(5, "POLICE"));
		mapTelefono.insert(pair<int, string>(6, "back"));
		mapTelefonoName.insert(pair<int, string>(6, "BACK"));
		mapTelefono.insert(pair<int, string>(7, "doctor"));
		mapTelefonoName.insert(pair<int, string>(7, "DOCTOR"));
		mapTelefono.insert(pair<int, string>(8, "exit"));
		mapTelefonoName.insert(pair<int, string>(8, "CLOSE"));

		mapRadio.insert(pair<int, string>(0, "previous_station"));
		mapRadioName.insert(pair<int, string>(0, "PREVIOUS STATION"));
		mapRadio.insert(pair<int, string>(1, "next_station"));
		mapRadioName.insert(pair<int, string>(1, "NEXT STATION"));
		mapRadio.insert(pair<int, string>(2, "1_radio"));
		mapRadioName.insert(pair<int, string>(2, "CHANNEL 1"));
		mapRadio.insert(pair<int, string>(3, "2_radio"));
		mapRadioName.insert(pair<int, string>(3, "CHANNEL 2"));
		mapRadio.insert(pair<int, string>(4, "3_radio"));
		mapRadioName.insert(pair<int, string>(4, "CHANNEL 3"));
		mapRadio.insert(pair<int, string>(5, "4_radio"));
		mapRadioName.insert(pair<int, string>(5, "CHANNEL 4"));
		mapRadio.insert(pair<int, string>(6, "back"));
		mapRadioName.insert(pair<int, string>(6, "BACK"));
		mapRadio.insert(pair<int, string>(7, "radioOff"));
		mapRadioName.insert(pair<int, string>(7, "RADIO OFF"));
		mapRadio.insert(pair<int, string>(8, "exit"));
		mapRadioName.insert(pair<int, string>(8, "CLOSE"));

		for (int i=0; i<36; i++) {
			string path = "W" + std::to_string(i + 1);
			string name;
			//char tmp = ' ';
			if (i <= 25) {
				/* SLB TODO check
				tmp = 'A' + i;
				stringstream ss;
				ss << tmp;
				ss >> name;
				mapSpeller.insert(pair<int, string>(i, name));
				*/
				name = std::to_string('A' + i); // SLB
			}
			else if (i != 30) {
				/* SLB TODO check
				tmp = '1' + i - 26;
				stringstream ss;
				ss << tmp;
				ss >> name;
				mapSpeller.insert(pair<int, string>(i, name));
				*/
				name = std::to_string('1' + i - 26); // SLB
			}
			else {
				name = "BACK";
				//mapSpeller.insert(pair<int, string>(i, name));
			}
			mapSpeller.insert(pair <int, string> (i, name)); // SLB
			
		}

		mapVector = new map<int,string>[16];
		mapVectorName = new map<int,string>[16];

		mapVector[CIBO] = mapCibo;
		mapVectorName[CIBO] = mapCiboName;

		if (isHot) {
			mapVector[TEMP] = mapCaldo;
			mapVectorName[TEMP] = mapCaldoName;
		}
		else {
			mapVector[TEMP] = mapFreddo;
			mapVectorName[TEMP] = mapFreddoName;
		}

		mapVector[BERE] = mapBere;
		mapVectorName[BERE] = mapBereName;
		
		mapVector[TV] = mapTv;
		mapVectorName[TV] = mapTvName;
		
		mapVector[RADIO] = mapRadio;
		mapVectorName[RADIO] = mapRadioName;

		mapVector[LETTO] = mapLetto;
		mapVectorName[LETTO] = mapLettoName;

		mapVector[TELEFONO] = mapTelefono;
		mapVectorName[TELEFONO] = mapTelefonoName;

		mapVector[EMOZIONI] = mapEmozioni;
		mapVectorName[EMOZIONI] = mapEmozioniName;
	}
}

// SLB
void initSpellingText() {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB

	pugi::xml_node tmp = doc.child("target");
	testoPre = testoProcessed = "";

	if (symbolicTexture) {
		// SLB TODO CHECK following two lines commented, possibly solved the symbolic matrix redraw issue?
		//rowsNumber = 4;
		//elements = squaresNumber = 16;

		for (pugi::xml_node tool = tmp.child("t"); tool; tool = tool.next_sibling("t")) {
			testoPre += tool.first_child().value();
			testoPre += " "; // SLB
			targets.push_back(tool.first_child().value());
		}
	}
	else { // SLB added to manage the alphabetic speller string
		// SLB Speller PALETTE
		for (pugi::xml_node tool = tmp.child("t"); tool; tool = tool.next_sibling("t")) {
			testoPre += mapTextureName[atoi(tool.first_child().value())-1];
			targets.push_back(tool.first_child().value());
		}
	}

	testoProcessed = testoPre + " (" + targets[currentSession] + ")";
	maxSessions = targets.size(); // SLB

}

// SLB
void initTexturesTheme() {
	
	if (customPalette) { // SLB
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB

		texturetype += doc.child("letter_color").first_child().value();
		texturetype += "_";
		texturetype += doc.child("background_color").first_child().value();
		texturetype += "/";

		string selection;

		/*Seleziono il background dalla mappa*/
	    selection = colorMap[doc.child("background_color").first_child().value()];
		char *s4 = new char[selection.size() + 1];
		memcpy(s4, selection.c_str(), selection.size() + 1);
		char *pch = strtok(s4, "_");
		int i = 0;
		while (pch != NULL) {
			backGroundColor[i] = atof(pch);
			pch = strtok(NULL, "_");
			i++;
		}

		/*Seleziono il colore del testo*/
		selection = colorMap[doc.child("letter_color").first_child().value()];
		char *ss4 = new char[selection.size() + 1];
		memcpy(ss4, selection.c_str(), selection.size() + 1);
		char *ppch = strtok(ss4, "_");
		i = 0;
		while (ppch != NULL) {
			textColor[i] = atof(ppch);
			ppch = strtok(NULL, "_");
			i++;
		}

		// SLB delete[]
		/*
		delete[] s4;
		delete[] pch;
		delete[] ss4;
		delete[] ppch;
		*/
	}
	else {
		texturetype = "2K/";
	}

	texturePathName = texturePathRoot + "/" + texturetype; // SLB

}

// SLB
void initObjects() {

	interface		= new P300Interface(elements, g_Width, g_Height, timeFlash, numberOfFlash, maxSessions-1, texturePathName);
	squaresVector	= new P300Square*[squaresNumber];
	rowsVector		= new P300Row*[rowsNumber];
	u_coor			= new GLfloat[squaresNumber];
	v_coor			= new GLfloat[squaresNumber];
	tpnVector		= new string[squaresNumber];
	tpnfVector		= new string[squaresNumber];
	showSquare		= new bool[squaresNumber];
	showRow			= new bool[rowsNumber];
	showColumn		= new bool[rowsNumber];

}

void initializeAll() {

	readConfiguration();
	
	setColorMap();

	initMapTexture(); // SLB moved here, necessary to create the spelling text strings

	initSpellingText(); // SLB

	initTexturesTheme(); // SLB

	initObjects(); // SLB
	
	initParameterVectors(); // Inizializzo i parametri e i quadrati


	initPaths();

	initSquare();

	initCounterSquare();

	initCounterRow();

	initCounterColumn();

	initTobii(); // SLB

}
// ----------------------------------------------------------------------------
// -------------------- Fine sezione quadrati ---------------------------------
//-----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// ----------------------- Funzioni relative ai flash --------------------------
// -----------------------------------------------------------------------------
void flashSquare(int square) {

	if (firstTime) { // Invio comando di inizio stimolo
		sendToServer(START_STIMULUS);
		fillStackS();
		readyToFlash = false;
		firstTime = false;
		pausa = false;
	}

	if (!pausa) { // SLB
		
		cout << " Stack size : " << flashHelperS.size() << endl;

		if (!flashHelperS.empty()) {
			
			// SLB
			if (tobiienabled)
				EyeT->setRunning(true);
			
			int tmp = flashHelperS.top();
			flashHelperS.pop();
			showSquare[tmp] = !showSquare[tmp];

			if (showSquare[tmp]) {
				counterSquares[tmp]++;
				sendToServer(tmp + 1);
			}

			square = tmp;
			glutPostRedisplay();

			if (flashHelperS.size() >= 0)
				glutTimerFunc(interface->getFlashFrequency(), flashSquare, 0);
			
		}
		else {
			// SLB
			if (tobiienabled)
				EyeT->setRunning(false);

			initCounterSquare();
			sendToServer(END_STIMULUS);
			pausa = true;
			readyToFlash = true;
			
			if (currentSession == interface->getMaxSessions()) { // SLB
				sendToServer(END_CALIBRATION);
				drawOnScreen();
				firstTime = true;
			}
			else {
				currentSession++;
			}

			// SLB
			if (tobiienabled)
				EyeT->Process(g_Width, g_Height, text_height);
			
		}
	}
}

void fillStackS() {
	int* tmpr = new int[squaresNumber * numberOfFlash];  // Vettore delle permutazioni
	
	for (int i=0; i<squaresNumber * numberOfFlash; i++) {
		tmpr[i] = i % squaresNumber;
	}

	int start, stop;
	for (int i=0; i<numberOfFlash; i++) {
		start = (i * squaresNumber);
		stop = ((i+1) * squaresNumber);
		random_shuffle(&tmpr[start], &tmpr[stop]);
	}

	//random_shuffle(&tmpr[0], &tmpr[squaresNumber  * numberOfFlash - 1]);

	for (int i=0; i<squaresNumber * numberOfFlash; i++) {
		flashHelperS.push(tmpr[i]);
		flashHelperS.push(tmpr[i]);
	}

}

void flashRC(int val) {
	int square;

	if (firstTime) { // Invio comando di inizio stimolo
		sendToServer(START_STIMULUS);
		fillStackRC();
		firstTime = false;
		readyToFlash = false;
		pausa = false;
	}

	if (!pausa) { // SLB
		
		cout << " Stack size : " << flashHelperRC.size() << endl;

		if (!flashHelperRC.empty()) { // Verifica che lo stack non sia vuoto

			// SLB
			if (tobiienabled)
				EyeT->setRunning(true);

			int tmp = flashHelperRC.top();
			flashHelperRC.pop(); // Ricavo il primo valore in cima allo stack
			if ((stackIndex % 4) < 2) { // Verifico se sia una colonna o una riga
				showColumn[tmp] = !showColumn[tmp];
				if (showColumn[tmp]) { // Aumenta il contatore e manda l'ID della colonna 
					counterColumns[tmp]++;
					sendToServer(tmp + 1 + rowsNumber);
				}

				for (int i=0; i<rowsNumber; i++) { // Flasha la colonna
					showSquare[tmp + (i * rowsNumber)] = !showSquare[tmp + (i * rowsNumber)];
				}
			}
			else { // � una riga
				showRow[tmp] = !showRow[tmp];
				if (showRow[tmp]) { // Aumenta il contatore e manda l'ID della riga 
					counterRows[tmp]++;
					sendToServer(tmp + 1);
				}

				for (int i=0; i<rowsNumber; i++) { // flasha la riga
					showSquare[(tmp * rowsNumber) + i] = !showSquare[(tmp * rowsNumber) + i];
				}
			}

			stackIndex++;
			square = tmp;
			glutPostRedisplay();

			if (flashHelperRC.size() >= 0)
				glutTimerFunc(interface->getFlashFrequency(), flashRC, val);

		}
		else {
			// SLB
			if (tobiienabled)
				EyeT->setRunning(false);
			
			initCounterRow();
			initCounterColumn();
			sendToServer(END_STIMULUS);
			pausa = true;
			readyToFlash = true;
			
			if (currentSession == interface->getMaxSessions()) { // SLB
				sendToServer(END_CALIBRATION);
				firstTime = true;
			}
			else {
				currentSession++;
			}

			// SLB
			if (tobiienabled)
				EyeT->Process(g_Width, g_Height, text_height);
			
		}
	}
}

void fillStackRC() {
	// La funzione si basa sul fatto che l'inserimento nello stack � sequenziale,
	// dunque il primo elemento sulla cima dello stack sar� sicuramente una colonna.
	int* tmpr = new int[rowsNumber*numberOfFlash]; // Vettore di permutazione delle righe
	int* tmpc = new int[rowsNumber*numberOfFlash]; // Vettore di permutazione delle colonne
	
	for (int i=0; i<rowsNumber*numberOfFlash; i++) {
		tmpr[i] = i % rowsNumber;
		tmpc[i] = i % rowsNumber;
	}

	int start, stop;
	for (int i=0; i<numberOfFlash; i++) {
		start = (i * rowsNumber);
		stop = ((i+1) * rowsNumber);
		random_shuffle(&tmpr[start], &tmpr[stop]);
		random_shuffle(&tmpc[start], &tmpc[stop]);
	}

	//	random_shuffle(&tmpr[0], &tmpr[rowsNumber*numberOfFlash - 1]);
	//	random_shuffle(&tmpc[0], &tmpc[rowsNumber*numberOfFlash - 1]);

	for (int i=0; i<numberOfFlash*rowsNumber; i++) {
		flashHelperRC.push(tmpr[i]);
		flashHelperRC.push(tmpr[i]);
		flashHelperRC.push(tmpc[i]);
		flashHelperRC.push(tmpc[i]);
	}

}
// ----------------------------------------------------------------------------
// -------------------- Fine sezione dei flash-- ------------------------------
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// --------------------------- Sezione funzioni varie -------------------------
// ----------------------------------------------------------------------------
void deleteAll() { // Funzione che cancella tutti i puntatori instanziati
	delete interface;

	for (int i=0; i<squaresNumber; i++) {
		delete squaresVector[i];
	}
	delete squaresVector;

	for (int i=0; i<rowsNumber; i++) {
		delete rowsVector[i];
	}
	delete rowsVector;
	
	delete u_coor;
	delete v_coor;
	delete tpnVector;
	delete tpnfVector;
	delete showSquare;
	delete showRow;
	delete showColumn;

	delete counterSquares;
	delete counterRows;
	delete counterColumns;
	
	if (tobiienabled) // SLB
		delete EyeT;
	
}

void sendToServer(int com) {

	clientTCP *conn = new clientTCP;
	
	if (!SLBDEBUG) // SLB
		conn->sendCommand(std::to_string(com).c_str()); // SLB
	delete conn;
}

void readConfiguration() {
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB

	if (result)	{
		elements		= atoi(doc.child("elements").first_child().value());
		rowsNumber		= atoi(doc.child("row_num_interface").first_child().value());
		squaresNumber	= rowsNumber*rowsNumber;
		numberOfFlash	= atoi(doc.child("num_flash").first_child().value());
		timeFlash		= atoi(doc.child("time_flash").first_child().value());
		textureFlash	= doc.child("flashing_bmptexture").first_child().value(); // SLB
		symbolicTexture	= (bool) atoi(doc.child("symbolicmode").first_child().value());
		customPalette	= (bool) atoi(doc.child("palette").first_child().value());
		flashingMode	= (bool) atoi(doc.child("mode").first_child().value()); // SLB bool
		tobiienabled	= (bool) atoi(doc.child("tobiienabled").first_child().value()); // SLB
		
	}
	else {
		cout << "Could not find configuration file." << endl;
		/*
		std::cout << "XML  parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
		std::cout << "Error description: " << result.description() << "\n";
		std::cout << "Error offset: " << result.offset << " (error at [..." << (result.offset) << "]\n\n";
		*/
	}
}
// ----------------------------------------------------------------------------
// --------------------------- Fine funzioni varie ----------------------------
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// ---------------------- Inizio sezione scritta a video ----------------------
// ----------------------------------------------------------------------------
int selectTextureToShow() {

	if (idFromClient[1] == -1)
		return (idFromClient[0] - 1);

	int i = idFromClient[0] - 1;
	int j = idFromClient[1] - 1;

	return ((i*rowsNumber) + (j-rowsNumber));
}

void printStringOnVideo(void* font, string s) {

// don't forget to enable it later when you need it
	for (int i=0; i<s.size(); i++)
		glutBitmapCharacter(font, s[i]);

}

void drawOnScreen() {
	// SLB
	//int width = glutGet(GLUT_SCREEN_WIDTH);
	//int height = glutGet(GLUT_SCREEN_HEIGHT);
	int width = g_Width;
	int height = g_Height;
	//int x = (width/2) - testoProcessed.size()*6;
	//int x = (width/2) - testoProcessed.size()*4;
	int x = text_height;
	int y = height-text_height;

	glRasterPos2f(x, y);

	processText();

	//printStringOnVideo(GLUT_BITMAP_TIMES_ROMAN_24, testoProcessed); // SLB
	if (glutGet(GLUT_WINDOW_HEIGHT) < (height/2))
		printStringOnVideo(GLUT_BITMAP_HELVETICA_12, testoProcessed);
	else
		printStringOnVideo(GLUT_BITMAP_HELVETICA_18, testoProcessed);
}

// SLB
void drawIDs() {
	if (symbolicTexture || customPalette) {

		glColor3f(textColor[0], textColor[1], textColor[2]);
		for (int i=0; i<rowsNumber*rowsNumber; i++) {
			glRasterPos2f(u_coor[i]-5, v_coor[i]);
			printStringOnVideo(GLUT_BITMAP_HELVETICA_12, std::to_string(i+1));
		}

	}
}

void processText() {

	if (!spellingOn) {
		if (symbolicTexture || customPalette) { // SLB TODO CHECK preferences of having different calibration strings depending on the spelling mode
			testoProcessed = testoPre + " (" + targets[currentSession] + ")";
			
		}
		else
			testoProcessed = testoPre + " (" + testoPre[currentSession] + ")";
		
	} else {
		testoPre = testoSpelling;
		testoProcessed = testoPre;
	}
}

void updateTestoSpelling(string res) {

	if (symbolicTexture && (!spellingController)) // SLB
		testoSpelling = res; // SLB
	else
		testoSpelling = testoSpelling + " " + res;
}
// ----------------------------------------------------------------------------
// ---------------------------- Fine scritta a video --------------------------
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// --------------------------- Sezione funzioni rendering----------------------
// ----------------------------------------------------------------------------
void display(void) {

	glClearColor(backGroundColor[0], backGroundColor[1], backGroundColor[2], backGroundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// SLB
	/*
	float const screen_width	= glutGet(GLUT_SCREEN_WIDTH);
	float const screen_height	= glutGet(GLUT_SCREEN_HEIGHT);
	float const win_width		= glutGet(GLUT_WINDOW_WIDTH);
	float const win_height		= glutGet(GLUT_WINDOW_HEIGHT);
	float const screen_aspect	= (float)screen_width / (float)screen_height;
	float const win_aspect		= (float)win_width / (float)win_height;
	float const x_aspect		= (float)screen_width / (float)win_width;
	float const y_aspect		= (float)screen_height / (float)win_height;
	*/
	float const win_width		= glutGet(GLUT_WINDOW_WIDTH);
	float const win_height		= glutGet(GLUT_WINDOW_HEIGHT);
	
	glViewport(0, 0, win_width, win_height);
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	//glOrtho(-0.5, screen_width-0.5, -0.5, screen_height-0.5, -1.0, 1.0);
	glOrtho(-0.5, g_Width-0.5, -0.5, g_Height-0.5, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);

	// Set up viewing transformation, looking down -Z axis
	///glLoadIdentity();
	//gluLookAt(posx, 0, -g_fViewDistance, 0, 0, -1, 0, 1, 0);

	// Render the scene
	glMatrixMode(GL_COLOR);
	glColor3f(textColor[0], textColor[1], textColor[2]);

	drawOnScreen();
	
	glMatrixMode(GL_MODELVIEW);

	loadSquare(); // Carico le texture

	// SLB drawIDs
	drawIDs();
	//
	
	// SLB
	if (tobiienabled) {
		EyeT->showGaze(g_Width, g_Height);
		EyeT->showInnerSquare(g_Width, g_Height, text_height, rowsNumber); // SLB TODO performance fix
	}

	// SLB TODO CHECK maybe fixes mouse move between targets
	///glutPostRedisplay();
	//

	//glFlush(); // SLB ? added for GL_SINGLE
	glutSwapBuffers(); // SLB for GL_DOUBLE
}

void reshape(GLint width, GLint height) {
	
	if (square_ratio) {
		if (height < 300)
			height = 300;
		glutReshapeWindow(height, height);
	}
	else {
		glutReshapeWindow(width, height);
	}

}

const char* BoolToString(bool b) {
  return b ? "1" : "0";
}

// SLB
void updateXML(int res) {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/memory.xml").c_str()); // SLB
	
	if (result) {
		// SLB TODO possible optimisation by matching memory.xml string with int constants to eliminate all the if's
		if (res == FINESTRA) {  // FINESTRA
			pugi::xml_node node = doc.child("window");
			node.first_child().set_value(BoolToString(memoryInformation[FINESTRA]));
		}
		else if (res == PORTA) {  // PORTA
			pugi::xml_node node = doc.child("door");
			node.first_child().set_value(BoolToString(memoryInformation[PORTA]));
		}
		else if (res == LUCE) {  // SLB
			pugi::xml_node node = doc.child("light");
			node.first_child().set_value(BoolToString(memoryInformation[LUCE]));
		}
		doc.save_file((configFilesRoot + "/memory.xml").c_str()); // SLB
	}
}

void Keyboard(unsigned char key, int x, int y) {
	string s1, s2;
	// M inizia il flash dei quadrati, N di righe e colonne
	// SLB redundant?
	if (spellerHandler)
		rowsNumber = SPELLER_ON;
	//

	switch (key) {
		case 's':
			if (readyToFlash) {
				readyToFlash = false;
				s1 = testoPre;
				s2 = testoProcessed;
				readConfiguration();

				if (flashingMode == 0) { // flash square
					printf("Flashing squares\n");
					sendToServer(START_CALIBRATION);
					sendToServer(FLASH_S);
					firstTime = true;
					Sleep(1000);
					currentSession=0;
					flashSquare(0);
				}
				else if (flashingMode == 1) { // rows-cols // SLB
					printf("Flashing rows and columns\n");
					sendToServer(START_CALIBRATION);
					sendToServer(FLASH_RC);
					firstTime = true;
					Sleep(1000);
					currentSession=0;
					flashRC(0);
				}
				
				if (symbolicTexture && !spellingOn) {
					testoPre = s1;
					testoProcessed = s2;
				}

			}
			break;

		case 'x':
			glutDestroyWindow(window); // SLB
			exit(0);
			break;
	}

}
/* SLB Disabled to let the gui cycle thru the targets automatically
// ^ doesn't really work: still pauses, sometimes?!
void MouseButton(int button, int state, int x, int y) {
	// Respond to mouse button presses.
	// If button1 pressed, mark this state so we know in motion function.
	if (button == GLUT_LEFT_BUTTON) {
		g_bButton1Down = (state == GLUT_DOWN) ? TRUE : FALSE;
		g_yClick = y - 3 * g_fViewDistance;
	}

	if (button == GLUT_RIGHT_BUTTON) {
		g_bButton2Down = (state == GLUT_DOWN) ? TRUE : FALSE;
		g_xClick = x - 3 * g_fViewDistance; glMatrixMode(GL_COLOR);
	}
}

void MouseMotion(int x, int y) {
	// If button1 pressed, zoom in/out if mouse is moved up/down.
	if (g_bButton1Down) {
		g_fViewDistance = (y - g_yClick) / 3.0;
		if (g_fViewDistance < VIEWING_DISTANCE_MIN)
			g_fViewDistance = VIEWING_DISTANCE_MIN;
		glutPostRedisplay();
	}

	if (g_bButton2Down) {
		g_fViewDistance = (x - g_xClick) / 3.0;
		if (g_fViewDistance < VIEWING_DISTANCE_MIN)
			g_fViewDistance = VIEWING_DISTANCE_MIN;
		glutPostRedisplay();
	}
}
*/
// ----------------------------------------------------------------------------
// --------------------------- Fine funzioni rendering-------------------------
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// --------------------------- Inizio funzioni per la socket ------------------
// ----------------------------------------------------------------------------
DWORD WINAPI createSocket(LPVOID lpParameter) {

	WSADATA wsa;
	SOCKET master, new_socket, client_socket[1], s;
	struct sockaddr_in server, address;
	int max_clients = 1, activity, addrlen, i, valread;
	char *message = "";

	//size of our receive buffer, this is string length.
	int MAXRECV = 1024;
	//set of socket descriptors
	fd_set readfds;
	//1 extra for null character, string termination
	char *buffer;
	buffer = (char*)malloc((MAXRECV + 1) * sizeof(char));

	for (int i=0; i<1; i++) {
		client_socket[i] = 0;
	}

	cout << "UniPA BCI Framework Copyright (C) 2016-2017 Salvatore La Bua\n"
			"RoboticsLab, Department of Industrial and Digital Innovation (DIID),\n"
			"Universita' degli Studi di Palermo, V. delle Scienze, Palermo, Italy.\n"
			"This program comes with ABSOLUTELY NO WARRANTY.\n"
			"This is free software, and you are welcome to redistribute it\n"
			"under certain conditions.\n\n";
	printf("GUI: Initialising Winsock... "); // SLB
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	printf("Initialised.\n");

	try {
		//Create a socket
		if ((master = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
			printf("Could not create socket : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		printf("GUI: Socket created.\n");

		//Prepare the sockaddr_in structure
		server.sin_family = AF_INET;
		inet_pton(AF_INET, "127.0.0.2", &(server.sin_addr));
		server.sin_port = htons(8888);

		//std::bind(master, (struct sockaddr *)&server, sizeof(server));

		//Bind
		if (bind(master, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
			printf("Bind failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		puts("GUI: Bind done.\n"); 

		//Listen to incoming connections
		listen(master, 3);

		//Accept and incoming connection
		//  puts("Waiting for incoming connections...");

		addrlen = sizeof(struct sockaddr_in);

		while (true) {
			//clear the socket fd set
			FD_ZERO(&readfds);

			//add master socket to fd set
			FD_SET(master, &readfds);

			//add child sockets to fd set
			for (int i=0; i<max_clients; i++) {
				s = client_socket[i];
				if (s > 0) {
					FD_SET(s, &readfds);
				}
			}

			//wait for an activity on any of the sockets, timeout is NULL , so wait indefinitely
			activity = select(0, &readfds, NULL, NULL, NULL);

			if (activity == SOCKET_ERROR) {
				printf("select call failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}

			//If something happened on the master socket , then its an incoming connection
			if (FD_ISSET(master, &readfds)) {
				if ((new_socket = accept(master, (struct sockaddr *)&address, (int *)&addrlen))<0) {
					perror("accept");
					exit(EXIT_FAILURE);
				}

				//add new socket to array of sockets
				for (int i=0; i<max_clients; i++) {
					if (client_socket[i] == 0) {
						client_socket[i] = new_socket;
						break;
					}
				}
			}

			//else its some IO operation on some other socket :)
			for (int i=0; i<max_clients; i++) {
				s = client_socket[i];
				//if client presend in read sockets             
				if (FD_ISSET(s, &readfds)) {
					//get details of the client
					getpeername(s, (struct sockaddr*)&address, (int*)&addrlen);

					//Check if it was for closing , and also read the incoming message
					//recv does not place a null terminator at the end of the string (whilst printf %s assumes there is one).
					valread = recv(s, buffer, MAXRECV, 0);

					if (valread == SOCKET_ERROR) {
						int error_code = WSAGetLastError();
						if (error_code == WSAECONNRESET) {
							//Somebody disconnected , get his details and print
							printf("Host disconnected unexpectedly , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

							//Close the socket and mark as 0 in list for reuse
							closesocket(s);
							client_socket[i] = 0;
						}
						else {
							printf("recv failed with error code : %d", error_code);
						}
					}

					if (valread == 0) {
						//Close the socket and mark as 0 in list for reuse
						closesocket(s);
						client_socket[i] = 0;
					}
					else {
						//add null character, if you want to use with printf/puts or other string handling functions
						buffer[valread] = '\0';

						char *pch;
						int i = 0;

						pch = strtok(buffer, "-");
						while (pch != NULL) {
							idFromClient[i] = atoi(pch);
							pch = strtok(NULL, "-");
							i++;
						}

						if (idFromClient[0] == RESTART_RC) {
							/* SLB TODO partly fixes the spelling text when selecting calibration after a spelling session
							spellingOn = false;
							initSpellingText();
							*/
							sendToServer(FLASH_RC);
							firstTime = true;
							flashRC(0);
						}

						if (idFromClient[0] == RESTART_S) {
							/* SLB TODO partly fixes the spelling text when selecting calibration after a spelling session
							spellingOn = false;
							initSpellingText();
							*/
							sendToServer(FLASH_S);
							firstTime = true;
							flashSquare(0);
						}

						if (idFromClient[0] == START_SPELLER) {
							cout << "Starting speller." << endl;
							testoPre = testoProcessed = "";
							spellingOn = true;
							drawOnScreen();
							glutPostRedisplay(); // SLB
						}

						if (idFromClient[0] == START_FRAMEWORK) {
							cout << "Starting framework." << endl << endl;
							startFramework = true;
						}
						
						if (idFromClient[0] == KILL_FRAMEWORK) { // SLB
							cout << endl << "Stopping framework..." << endl;
							glutDestroyWindow(window);
							return 0;
						}

						if (idFromClient[0] < START_CALIBRATION && idFromClient[0] > 0) {
							
							cout << idFromClient[0];
							if (idFromClient[1] != -1)
								cout << " - " << idFromClient[1];
							cout << endl;

							int res = selectTextureToShow();

							if (symbolicTexture) {
								//controllo qual e' stata l'ultima selezione
								if (lastSensorialSelection == -1) {
									cout << mapTextureName[res] << endl << endl; // SLB added endl endl
									updateTestoSpelling(mapTextureName[res]);
								}
								else {
									cout << mapVectorName[lastSensorialSelection][res] << endl;
									updateTestoSpelling(mapVectorName[lastSensorialSelection][res]);
								}
							}
							else
								updateTestoSpelling(mapTextureName[res]);

							testoPre = testoProcessed = res; // SLB
							spellingOn = true;
							drawOnScreen();
							glutPostRedisplay(); // SLB

							//se l'id ritornato, e' quello della finestra, devo modificare la memoria
							/*
							if (symbolicTexture && (!spellingController) && (lastSensorialSelection == -1) &&
								((res == FINESTRA) || (res == PORTA))) {
							*/
							//SLB
							if (symbolicTexture && (rowsNumber == 4) && (!spellingController) && (lastSensorialSelection == -1) &&
								((res == FINESTRA) || (res == PORTA) || (res == LUCE))) {
									
									// SLB new
									memoryInformation[res] = !memoryInformation[res];

									updateXML(res);
									rebuildMainWindow(res);

							}

							//sendCommandToRobot(res); // SLB TODO CHECK here GEMINOID INTERFACE

							if (symbolicTexture) //se � attivo il modulo dei sensori, potrei voler modificare l'interfaccia
								rebuildInterface(res);
							else {
								if (res == CHIUDI) { //devo uscire dall'interfaccia
									// ??
									// SLB TODO CHECK workaround
									//res = BACK;
									//rebuildInterface(res);
									//
								}
							}

						}

						// SLB delete[]
						//delete[] pch;
						
					}
				}
			}
		}
	}
	catch(int exception_code) {
	}
	
	printf("\n closing stream data\n");
	SuspendThread(0);
	
	// SLB delete[]
	/*
	delete[] message;
	delete[] buffer;
	*/

	return 0;
}

void rebuildInterface(int res) {

	//se abbiamo selezionato lo speller, dobbiamo chiamare una funzione apposita che lo gestisca
	if ((res == SPELLER) || spellingController) {
		manageSpeller(res);
		return;
	}
	
	//controllo se per l'indice in questione devo modificare le texture  se ci troviamo attualmente nella matrice principale
	if (mapVector[res].size()>0 || lastSensorialSelection!=-1) {
		
		//devo chiudere
		//if (res == CHIUDI_SOTTOINTERFACCIA) {
		if (res == CLOSE) { // SLB renamed const
			// SLB ?? was empty already
			// SLB TODO CHECK workaround
			//texturePathName = texturePathRoot + "/2K/"; // path di default
			//testoSpelling = "";
			//reloadParameters(res); // ricarico l'interfaccia
			//rebuildMainWindow(res);
			res = BACK;
			//
		}

		//indice dello speller
		//a seconda dell'indice scelto devo ricaricare i parametri della dimensione
		if (res == BACK) { // l'indice BACK serve per tornare all'interfaccia principale
			lastSensorialSelection = -1; //indice di default;
			texturePathName = texturePathRoot + "/" + texturetype; // path di default
			testoSpelling = "";
			reloadParameters(res); // ricarico l'interfaccia
		}
		else { //devo caricare il set di texture corretto
			if (lastSensorialSelection == -1) {
				texturePathName = texturePathRoot + "/" + texturetype + mapTexture[res] + "/"; //mi sposto nel path corretto
				lastSensorialSelection = res;
				testoSpelling = "";
				reloadParameters(res);	
			}
			
			//lastSensorialSelection=res; // SLB TODO CHECK was commented already

		  }
	}
	//Eventualmente inserire delete
	
}

void rebuildMainWindow(int res) {
	
		getMemoryInformation(); // SLB

		// SLB
		setTexturesFromMemory();

		testoSpelling = "";

		/* SLB disabled printing matrix ids
		cout << endl; // SLB
		for (map<int, string> ::const_iterator it = mapTextureName.begin(); it != mapTextureName.end(); ++it) {
			cout << it->first << " " << it->second << endl;
		}
		*/

		reloadParameters(BACK);

}

// SLB
void setTexturesFromMemory() {
	if (memoryInformation[FINESTRA]) { //se dalla memoria sappiamo gi� che la finestra � aperta
		mapTexture[FINESTRA] = "windowClose";
		mapTextureName[FINESTRA] = "CLOSE THE WINDOW";
	}
	else {
		mapTexture[FINESTRA] = "windowOpen";
		mapTextureName[FINESTRA] = "OPEN THE WINDOW";
	}
	
	if (memoryInformation[PORTA]) { //se l'indice booleano della porta � aperta,
		mapTexture[PORTA] = "doorClose";
		mapTextureName[PORTA] = "CLOSE THE DOOR";
	}
	else {
		mapTexture[PORTA] = "doorOpen";
		mapTextureName[PORTA] = "OPEN THE DOOR";
	}

	if (memoryInformation[LUCE]) { // SLB
		mapTexture[LUCE] = "lightOff";
		mapTextureName[LUCE] = "TURN OFF THE LIGHT";
	}
	else {
		mapTexture[LUCE] = "lightOn";
		mapTextureName[LUCE] = "TURN ON THE LIGHT";
	}
}

void reloadParameters(int res) {
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB

	if (res == BACK) {
		rowsNumber = 4;
		elements = squaresNumber = 16;
	}
	else {
		rowsNumber = 3;
		elements = squaresNumber = 9;
	}

	pugi::xml_node node = doc.child("row_num_interface");
	node.first_child().set_value(std::to_string(rowsNumber).c_str());
	node = doc.child("elements");
	node.first_child().set_value(std::to_string(elements).c_str());
	doc.save_file((configFilesRoot + "/configuration.xml").c_str()); // SLB

	initObjects(); // SLB

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clean the screen and the depth buffer
	glutSwapBuffers();

	// Inizializzo i parametri e i quadrati
	initParameterVectors();

	for (int i=0; i<squaresNumber; i++) {
			if (lastSensorialSelection == -1)
			    tpnVector[i] = interface->getTexturePathName() + mapTexture[i] + ".bmp";				
			else
				tpnVector[i] = interface->getTexturePathName() + mapVector[res][i] + ".bmp";

			tpnfVector[i] = texturePathRoot + "/" + texturetype + "/" + textureFlash + ".bmp"; // SLB
	}
	
	initSquare();

	initCounterSquare();

	initCounterRow();

	initCounterColumn();

	glutDisplayFunc(display);	// Setta la callback per il display
	glutReshapeFunc(reshape);
	glutPostRedisplay();

}

void manageSpeller(int res) {
	if (spellingController) {
		if (res != 30) {
			updateTestoSpelling(mapSpeller[res]);
		}
		else {
			spellingController = false;
			lastSensorialSelection = -1;
			//reloadParameters(6);
			reloadParameters(BACK); // SLB
		}
	}
	else {
		testoSpelling = "";
		spellingController = true;
		lastSensorialSelection = res;
		squaresNumber = elements = 36;
		rowsNumber = 6;

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB

		pugi::xml_node node = doc.child("row_num_interface");
		node.first_child().set_value(std::to_string(rowsNumber).c_str());
		node = doc.child("elements");
		node.first_child().set_value(std::to_string(elements).c_str());
		doc.save_file((configFilesRoot + "/configuration.xml").c_str()); // SLB

		initObjects(); // SLB

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clean the screen and the depth buffer
		glutSwapBuffers();

		// Inizializzo i parametri e i quadrati
		initParameterVectors();

		for (int i=0; i<squaresNumber; i++) {
			tpnVector[i] = interface->getTexturePathName() + "speller/W" + std::to_string(i+1) + ".bmp";
			tpnfVector[i] = interface->getTexturePathName() + "/" + textureFlash + ".bmp";
		}

		initSquare();

		initCounterSquare();

		initCounterRow();

		initCounterColumn();

		glutDisplayFunc(display);	// Setta la callback per il display
		glutReshapeFunc(reshape);
		glutPostRedisplay();

	}

}

void sendCommandToRobot(int res) {
		RobotController* robot = new RobotController;
		robot->sendCommandToRobot(mapTextureName[res]);
}

size_t write_to_string(void *ptr, size_t size, size_t count, void *stream) {
  ((string*)stream)->append((char*)ptr, 0, size*count);
  return size*count;
}

int getOnlineParameters() {

	char* configuration=new char[4];
	// Leggo l'orario dal sistema
	time_t currentTime;
	struct tm *localTime;

	time(&currentTime); // Get the current time
	localTime = localtime(&currentTime);
	int Hour = localTime->tm_hour;
	if (Hour>=8 && Hour<12)
		configuration[0]='0';
	else if (Hour>=12 && Hour<16)
		configuration[0]='1';
	else if (Hour>=16 && Hour<20)
		configuration[0]='2';
	else
		configuration[0]='3';
	
	string response="";
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://ip-api.com/json");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		res = curl_easy_perform(curl);
		// SLB check no connection
		if (res) {
			cout << endl << "Could not connect to the internet." << endl;
			cout << "Loading default symbolic interface..." << endl << endl;
			return 1;
		}
		//
		curl_easy_cleanup(curl);
		rapidjson::Document d;
		d.Parse<0>(response.c_str());
		city = d["city"].GetString();
		cout << endl << endl << "City: " << city << endl;

		if (city!="Palermo")
			city="Palermo";
	}

	string response2 = "";
	CURL *curl2;
	CURLcode res2;
	curl2 = curl_easy_init();
	if (curl2) {
		string request="http://api.openweathermap.org/data/2.5/weather?q="+city+"&appid="+API_WEATHER_KEY;
		curl_easy_setopt(curl2, CURLOPT_URL,request.c_str());
		curl_easy_setopt(curl2, CURLOPT_WRITEFUNCTION, write_to_string);
		curl_easy_setopt(curl2, CURLOPT_WRITEDATA, &response2);
		
		res2 = curl_easy_perform(curl2);
		// SLB check no connection
		if (res) {
			cout << endl << "Could not connect to the internet." << endl;
			cout << "Loading default symbolic interface..." << endl << endl;
			return 1;
		}
		//
		curl_easy_cleanup(curl2);
		rapidjson::Document doc;
		doc.Parse<0>(response2.c_str());
		const rapidjson::Value& tempo = doc["main"];
		const rapidjson::Value& vento = doc["wind"];
		const rapidjson::Value& cielo = doc["weather"][0u];
		temperature = tempo["temp"].GetDouble()-273,15;
		cout << "Temperature: " << temperature << endl;
		if (temperature > 15) // fa caldo
			configuration[1]='4';
		else
			configuration[1]='5';

		description = cielo["main"].GetString();
		cout << "Weather status: " << description << endl;
		//verifichiamo il cielo
		if (description == "Rain")
			configuration[2]='6';
		else
			configuration[2]='7';
		
		configuration[3] = '\0';
		wind = vento["speed"].GetDouble();
		cout << "Wind: " << wind << endl;
		string cnf(configuration);
		cout << "Configuration found: " << cnf << endl << endl << endl;
		//cout << tempo.GetString() << endl;
		//cout << tempo["description"].GetString() << endl;
		DecisionTree *d = new DecisionTree;
		int r = d->getConfiguration(cnf);
		
		return r;

	}

	// SLB delete[]
	//delete[] configuration;
		
}
// ----------------------------------------------------------------------------
// --------------------------- Fine funzioni per la socket --------------------
// ----------------------------------------------------------------------------


int main(int argc, char** argv) {

	srand(time(NULL)); // SLB is this really necessary?

	//Questo codice serve per creare una socket server
	DWORD myThreadID;
	HANDLE myHandle = CreateThread(0, 0, createSocket, NULL, 0, &myThreadID);

	while (!startFramework) {}

	initializeAll();

	pausa = false; // SLB moved here

	// GLUT Window Initialization:
	glutInit(&argc, argv);
	glutInitWindowSize(g_Width, g_Height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	window = glutCreateWindow(g_Title); // SLB int moved to global
	glutPopWindow();

	// Initialize OpenGL graphics state
	/*
	glEnable(GL_DEPTH_TEST); // Abilita il controllo sullo z-buffer
	glDepthFunc(GL_LEQUAL);  // Ordina sullo z-buffer dal pi� lontano al pi� vicino
	glShadeModel(GL_FLAT);   // Usa il modello dolce per gli shader
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	*/

	// Render the scene
	glutDisplayFunc(display);	// Setta la callback per il display
	glutReshapeFunc(reshape);	// Setta la callback per il reshape, chiamato a primo lancio della finestra

	// printf("Press 'm' for flashing squares\n");
	// printf("Press 'n' for flashing rows and columns\n");
	printf("Press 's' to start\n");	// SLB
	printf("Press 'x' for exit\n");

	//glutMouseFunc(MouseButton);
	//glutMotionFunc(MouseMotion);
	// SLB
	glutMouseFunc(NULL);
	glutMotionFunc(NULL);
	glutPassiveMotionFunc(NULL);

	glutKeyboardFunc(Keyboard);

	// Turn the flow of control over to GLUT 

	// SLB redundant?
	if (rowsNumber == SPELLER_ON) {
		rowsNumber = SPELLER_OUT;
		spellerHandler = true;
	}
	//

	glutMainLoop();

	deleteAll();
	//system("PAUSE");

	return 0;
}

