#include "Instance.h"
#include <sstream>


Instance::Instance(std::string line, int id)
{
	std::string LABEL_SYM[]={"-","+","*","/","!","?"}; //Label presenti

	this->uniqueId = id;
	label=-1;
	std::vector<std::string> temp =split(line,' ');
	//verifichiamo la label di appartenenza
	
	for (int i=0; i<LABEL_NUM; i++) {
		if (temp[1]==(LABEL_SYM[i]))
			label=i;
	}
	const char * charfts  = temp[0].c_str();
	this->fts = new int[temp[0].size()];

	for (int i=0; i<PARAM_NUM; i++) {
		std::stringstream strValue;
		strValue << charfts[i];
		strValue >> fts[i];
	}

	// SLB delete[]
	//delete[] charfts;
}

Instance::Instance(std::string line, int id,int indexLabel)
{
	std::string LABEL_SYM[]={"-","+","*","/"}; //Label presenti
	this->uniqueId = id;

	this->label=indexLabel;

	const char * charfts  = line.c_str();
	this->fts = new int[line.size()];

	for (int i=0; i<PARAM_NUM; i++) {
		std::stringstream strValue;
		strValue << charfts[i];
		strValue >> fts[i];
	}

	// SLB delete[]
	//delete[] charfts;
}

int Instance::getLabel() {
	return label;
}

int Instance::getLabelNum() {
	return LABEL_NUM;
}

int Instance::getParamNum() {
	return PARAM_NUM;
}

int* Instance::getFeatures() {
	return fts;
}

std::vector<std::string>& Instance::split(const std::string &s, char delim, std::vector<std::string>& elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> Instance::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

Instance::~Instance()
{
    //dtor
}
