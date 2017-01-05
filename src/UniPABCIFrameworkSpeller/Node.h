#ifndef NODE_H
#define NODE_H

#include <vector>
#include "Instance.h"


class Node
{
    public:
		Node();
        Node(Node* parent, std::vector<Instance>& instances);
		Node* parent;
        std::vector<Node*> children;
		int classify(Instance t);
        virtual ~Node();


	/**
	 * The test feature for internal node
	 */
	int testFts;
	int numOfLabel;
	int numOfFts;

	std::vector<Instance> instances;

	int predictedLabel;

};

#endif // NODE_H
