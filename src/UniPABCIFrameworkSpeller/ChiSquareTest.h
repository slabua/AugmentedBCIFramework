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


#ifndef CHISQUARETEST_H
#define CHISQUARETEST_H

#include <vector>


/*ChiSquare si occupa di effettuare la sogliatura dell'albero per ridurre il problema del sovraddatamento della rete, le
soglie ottimali sono:(dal piu alto al piu basso)
/**
	 * Chi-square test with threshold .001.
			* ChiSquareTest(16.27);

	 * Chi-square test with threshold .01.
			ChiSquareTest(11.34);

	 * Chi-square test with threshold .5
	         ChiSquareTest(7.82)
*/
class ChiSquareTest
{
    public:
        ChiSquareTest(double threshold);
        virtual ~ChiSquareTest();
        double Get() { return threshold; }
        void Set(double val) { threshold = val; }
       bool test(std::vector<std::vector <int> > count,int x,int y);
		// bool test(std::vector<std::vector <int> > count,int x,int y);
    protected:
    private:
        double threshold;
};

#endif // CHISQUARETEST_H
