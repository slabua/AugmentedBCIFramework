#include "DecisionTree.h"
#include <iostream>
#include <curl/curl.h>
#include "document.h"
#include <algorithm>
#include <fstream>
#include "ID3.h"
#include "rootpaths.h" // SLB

using namespace rapidjson;
using std::cout;


vector<string> DecisionTree::getTrainingData() {
	CURL *curl;
	CURLcode res;
	string response = "";
	char t;
	string tmp;
	vector<string> training;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://bcip300framework.altervista.org/gettraining.php");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		rapidjson::Document d;
		d.Parse<0>(response.c_str());
		//cout<< endl << endl <<d[0u].GetString()<<response<<endl;
		for (Value::ConstValueIterator itr = d.Begin(); itr != d.End(); ++itr) {
			tmp=itr->GetString();
			tmp.erase(std::remove(tmp.begin(), tmp.end(), ','), tmp.end());
			t=tmp[3];
			tmp[3]=' ';
			tmp.push_back(t);
			training.push_back(tmp);
		}

	}
	return training;
}

// SLB
vector<string> DecisionTree::getTrainingDataFile() {
	
	string trainElement;
	vector<string> train;
	std::ifstream trainfile;
	trainfile.open((configFilesRoot + "/training.txt").c_str());
	//while (!trainfile.eof()) {
	while (getline(trainfile, trainElement)) {
		//getline(trainfile, trainElement);
		train.push_back(trainElement);
	}
	trainfile.close();
	
	return train;
}

void DecisionTree::load(vector<string> train, std::string test, vector<Instance>& trainInstances,  vector<Instance>& testInstances) {
	int UNIQEID = 0;
	for (int i=0; i<train.size(); i++) {
		Instance ins =  Instance(train[i], UNIQEID++);
		trainInstances.push_back(ins);
	}
	try {
		for (int i=0; i<Instance::LABEL_NUM; i++) {
			Instance ins =  Instance(test, UNIQEID++,i);
			testInstances.push_back(ins);
		}
	} catch (int e) {
		std::cout << "An exception occurred. Exception Nr. " << e << '\n';
	}
}

void DecisionTree::load(std::string trainfile, std::string test, vector<Instance>& trainInstances,  vector<Instance>& testInstances) {
	int UNIQEID = 0;
	try {
		std::string line;
		std::ifstream classify(trainfile);
		while (getline(classify,line)) {
			Instance ins = Instance(line, UNIQEID++);
			trainInstances.push_back(ins);
		}
	}  catch (int e) {
		std::cout << "An exception occurred. Exception Nr. " << e << '\n';
  	}
	try {
		for (int i=0; i<Instance::LABEL_NUM; i++) {
			Instance ins =  Instance(test, UNIQEID++,i);
			testInstances.push_back(ins);
		}
	} catch (int e) {
		std::cout << "An exception occurred. Exception Nr. " << e << '\n';
	}
}

int DecisionTree::getConfiguration(string param) {
	//questa stringa arriva direttamente dal sistema;
	vector<Instance> testInstances;
	vector<Instance> trainInstances;
	
	///vector<string> train = getTrainingData();	// SLB disable using altervista website
	vector<string> train = getTrainingDataFile();	// SLB

	this->load(train, param, trainInstances, testInstances);
		
	ID3* id3 = new ID3();
	id3->learn(trainInstances); //by default ID3: no pruning; information gain;
	vector<int> trainpredictions = id3->classify(trainInstances);
	//cout<<"ID3 with full tree on training\t"<<id3->computeAccuracy(trainpredictions, trainInstances)<<endl;
	
	vector<int> predictions = id3->classify(testInstances);

	//cout<<"ID3 with full tree on test\t"<< id3->computeAccuracy(predictions, testInstances);
			
	//cout << predictions[0] << endl;

	return (predictions[0])+1;
}

size_t DecisionTree::write_to_string(void *ptr, size_t size, size_t count, void *stream) {
	((string*)stream)->append((char*)ptr, 0, size*count);
	return size*count;
}