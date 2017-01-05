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
