#ifndef INCLUDED_TobiiTracker_H  // makes sure this header is not included more than once
#define INCLUDED_TobiiTracker_H

#include <Windows.h>
#include <vector>
#include <fstream>

//using namespace std; // SLB do not add, it conflicts with other classes.


class TobiiTracker
{
	
	public:
		//TobiiTracker();
		TobiiTracker(int, int, int);
		virtual ~TobiiTracker();
		virtual void Preflight();
		virtual bool Initialize();
		virtual void Process(int, int, int);
		virtual void readTrackingFile();							// SLB
		virtual void initialiseCoordinateFile();					// SLB
		virtual void initialiseVariables();							// SLB
		virtual void initialiseVectors();							// SLB
		virtual void readConfiguration();							// SLB
		virtual void updateConfiguration();							// SLB
		virtual std::pair <int, int> getScreenSize();				// SLB
		virtual RECT getWindowInfo();								// SLB
		virtual void squareBounds(int, int, int);					// SLB
		virtual void innerOuterPoints(std::pair <float, float>);	// SLB
		virtual void sortingIDs();									// SLB
		virtual void innerOuterFocussedID(int, int);					// SLB
		virtual void focusIDinfo();									// SLB
		virtual void writeBioFile();								// SLB
		virtual void printStats();									// SLB
		virtual void Disable();										// SLB
		virtual void setRunning(bool);								// SLB
		//virtual std::pair <float, float> getEventParams();		// SLB
		virtual void screenToWindowCoordinates();					// SLB
		virtual void showGaze(int, int);							// SLB
		virtual void showInnerSquare(int, int, int, int);			// SLB
		
	// Use this space to declare any TobiiTracker-specific methods and member variables you'll need
	
	private:
		bool tobiienabled;
		bool tobiiwindowcoord;
		std::string username;
		int rows;

		std::ifstream infile;
		std::ofstream expectedFile;

		RECT windowInfo;
		RECT windowActiveArea;
		std::pair <int, int> iconsOrigin;

		struct array_data_s {
			int id;
			int value;
			int inner;
			int outer;
			bool operator<(const array_data_s& a) const {
				return value < a.value;
			}
		} data_struct;
		std::vector <array_data_s> array_data_structs;
		
		int array_data[6][6];
		int array_data_innerSquare[6][6];
		int array_data_outerSquare[6][6];

		int text_pad;
		int x_icon_size;
		int y_icon_size;
		int x_innerSquareOffset;
		int y_innerSquareOffset;

		std::vector <std::vector <int>> bounds;
		std::vector <std::vector <std::pair <int, int>>> centers;
		std::vector <std::pair <float, float>> v;
		std::vector <std::pair <float, float>> centralCoordinates;
		std::vector <std::pair <float, float>> lateralCoordinates;
		std::vector <std::pair <float, float>> outerCoordinates;

		int selected_IDs[36];
		int selected_values[36];
		int array_inner[36];
		int array_outer[36];

		std::string pathToEyeTracking;
		std::string pathToBiometrics;

		std::string selected_IDs_string;
		int count_tot_valid_Points;
		int totEyetrackingPoints;

		int id_max;
		int max_value;
		int max_inner;
		int max_outer;
		int i_max;
		int j_max;
		float focusPercent;
		float centralFocus;
		float lateralFocus;

		//static const int innerSquarePercentage = 25;
		int innerSquarePercentage;

};

#endif // INCLUDED_TobiiTracker_H
