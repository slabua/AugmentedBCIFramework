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

#include "ActionSelection.h"
#include "rootpaths.h"
#include <iostream>
#include "pugixml.hpp"
#include <ctime>

using std::cout;
using std::endl;


ActionSelection::ActionSelection(void)
{
}


ActionSelection::~ActionSelection(void)
{
}

void ActionSelection::readConfiguration() {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str());
	
	if (result) {
		skillslidervalue	= atof(doc.child("skillslidervalue").first_child().value()) / 100;	// [0-1]
		
		flash_analysis		= atoi(doc.child("flash_analysis").first_child().value());
		flash_precision		= atof(doc.child("flash_precision").first_child().value()) / 100;	// [0-1]

		tobiienabled		= (bool) atoi(doc.child("tobiienabled").first_child().value());
		tobiiresultID		= atoi(doc.child("tobiiresultID").first_child().value());
		tobiiresultPC		= atof(doc.child("tobiiresultPC").first_child().value());			// [0-1]
		
		results.first		= flash_analysis;
		results.second		= tobiiresultID;

		precisions.first	= flash_precision;
		precisions.second	= tobiiresultPC;
	}
	else
		cout << "Could not find configuration file." << endl;

}

void ActionSelection::updateConfiguration() {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str());

	if (result) {

		pugi::xml_node node = doc.child("selectedID");
		node.first_child().set_value(std::to_string(ID).c_str());

		doc.save_file((configFilesRoot + "/configuration.xml").c_str());
	}
	else
		cout << "Could not find configuration file." << endl;

}

void ActionSelection::weighingModule() {

	if (!tobiienabled)
		skillslidervalue = 1;

	precisions.first	*= skillslidervalue;
	precisions.second	*= (1-skillslidervalue);

	float normFactor	= precisions.first + precisions.second;
	precisions.first	/= normFactor;
	precisions.second	/= normFactor;
	
	// Note: precisions.first + precisions.second is always = 1

}

void ActionSelection::idSelectionModule() {

	srand(time(NULL));
	float rnd = (float) rand() / (float) RAND_MAX;
	
	// "X = [CID, FID]; P = [CIDpc, FIDpc]; ID = X(find(rand < cumsum(P), 1, 'first'));"
	ID = (rnd < precisions.first) ? results.first : results.second;

}

void ActionSelection::Process() {

	readConfiguration();
	
	weighingModule();
	idSelectionModule();

	updateConfiguration();

}

