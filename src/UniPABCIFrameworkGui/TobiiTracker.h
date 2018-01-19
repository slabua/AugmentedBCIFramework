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

#ifndef INCLUDED_TobiiTracker_H  // makes sure this header is not included more than once
#define INCLUDED_TobiiTracker_H

#include <Windows.h>
#include <vector>


class TobiiTracker
{
	
	public:
		TobiiTracker();
		virtual ~TobiiTracker();
		virtual bool Initialize();
		virtual void readConfiguration();							// SLB
		//virtual std::pair <int, int> getScreenSize();				// SLB
		virtual void Disable();										// SLB
		virtual void setRunning(bool);								// SLB
		virtual bool getUserTracked();								// SLB
		
	// Use this space to declare any TobiiTracker-specific methods and member variables you'll need
	
	private:
		bool tobiienabled;
		std::string username;

		//RECT windowInfo;
		//RECT windowActiveArea;

};

#endif // INCLUDED_TobiiTracker_H
