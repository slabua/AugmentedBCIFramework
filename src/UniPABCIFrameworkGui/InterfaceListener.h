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

#ifndef INTERFACELISTENER_H
#define INTERFACELISTENER_H

#include <afxmt.h>

#define START_CALIBRATION 100
#define END_CALIBRATION -100
#define START_STIMULUS 200
#define END_STIMULUS -200
#define FLASH_RC 300
#define FLASH_S 400


class InterfaceListener {
	public:
		InterfaceListener();
		void startAcquisition();
		static int bufferToInt(char*);
		static UINT ListenNetwork(LPVOID);
		int getSymbolTag();
		void stopAcquisition();
		bool isStartedInterface();
		void setStartedInterface(bool);
		void setEndedStimulus();
		bool getEndedStimulus();
		CEvent& getSymbolTagEvent();
		CEvent& getStartedCalibrationEvent();
		CEvent& getCallNextStimulus();
		CEvent& getFinishCalibrationEvent();
	private:
		static bool endedStimulus;
		static bool is_listening;
		static bool _startedOnlineP300Interface;
		static int symbolTag;
		bool waitForParameters;
		static CEvent _switchSimbolTag;
		static CEvent _startedCalibration;
		static CEvent _callNextStimulus;
		static CEvent _finishCalibration;
};

#endif

