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

#include "ID3.h"


ID3::ID3()
{
	root=NULL;
}

Node* ID3::generate(vector<Instance> instances) {
		Node* root = new Node(nullptr, instances);
		expand(root, 0);// when use ID3.chi_square_100// there is no prunning,
		return root;
}

void ID3::expand(Node* node,int depth) {
    	double maxGain = -100000;
		int sumCount=0,sumLabel=0;
		int maxGainDecision = -1;

		/*int** mcount=new int*[Instance::FTSVALUERANGE];
        for (int i=0; i<Instance::FTSVALUERANGE; i++) {
			mcount[i] = new int[Instance::LABEL_NUM];
            mcount[i]=0;
       }*/
		vector <vector <int> > mcount;
		for (int i=0; i<Instance::FTSVALUERANGE; i++)
			mcount.push_back(vector<int>(Instance::LABEL_NUM));

		int* label=new int[Instance::LABEL_NUM];
		for (int i=0; i<Instance::LABEL_NUM; i++)
			label[i]=0;

		for (int i=0; i<node->instances.size(); i++) {
				label[node->instances[i].label]++;
		}

		int count[Instance::FTSVALUERANGE][Instance::LABEL_NUM]={0};
		/* Iterate over all attributes, find the best attribute */
		for (int s = 0; s < node->numOfFts; ++s) {

			//Setto la matrice tutta a 0
				memset(count, 0, sizeof(count));

            for (int j=0;j<node->instances.size();j++) {
                    Instance tmp=node->instances[j];
				count[tmp.getFeatures()[s]][tmp.getLabel()]++;
			}
			double gain = impurity_entropy(label,Instance::LABEL_NUM);

			for (int i=0; i<Instance::FTSVALUERANGE; i++) {
				//azzero le somme sulle diverse occorenze
				sumCount=sumLabel=0;

				for (int j=0;j<Instance::LABEL_NUM;j++) {
					sumCount+=count[i][j];
					sumLabel+=label[j];
				}
				gain -= 1.0* sumCount/sumLabel* impurity_entropy(count[i],Instance::LABEL_NUM);


			}

			if (gain > maxGain) {
				maxGain = gain;
				maxGainDecision = s;
				for (int i=0; i<Instance::FTSVALUERANGE; i++) {
					for (int j=0; j< Instance::LABEL_NUM;j++)
						mcount[i][j] = count[i][j];
				}
			}

		}
		/*le possibili sogliature sono:
	    static ChiSquareTest chi_square_100() {return ChiSquareTest(0);};
		static ChiSquareTest chi_square_001() {return ChiSquareTest(16.27);};
		static ChiSquareTest chi_square_01()  {return ChiSquareTest(11.34);};
		static ChiSquareTest chi_square_05()  {return ChiSquareTest(7.82);};

		/** If maxGain ==0 then the node is pure, stop growing the tree */
		if (maxGain > 1e-10 && chi_square_100().test(mcount,Instance::FTSVALUERANGE,Instance::LABEL_NUM)) {
			node->testFts = maxGainDecision;

			vector< vector<Instance> > ts;
			for (int i=0; i<Instance::FTSVALUERANGE; i++) {
				ts.push_back(vector<Instance>());
			}

			for (int i=0; i< node->instances.size(); i++) {
                    Instance tmp=node->instances[i];
                    int index=tmp.getFeatures()[maxGainDecision];
                    ts[index].push_back(tmp);
            }
			/* Grow the tree recursively */
			for (int i=0; i<Instance::FTSVALUERANGE; i++) {
				if (ts[i].size() > 0) {
					node->children[i] = new Node(node, ts[i]);
					expand(node->children[i], depth + 1);
				}
			}
		}
	}

void ID3::learn(vector<Instance> instances) {
		this->root = generate(instances);
}

double ID3::impurity_entropy(int* element,int size) {
	double res=0;
	int sum=0;
	double* prob=new double[size];
	
	for (int i=0; i<size; i++)
		sum+=element[i];
	
	for (int i=0; i<size; i++) {
		prob[i]=element[i]/((double) sum);

		if (prob[i]>0)
			res += (prob[i] * (log(prob[i]) / log(2)));

	}
	
	return -res;
	delete prob;
}

vector<int> ID3::classify(vector<Instance> testInstances) {
	vector<int> predictions;
	for (int i=0; i<testInstances.size(); i++) {
		int predictedCategory = root->classify(testInstances[i]);
			
		predictions.push_back(predictedCategory);
	}

	return predictions;
}

double ID3::computeAccuracy(vector<int> predictions,vector<Instance> testInstances)
{
    if (predictions.size() != testInstances.size()) {
		return 0;
	}
	else {
		int right = 0, wrong = 0;
		for (int i=0; i<predictions.size(); i++) {
			 if (predictions[i] == ((Instance)testInstances[i]).getLabel()) 
				right++;
			 else 
				wrong++;
		}
		return right * 1.0 / (right + wrong);
	}
}

ID3::~ID3()
{
    //dtor
}
