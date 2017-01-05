#pragma once

#include <vector>

class ActionSelection
{
public:
	ActionSelection(void);
	virtual ~ActionSelection(void);

	virtual void readConfiguration(void);
	virtual void updateConfiguration(void);
	virtual void weighingModule(void);
	virtual void idSelectionModule(void);
	virtual void Process(void);

private:
	float skillslidervalue;
	bool tobiienabled;
	int tobiiresultID;
	float tobiiresultPC;
	int flash_analysis;
	float flash_precision;

	std::pair <int, int> results;
	std::pair <float, float> precisions;

	int ID;
};

