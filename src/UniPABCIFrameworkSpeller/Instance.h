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

#ifndef ISTANCES_H
#define ISTANCES_H

#include <string>
#include <vector>

class Instance
{
    public:
        Instance(std::string,int);
		//Costruttore per il testing
		Instance(std::string,int,int);
        virtual ~Instance();
        std::vector<std::string> split(const std::string &s, char delim);
        std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
        int getLabel();
        int* getFeatures();
        int getParamNum();
        int getLabelNum();

    const static int PARAM_NUM=3;//numero di parametri di ogni caratteristica
	const static int LABEL_NUM=6;//Numero di comportamenti riconoscibli
	static std::string LABEL_SYM[];
	const static int FTSVALUERANGE =8; // range dei valori di inputr
	int label; //label riconosciuta
	int* fts; //parametri di feature
	int uniqueId;// every instance will have an uniqe Id;
};

#endif // ISTANCES_H

