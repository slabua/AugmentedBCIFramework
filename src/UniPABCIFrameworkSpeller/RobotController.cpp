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