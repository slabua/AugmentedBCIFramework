#pragma once

#include <string>

using std::string;


class RobotController {
	public:
		RobotController();
		~RobotController(void);
		void sendCommandToRobot(string);
		unsigned int sendCommandToNao(string);
	private:
		string RobotIP;
		string RobotPort;
		string naoIP;
		string naoPort;
};

