#pragma once

#include <string>

using std::string;


class RobotController {
	public:
		RobotController();
		~RobotController(void);
		//void sendCommandToRobot(string);
		void sendRawCommand(string);	// SLB
		void playmotion(string);		// SLB
		void playvoice(string);			// SLB
		unsigned int sendCommandToNao(string);
	private:
		string RobotIP;
		string RobotPort;
		string naoIP;
		string naoPort;
};

