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

#ifndef ID3_H
#define ID3_H

#include "Node.h"
#include "Instance.h"
#include "ChiSquareTest.h"
#include <vector>

using std::vector;


class ID3 {

    public:
        ID3();
        virtual ~ID3();
        static double impurity_entropy(int* element,int size);
        vector<int> classify(vector<Instance> testInstances);
        static void expand(Node* node, int depth);
        static Node* generate(vector<Instance> instances);
        static double computeAccuracy(vector<int> predictions,vector<Instance> testInstances);
        static ChiSquareTest chi_square_100() {return ChiSquareTest(0);};
		static ChiSquareTest chi_square_001() {return ChiSquareTest(16.27);};
		static ChiSquareTest chi_square_01()  {return ChiSquareTest(11.34);};
		static ChiSquareTest chi_square_05()  {return ChiSquareTest(7.82);};
        void learn(vector<Instance> instances);
    protected:
    private:
        	Node* root;

};

#endif // ID3_H

