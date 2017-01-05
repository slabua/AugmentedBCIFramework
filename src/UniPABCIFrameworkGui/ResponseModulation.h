#pragma once

#include <fstream>
#include <vector>

#define ACTIVATION_THRESHOLD 0.25
// Threshold value has been choosen by observing a valid value
// computed during the preliminary experiments


class ResponseModulation
{
public:
	ResponseModulation(void);
	virtual ~ResponseModulation(void);

	virtual void readConfiguration();
	virtual void updateConfiguration();
	virtual bool isCalibration();
	virtual std::string getResult();
	virtual void writeResult();
	virtual void thresholdModule();
	virtual void modulationModule();
	virtual void Process();

private:
	bool bioHybridMode;
	int flash_analysis;
	int selectedID;
	std::vector <int> tobiiresultIDs;
	float stimuli_entropy, epy_min, epy_max, epy_cum, epy_norm;
	float stimuli_energy, egy_min, egy_max, egy_cum, egy_norm;
	//float rsq, rsq_cum, rsq_gap, rsq_min, rsq_max, rsq_norm;
	float rsq, rsq_cum, rsq_min, rsq_max, rsq_norm;
	bool activation_threshold;
	float behaviour_intensity;

	float flash_precision;
	float tobiiresultPC;
	std::string username;
	std::string binfilename;

	std::string pathToResults;
	std::ofstream resultsFile;

};

