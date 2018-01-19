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

#pragma once

#include <vector>

class ActionSelection
{
public:
	ActionSelection(void);
	virtual ~ActionSelection(void);

	virtual void readConfiguration(void);
	virtual void updateConfiguration(void);
	virtual void weighingModule(void);
	virtual void idSelectionModule(void);
	virtual void Process(void);

private:
	float skillslidervalue;
	bool tobiienabled;
	int tobiiresultID;
	float tobiiresultPC;
	int flash_analysis;
	float flash_precision;

	std::pair <int, int> results;
	std::pair <float, float> precisions;

	int ID;
};

