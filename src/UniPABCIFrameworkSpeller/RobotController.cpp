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

#include "RobotController.h"
#include "clientTCP.h"
#include "pugixml.hpp"
#include "rootpaths.h" // SLB


RobotController::RobotController() {
	pugi::xml_document doc;

	//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB

	if (result)
	{
		RobotIP		= doc.child("iprobot").first_child().value();
		RobotPort	= doc.child("portrobot").first_child().value();
		naoPort		= doc.child("ipnao").first_child().value();
		naoIP		= doc.child("portnao").first_child().value();
	}
}

RobotController::~RobotController(void) {}

//telenoid: 192.168.1.71:11000
//playmotion angry.txt
void RobotController::sendCommandToRobot(string code) {
	clientTCP *conn = new clientTCP(RobotPort, RobotIP);
	string command = "playvoice "+code+".wav";
	conn->sendCommandToRobot(command.c_str());

}

unsigned int RobotController::sendCommandToNao(string code) {

	string sendingAdress = "";
	string comando = sendingAdress+" "+naoIP+" " + code;
	printf("Connection to NAO Opened....\t");
	clientTCP* conn  = new clientTCP(naoIP, naoPort);
	conn->sendCommand(comando.c_str());
	delete conn;
	printf("Command sent\n");

	return 0;
}