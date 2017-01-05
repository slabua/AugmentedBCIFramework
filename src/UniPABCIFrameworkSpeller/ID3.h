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
