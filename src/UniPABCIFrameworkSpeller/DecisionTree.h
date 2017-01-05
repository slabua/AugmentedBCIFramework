#include <string>
#include "Instance.h"

using std::string;
using std::vector;

//const string URL="http://bcip300framework.altervista.org/gettraining.php";


class DecisionTree {

	public:
		vector<string> getTrainingData();
		vector<string> getTrainingDataFile(); // SLB
		int getConfiguration(string);
		static void load(string, string, vector<Instance>&,  vector<Instance>&);
		static void load(vector<string>, string, vector<Instance>&,  vector<Instance>&);
		static size_t write_to_string(void*, size_t, size_t, void*);

};