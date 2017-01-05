#ifndef ISTANCES_H
#define ISTANCES_H

#include <string>
#include <vector>

class Instance
{
    public:
        Instance(std::string,int);
		//Costruttore per il testing
		Instance(std::string,int,int);
        virtual ~Instance();
        std::vector<std::string> split(const std::string &s, char delim);
        std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
        int getLabel();
        int* getFeatures();
        int getParamNum();
        int getLabelNum();

    const static int PARAM_NUM=3;//numero di parametri di ogni caratteristica
	const static int LABEL_NUM=6;//Numero di comportamenti riconoscibli
	static std::string LABEL_SYM[];
	const static int FTSVALUERANGE =8; // range dei valori di inputr
	int label; //label riconosciuta
	int* fts; //parametri di feature
	int uniqueId;// every instance will have an uniqe Id;
};

#endif // ISTANCES_H
