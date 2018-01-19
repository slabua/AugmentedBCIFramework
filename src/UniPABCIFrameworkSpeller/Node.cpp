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

#include "Node.h"
#include <iostream>

using std::cout;


Node::Node()
{
	predictedLabel = -1;
	parent=nullptr;
	//children=nullptr;
}

Node::Node(Node* parent, std::vector<Instance>& instances) {

	this->parent = parent;
	this->children= std::vector<Node*>(Instance::FTSVALUERANGE);
	this->instances = instances;
	this->numOfFts = ((Instance) instances[0]).getParamNum();
	this->numOfLabel = ((Instance) instances[0]).getLabelNum();
	this->testFts = -1;

	int* count;
	
	count =new int[this->numOfLabel];
	for (int i=0; i<this->numOfLabel; i++)
		count[i]=0;

	for (int i=0; i<instances.size(); i++) {
		count[instances[i].getLabel()]++;
	}
	/*Devo prendere il maggiore fra gli elementi*/
	this->predictedLabel=0;
	for (int i=1; i<numOfLabel; i++) {
		if (count[i]>count[i-1])
		this->predictedLabel=i;
	}
	delete count;
}

int Node::classify(Instance t) {
	// System.out.println("test\t" + testFts);
	if (testFts == -1) {
		return predictedLabel;
	}
	else {
		bool b=(children[t.fts[testFts]])!=nullptr;
		if (b) {
			return (children[t.fts[testFts]])->classify(t);
			return 0; // SLB ?!
		}
		else {
			return t.getLabel();// cannot decide; return parent label
		}
	}
}

Node::~Node()
{
    //dtor
}
