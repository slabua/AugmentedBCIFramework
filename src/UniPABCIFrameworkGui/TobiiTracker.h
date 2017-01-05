#ifndef INCLUDED_TobiiTracker_H  // makes sure this header is not included more than once
#define INCLUDED_TobiiTracker_H

#include <Windows.h>
#include <vector>


class TobiiTracker
{
	
	public:
		TobiiTracker();
		virtual ~TobiiTracker();
		virtual bool Initialize();
		virtual void readConfiguration();							// SLB
		//virtual std::pair <int, int> getScreenSize();				// SLB
		virtual void Disable();										// SLB
		virtual void setRunning(bool);								// SLB
		virtual bool getUserTracked();								// SLB
		
	// Use this space to declare any TobiiTracker-specific methods and member variables you'll need
	
	private:
		bool tobiienabled;
		std::string username;

		//RECT windowInfo;
		//RECT windowActiveArea;

};

#endif // INCLUDED_TobiiTracker_H
