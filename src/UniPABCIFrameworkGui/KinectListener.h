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

#ifndef KINECTLISTENER_H
#define KINECTLISTENER_H

#include <afxmt.h>
//#include <afxwin.h>

#define GAMEK_ROCK			3
#define GAMEK_PAPER			1
#define GAMEK_SCISSORS		2
#define GAMEK_READY			9
#define GAMEK_DISCONNECTED	8
#define GAMEK_MINMOVES		10


class KinectListener {

	public:
		KinectListener();
		void startAcquisition();
		void stopAcquisition();
		static int bufferToInt(char*);
		static UINT ListenNetwork(LPVOID);
		int getMove();
		bool isStartedKinect();
		void setStartedKinect(bool);
		CEvent& getMoveReceivedEvent();
		CEvent& getIsRockEvent();
		CEvent& getIsPaperEvent();
		CEvent& getIsScissorsEvent();

	private:
		static bool isListening;
		static bool _startedKinect;
		static int gameMove;
		static CEvent _moveReceived;
		static CEvent _isRock;
		static CEvent _isPaper;
		static CEvent _isScissors;

};


#endif