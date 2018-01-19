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

#include "TobiiTracker.h"
#include <iostream>
#include <assert.h>
#include <algorithm>
#include "rootpaths.h" // SLB
#include "EyeX.h"
#include "glut.h"
#include "pugixml.hpp"
#include <deque>


// ID of the global interactor that provides our data stream; must be unique within the application.
static const TX_STRING InteractorId = "Twilight Sparkle";

// Global variables
static TX_HANDLE g_hGlobalInteractorSnapshot = TX_EMPTY_HANDLE;
static TX_CONTEXTHANDLE hContext = TX_EMPTY_HANDLE;

//TX_GAZEPOINTDATAEVENTPARAMS eventParams; // SLB

#pragma comment (lib, "Tobii.EyeX.Client.lib")

using namespace std;

// SLB
bool is_running = false;

bool tobiiconnected = false;
bool tobiigazeon;
bool tobiifiltered;
bool tobiishowinnersquare;

deque <pair <float, float>> points;
///pair <int, int> screenBounds;
TX_SIZE2 screenBounds;

const char* pathToEyeTrackingFile;
const char* pathToBiometricsFile;


TobiiTracker::TobiiTracker(int width, int height, int textheight)
	: tobiienabled(false)
{
}

TobiiTracker::~TobiiTracker()
{
	Disable(); // SLB
}

// SLB Included in Initialize()
void TobiiTracker::Preflight() {

	///screenBounds = getScreenSize();

	pathToEyeTracking		= outputFilesRoot + "/eyetrackingpoints.txt"; // SLB TODO to delete afterwards
	pathToEyeTrackingFile	= pathToEyeTracking.c_str();
	
	pathToBiometrics		= outputFilesRoot + "/" + username + "_biometrics.txt";
	pathToBiometricsFile	= pathToBiometrics.c_str();

	initialiseCoordinateFile();

}

void TX_CALLCONVENTION OnStateReceived(TX_HANDLE hStateBag) {
//void OnStateReceived(TX_HANDLE hStateBag) {
	
	TX_BOOL success;
	TX_INTEGER eyeTrackingState;
	TX_SIZE2 displaySize;
	///TX_SIZE2 screenBounds;
	TX_INTEGER presenceData;
	TX_INTEGER gazeTracking;

	///if (is_running) {
		success = (txGetStateValueAsInteger(hStateBag, TX_STATEPATH_EYETRACKINGSTATE, &eyeTrackingState) == TX_RESULT_OK);
		if (success) {
			switch (eyeTrackingState) {
				case TX_EYETRACKINGDEVICESTATUS_TRACKING:
					cout << "Eye-Tracking Device Status: 'TRACKING'.\n"
							"That means that the eye tracker is up and running and trying to track your eyes.\n";
					// SLB
					tobiiconnected = true;
					break;

				default:
					cout << "The eye-tracking device is not tracking.\n"
							"It could be a that the eye tracker is not connected, or that a screen setup or\n"
							"user calibration is missing. The status code is " << eyeTrackingState << "." << endl;
					// SLB
					tobiiconnected = false;
					break;
			}
		}
		
		success = (txGetStateValueAsSize2(hStateBag, TX_STATEPATH_DISPLAYSIZE, &displaySize) == TX_RESULT_OK);
		if (success) {
			printf("Display Size: %5.2f x %5.2f mm\n", displaySize.Width, displaySize.Height);
		}

		success = (txGetStateValueAsSize2(hStateBag, TX_STATEPATH_SCREENBOUNDS, &screenBounds) == TX_RESULT_OK);
		///success = (txGetStateValueAsSize2(hStateBag, TX_STATEPATH_SCREENBOUNDS, &screenDim) == TX_RESULT_OK);
		if (success) {
			printf("Screen Bounds: %5.0f x %5.0f pixels\n\n", screenBounds.Width, screenBounds.Height);
			///printf("Screen Bounds: %5.0f x %5.0f pixels\n\n", screenDim.Width, screenDim.Height);
		}

		/*
		success = (txGetStateValueAsInteger(hStateBag, TX_STATEPATH_USERPRESENCE, &presenceData) == TX_RESULT_OK);
		if (success) {
			if (presenceData != TX_USERPRESENCE_UNKNOWN) {
				printf("User is %s\n", presenceData == TX_USERPRESENCE_PRESENT ? "present" : "NOT present");
			}
		}
		*/
		// The following state requires EyeX Engine 1.4.0 or later:
		/*
		success = (txGetStateValueAsInteger(hStateBag, TX_STATEPATH_GAZETRACKING, &gazeTracking) == TX_RESULT_OK);
		if (success) {
			printf("User's eye-gaze is %s\n", gazeTracking == TX_GAZETRACKING_GAZETRACKED ? "tracked" : "NOT tracked");
		}
		*/
		
	///}
}

/*
 * Handles engine state change notifications.
 */
void TX_CALLCONVENTION OnEngineStateChanged(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam) {
	TX_RESULT result = TX_RESULT_UNKNOWN;
	TX_HANDLE hStateBag = TX_EMPTY_HANDLE;

	if (txGetAsyncDataResultCode(hAsyncData, &result) == TX_RESULT_OK &&
		txGetAsyncDataContent(hAsyncData, &hStateBag) == TX_RESULT_OK) {

			OnStateReceived(hStateBag);
			txReleaseObject(&hStateBag);
	}
}

/*
 * Initializes g_hGlobalInteractorSnapshot with an interactor that has the Gaze Point behavior.
 */
BOOL InitializeGlobalInteractorSnapshot(TX_CONTEXTHANDLE hContext) {
	BOOL success;
	TX_HANDLE hInteractor = TX_EMPTY_HANDLE;
	
	if (tobiifiltered) {
		TX_GAZEPOINTDATAPARAMS params = { TX_GAZEPOINTDATAMODE_LIGHTLYFILTERED }; // SLB
		success = txCreateGlobalInteractorSnapshot(hContext, InteractorId, &g_hGlobalInteractorSnapshot, &hInteractor) == TX_RESULT_OK;
		success &= txCreateGazePointDataBehavior(hInteractor, &params) == TX_RESULT_OK;

		txReleaseObject(&hInteractor);

		return success;

	}
	else {
		TX_GAZEPOINTDATAPARAMS params = { TX_GAZEPOINTDATAMODE_UNFILTERED }; // SLB
		success = txCreateGlobalInteractorSnapshot(hContext, InteractorId, &g_hGlobalInteractorSnapshot, &hInteractor) == TX_RESULT_OK;
		success &= txCreateGazePointDataBehavior(hInteractor, &params) == TX_RESULT_OK;

		txReleaseObject(&hInteractor);

		return success;
	}
	
}

/*
 * Callback function invoked when a snapshot has been committed.
 */
void TX_CALLCONVENTION OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData, TX_USERPARAM param) {
	// check the result code using an assertion.
	// this will catch validation errors and runtime errors in debug builds. in release builds it won't do anything.

	TX_RESULT result = TX_RESULT_UNKNOWN;
	txGetAsyncDataResultCode(hAsyncData, &result);
	assert(result == TX_RESULT_OK || result == TX_RESULT_CANCELLED);
}

/*
 * Callback function invoked when the status of the connection to the EyeX Engine has changed.
 */
void TX_CALLCONVENTION OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam) {
	switch (connectionState) {
		case TX_CONNECTIONSTATE_CONNECTED:
			{
				BOOL success;
				cout << "The connection state is now CONNECTED (We are connected to the EyeX Engine)\n";
				// commit the snapshot with the global interactor as soon as the connection to the engine is established.
				// (it cannot be done earlier because committing means "send to the engine".)
				success = txCommitSnapshotAsync(g_hGlobalInteractorSnapshot, OnSnapshotCommitted, NULL) == TX_RESULT_OK;

				txGetStateAsync(hContext, TX_STATEPATH_EYETRACKING, OnEngineStateChanged, NULL);
			
				if (!success) {
					cout<<"Failed to initialize the data stream.\n";
				}
				else {
					cout<<"Waiting for gaze data to start streaming...\n";
				}
				
			}
			break;

		case TX_CONNECTIONSTATE_DISCONNECTED:
			cout<<"The connection state is now DISCONNECTED (We are disconnected from the EyeX Engine)\n";
			break;

		case TX_CONNECTIONSTATE_TRYINGTOCONNECT:
			cout<<"The connection state is now TRYINGTOCONNECT (We are trying to connect to the EyeX Engine)\n";
			break;

		case TX_CONNECTIONSTATE_SERVERVERSIONTOOLOW:
			cout<<"The connection state is now SERVER_VERSION_TOO_LOW: this application requires a more recent version of the EyeX Engine to run.\n";
			break;

		case TX_CONNECTIONSTATE_SERVERVERSIONTOOHIGH:
			cout<<"The connection state is now SERVER_VERSION_TOO_HIGH: this application requires an older version of the EyeX Engine to run.\n";
			break;
	}
}

/*
 * Handles an event from the Gaze Point data stream.
 */
void OnGazeDataEvent(TX_HANDLE hGazeDataBehavior) {
	
	if (is_running) {
	
		TX_GAZEPOINTDATAEVENTPARAMS eventParams;
		///TX_BOOL success;
		///TX_INTEGER presenceData;

		if (txGetGazePointDataEventParams(hGazeDataBehavior, &eventParams) == TX_RESULT_OK) {
			///success = (txGetStateValueAsInteger(g_hGlobalInteractorSnapshot, TX_STATEPATH_USERPRESENCE, &presenceData) == TX_RESULT_OK);
			/*///
			if (success) {
				cout << "User is %s\n", presenceData == TX_USERPRESENCE_PRESENT ? "present" : "NOT present.";
			}
			*///
			
			// SLB TODO possible optimisation: write on a vector and save afterwards
			// instead of opening/closing the file for each detected point
			FILE *f;
			f = fopen(pathToEyeTrackingFile, "a+"); // EyeTracking file append
			if (f==0) {
				cout << "Error opening file!\n";
			}
			else {
				
				///eventParams.Y = screenBounds.second - eventParams.Y; // SLB sets the screen origin to the bottom-left corner
				eventParams.Y = screenBounds.Height - eventParams.Y; // SLB sets the screen origin to the bottom-left corner
				
				fprintf(f, "%.1f\t", eventParams.X);	// SLB was space+tab
				fprintf(f, "%.1f\n", eventParams.Y);	// SLB was space+newline
				
				//cout << eventParams.X << "\t" << eventParams.Y << endl; // SLB
				
				// SLB TODO possible performance loss here.
				if (tobiigazeon) {
					/*
					if (points.size() > 4)
						points.pop_front();
					points.emplace_back((float)eventParams.X, (float)eventParams.Y);
					*/
					points.clear();
					points.emplace_back((float)eventParams.X, (float)eventParams.Y);
				}
				//points[0] = pair <float, float> ((float) eventParams.X, (float) eventParams.Y);

				///fflush(f); // SLB check open/close file stream too often?!
				fclose(f);
			}
		}
		else {
			cout << "Failed to interpret gaze data event packet.\n";
		}
	
	}
}

/*
 * Callback function invoked when an event has been received from the EyeX Engine.
 */
void TX_CALLCONVENTION HandleEvent(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam) {
//void HandleEvent(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam) {
	TX_HANDLE hEvent = TX_EMPTY_HANDLE;
	TX_HANDLE hBehavior = TX_EMPTY_HANDLE;

	txGetAsyncDataContent(hAsyncData, &hEvent);

	// NOTE. Uncomment the following line of code to view the event object. The same function can be used with any interaction object.
	//OutputDebugStringA(txDebugObject(hEvent));

	if (txGetEventBehavior(hEvent, &hBehavior, TX_BEHAVIORTYPE_GAZEPOINTDATA) == TX_RESULT_OK) {
		OnGazeDataEvent(hBehavior);
		txReleaseObject(&hBehavior);
	}

	// NOTE since this is a very simple application with a single interactor and a single data stream, 
	// our event handling code can be very simple too. A more complex application would typically have to 
	// check for multiple behaviors and route events based on interactor IDs.

	txReleaseObject(&hEvent);
}

//void TobiiTracker::Initialize() { // SLB call with the Apply button
bool TobiiTracker::Initialize() { // SLB call with the Apply button

  // The user has pressed "Set Config" and all Preflight checks have passed.
  // The signal properties can no longer be modified, but the const limitation has gone, so
  // the TobiiTracker instance itself can be modified. Allocate any memory you need, start any
  // threads, store any information you need in private member variables.

	///TX_BOOL success; // SLB forced initialization
	
	readConfiguration();

	if (tobiienabled) { // SLB from config file
		
		Preflight();

		TX_BOOL success;
		///TX_CONTEXTHANDLE hContext = TX_EMPTY_HANDLE; // SLB
		TX_TICKET hConnectionStateChangedTicket		= TX_INVALID_TICKET;
		TX_TICKET hEventHandlerTicket				= TX_INVALID_TICKET;
		TX_TICKET hPresenceStateChangedTicket		= TX_INVALID_TICKET;
		TX_TICKET hGazeTrackingStateChangedTicket	= TX_INVALID_TICKET;
		
		// initialize and enable the context that is our link to the EyeX Engine.
		success = txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, NULL, NULL, NULL, NULL) == TX_RESULT_OK;
		success &= txCreateContext(&hContext, TX_FALSE) == TX_RESULT_OK;
		success &= InitializeGlobalInteractorSnapshot(hContext);
		success &= txRegisterConnectionStateChangedHandler(hContext, &hConnectionStateChangedTicket, OnEngineConnectionStateChanged, NULL) == TX_RESULT_OK;
		success &= txRegisterStateChangedHandler(hContext, &hPresenceStateChangedTicket, TX_STATEPATH_USERPRESENCE, OnEngineStateChanged, NULL) == TX_RESULT_OK;
		success &= txRegisterStateChangedHandler(hContext, &hGazeTrackingStateChangedTicket, TX_STATEPATH_GAZETRACKING, OnEngineStateChanged, NULL) == TX_RESULT_OK;
		success &= txRegisterEventHandler(hContext, &hEventHandlerTicket, HandleEvent, NULL) == TX_RESULT_OK;
		success &= txEnableConnection(hContext) == TX_RESULT_OK;
		
		/*SLB ///
		getch();
		Disable();
		//*///
		
	}

	return tobiiconnected;
}

// SLB
//void TobiiTracker::Disable(TX_CONTEXTHANDLE hContext) {
void TobiiTracker::Disable() {
	
	// disable and delete the context.
	BOOL success;

	txDisableConnection(hContext);
	txReleaseObject(&g_hGlobalInteractorSnapshot);
	success = txShutdownContext(hContext, TX_CLEANUPTIMEOUT_DEFAULT, TX_FALSE) == TX_RESULT_OK;
	success &= txReleaseContext(&hContext) == TX_RESULT_OK;
	success &= txUninitializeEyeX() == TX_RESULT_OK;
	if (!success) {
		printf("EyeX could not be shut down cleanly. Did you remember to release all handles?\n");
	}

	delete pathToEyeTrackingFile;
	delete pathToBiometricsFile;
}

// SLB
void TobiiTracker::readTrackingFile() {

	string tmp_x;
	string tmp_y;
	pair <float, float> tempPoint;
	totEyetrackingPoints = 0;

	infile.open(pathToEyeTrackingFile);
	while (!infile.eof()) {
		infile >> tmp_x;
		infile >> tmp_y;

		totEyetrackingPoints++;
				
		tempPoint.first		= atof(tmp_x.c_str());
		tempPoint.second	= atof(tmp_y.c_str());

		v.push_back(tempPoint);

		innerOuterPoints(tempPoint);
		/* SLB arrays available here:
		array_data[][]
		array_data_innerSquare[][]
		array_data_outerSquare[][]
		*/
	}
	infile.close();
	
	initialiseCoordinateFile();
}

// SLB
void TobiiTracker::initialiseCoordinateFile() {
 
	if (tobiienabled) {
		FILE *f;
		f = fopen(pathToEyeTrackingFile, "w+");
		if (f==0) {
			cout << "Error opening file!\n";
		}
		else {
			printf ("Eye-tracking file initialised.\n", fwrite ("", 0, 1, f));
			//cout << "Eye tracking file initialised." << endl; // SLB
			//fflush(f);
			fclose(f);
		}

	}
}

// SLB
void TobiiTracker::initialiseVariables() {

	selected_IDs_string		= "-1";
	count_tot_valid_Points	=  0;
	totEyetrackingPoints	=  0;

	id_max					= -1;
	max_value				=  0;
	max_inner				=  0;
	max_outer				=  0;
	i_max					= -1;
	j_max					= -1;
	focusPercent			=  0;
	centralFocus			=  0;
	lateralFocus			=  0;

}

// SLB
void TobiiTracker::initialiseVectors() {

	for (int r=0, el=0; r<6; r++) {
		for (int c=0; c<6; c++, el++) {

			array_data[r][c]				= 0;
			array_data_innerSquare[r][c]	= 0;
			array_data_outerSquare[r][c]	= 0;
			array_inner[el]					= 0;
			array_outer[el]					= 0;
			selected_IDs[el]				= 0;
			selected_values[el]				= 0;
			
		}
	}

	array_data_structs.clear();
	v.clear();
	centralCoordinates.clear();
	lateralCoordinates.clear();
	outerCoordinates.clear();

}

void TobiiTracker::Process(int g_Width, int g_Height, int text_height) { // SLB MAIN call with the START BUTTONS

	if (tobiienabled && tobiiconnected) {
		
		readConfiguration();

		initialiseVariables();

		initialiseVectors();

		squareBounds(g_Width, g_Height, text_height); // SLB TODO move to preflight, perhaps (performance)
		/* SLB outputs available here:
		bounds[0-1][]
		centers[][][first-second]
		*/

		readTrackingFile();
		/* SLB vectors available here:
		v[]
		array_data[][]
		array_data_innerSquare[][]
		array_data_outerSquare[][]
		*/
		
		sortingIDs();
		/* SLB vectors available here:
		array_data_structs[]
		*/
		
		focusIDinfo();
		/* SLB arrays available here:
		centralCoordinates[]
		lateralCoordinates[]
		outerCoordinates[]
		*/

		screenToWindowCoordinates();

		printStats();

		writeBioFile();

		updateConfiguration();
		
	}

}

// SLB
void TobiiTracker::setRunning(bool flag) {
	
	is_running = flag;
}

// SLB - General/MFC/Use Standard Windows Libraries
pair <int, int> TobiiTracker::getScreenSize() {

	RECT screenDimensions;
	pair <int, int> screenBounds;

	GetWindowRect(GetDesktopWindow(), &screenDimensions);

	screenBounds.first = screenDimensions.right;	// Screen Width
	screenBounds.second = screenDimensions.bottom;	// Screen Height
	
	//cout << screenBounds.first << "\t" << screenBounds.second << endl;

	return screenBounds;
}

// SLB - General/MFC/MFC Static
/*
pair <int, int> TobiiTracker::getScreenSize() {

	pair <int, int> screenBounds;

	screenBounds.first = screenDim.Width;	// Screen Width
	screenBounds.second = screenDim.Height;	// Screen Height
	
	return screenBounds;
}
*/

// SLB
RECT TobiiTracker::getWindowInfo() {

	RECT windowInfo;
	
	windowInfo.left		= glutGet((GLenum)GLUT_WINDOW_X);		// Bottom-left anchor point, x coordinate
	windowInfo.bottom	= glutGet((GLenum)GLUT_SCREEN_HEIGHT) - (glutGet((GLenum)GLUT_WINDOW_Y) + glutGet((GLenum)GLUT_WINDOW_HEIGHT));																			// Bottom-left anchor point, y coordinate
	windowInfo.right	= glutGet((GLenum)GLUT_WINDOW_WIDTH);	// Window width
	windowInfo.top		= glutGet((GLenum)GLUT_WINDOW_HEIGHT);	// Window height

	return windowInfo;
}

// SLB
void TobiiTracker::squareBounds(int g_Width, int g_Height, int text_height) {

	bounds.clear();
	centers.clear();

	windowInfo		= getWindowInfo();

	int x_offset	= windowInfo.left;
	int y_offset	= windowInfo.bottom;
	int width		= windowInfo.right;
	int height		= windowInfo.top;

	/*
	vector<RECT> bounds;
	RECT tempBound;
	for (int i=0; i<squaresNumber; i++) {
		tempBound.left		= x_offset + u_coor[i];
		tempBound.right		= x_offset + u_coor[i]+size;
		tempBound.bottom	= y_offset + v_coor[i];
		tempBound.top		= y_offset + v_coor[i]+size;
		bounds.push_back(tempBound);

		//cout << tempBound.left << " " << tempBound.right << " " << tempBound.bottom << " " << tempBound.top << endl;
	}
	*/

	//vector <vector <int>> bounds; // global
	vector <int> x_bounds;
	vector <int> y_bounds;

	//vector <vector <pair <int, int>>> centers; // global
	vector <pair <int, int>> rowcenters;
	pair <int, int> center;
	
	float x_scale	= (float) g_Width / width;
	float y_scale	= (float) g_Height / height;
	text_pad		= (float) text_height / y_scale;
	int padding		= 2*text_pad;

	windowActiveArea.left	= x_offset + text_pad;
	windowActiveArea.bottom	= y_offset + text_pad;
	windowActiveArea.right	= width - padding;
	windowActiveArea.top	= height - padding;
	
	x_icon_size = (float) (windowActiveArea.right) / rows;
	y_icon_size = (float) (windowActiveArea.top) / rows;

	int xcenter_init = windowActiveArea.left + (float)(x_icon_size/2);
	
	//cout << text_pad << " " << rows << " " << x_icon_size << endl;

	for (int i=0, xoffset=windowActiveArea.left, yoffset=windowActiveArea.bottom, ycenter=windowActiveArea.bottom+(float)(y_icon_size/2);
		i<rows+1;
		i++, xoffset+=x_icon_size, yoffset+=y_icon_size, ycenter+=y_icon_size) {
		
			x_bounds.push_back(xoffset);
			y_bounds.push_back(yoffset);

			//rowcenters.clear();
			for (int c=0, xcenter = xcenter_init; c<rows; c++, xcenter+=x_icon_size) {
				center.first = xcenter;
				center.second = ycenter;
				rowcenters.push_back(center);
			}

			//cout << xoffset << " " << rowcenters[0].first << endl;

			centers.push_back(rowcenters);
			rowcenters.clear();

	}

	bounds.push_back(x_bounds);
	bounds.push_back(y_bounds);
	centers.pop_back();
	
	bool useAreaPercentage = true; // SLB default behaviour for percentage calculation (hardcoded)
	if (useAreaPercentage) {
		// SLB innerSquarePercentage% of square area
		// this method assumes, meaningfully, only square-ratio window.
		// SLB TODO so either generalise this part or BETTER remove all other cases which considers non-square window possibilities.
		int icon_area = x_icon_size * y_icon_size;
		float innerSquareArea = (float) (icon_area*innerSquarePercentage) / 100;

		x_innerSquareOffset = (std::sqrt(innerSquareArea)) / 2; // relative to the icon center
		y_innerSquareOffset = x_innerSquareOffset; // relative to the icon center
	}
	else {
		// SLB innerSquarePercentage% of linear icon_size side
		x_innerSquareOffset = ((float) (x_icon_size*innerSquarePercentage) / 100) / 2; // relative to the icon center
		y_innerSquareOffset = ((float) (y_icon_size*innerSquarePercentage) / 100) / 2; // relative to the icon center
	}	

}

// SLB
void TobiiTracker::innerOuterPoints(pair <float, float> point) {
	
	float x_center = point.first;
	float y_center = point.second;

	for (int r=0; r<rows; r++) {
		for (int c=0; c<rows; c++) {

			if ((x_center > bounds[0][c] && x_center <= bounds[0][c+1]) &&
				(y_center > bounds[1][r] && y_center <= bounds[1][r+1])) {

				array_data[r][c]++;

				if ((x_center >  (centers[r][c].first  - x_innerSquareOffset)  &&
					 x_center <= (centers[r][c].first  + x_innerSquareOffset)) &&
					(y_center >  (centers[r][c].second - y_innerSquareOffset)  &&
					 y_center <= (centers[r][c].second + y_innerSquareOffset))) {
					
					array_data_innerSquare[r][c]++;
				}
				else {

					array_data_outerSquare[r][c]++;
				}
			}

		}
	}

}

// SLB
void TobiiTracker::sortingIDs() {
	// SLB The matrix is being generated from the bottom left corner
	// while we want the indexing to start from the top left corner
	count_tot_valid_Points = 0;
	//for (int element=1, z=0; z<rows; z++) {		// for bottom left
	for (int element=1, z=rows-1; z>=0; z--) {		// for top left
		//for (int c=0; c<rows; c++, element++) {	// for bottom left
		for (int c=0; c<rows; c++, element++) {		// for top left
			
			// SLB filling the vector containing the pairs <value, index>
			if (array_data[z][c] > 0) {
				count_tot_valid_Points	+= array_data[z][c];
				data_struct.id			= element;
				data_struct.value		= array_data[z][c];
				data_struct.inner		= array_data_innerSquare[z][c];
				data_struct.outer		= array_data_outerSquare[z][c];
				array_data_structs.push_back(data_struct);
			}
			
		}
	}

	if (array_data_structs.size() != 0) {
		// SLB descending sorting of selected IDs
		std::sort(array_data_structs.rbegin(), array_data_structs.rend());
		
		selected_IDs_string = "";
		for (int i=0; i<array_data_structs.size(); i++) {
			selected_IDs[i]		= array_data_structs[i].id;		// convenience array to store sorted IDs
			selected_values[i]	= array_data_structs[i].value;	// convenience array to store sorted values
			array_inner[i]		= array_data_structs[i].inner;	// convenience array to store sorted inner points
			array_outer[i]		= array_data_structs[i].outer;	// convenience array to store sorted outer points
			
			selected_IDs_string += std::to_string(selected_IDs[i]);
			selected_IDs_string += " ";
		}
	}

}

// SLB
void TobiiTracker::focusIDinfo() {

	if (array_data_structs.size() != 0) {
		// SLB calculating max values for the focussed icon ID only
		id_max		= array_data_structs[0].id;
		max_value	= array_data_structs[0].value;
		max_inner	= array_data_structs[0].inner;
		max_outer	= array_data_structs[0].outer;
		div_t zc	= div(id_max-1, rows);
		//i_max		= (rows - 1) - zc.quot;
		i_max		= zc.quot;
		j_max		= zc.rem;

		innerOuterFocussedID(i_max, j_max);
		/* SLB arrays available here:
		centralCoordinates[]
		lateralCoordinates[]
		outerCoordinates[]
		*/
		
		focusPercent = (float) max_value	/ (float) count_tot_valid_Points;
		centralFocus = (float) max_inner	/ (float) max_value;
		lateralFocus = (float) max_outer	/ (float) max_value;
	}

}

// SLB
void TobiiTracker::innerOuterFocussedID(int r, int c) {
	
	r = (rows-1) - r;

	for (int el=0; el<v.size(); el++) {

		float x_center = v[el].first;
		float y_center = v[el].second;

		if ((x_center >  (centers[r][c].first  - x_innerSquareOffset)  &&
			 x_center <= (centers[r][c].first  + x_innerSquareOffset)) &&
			(y_center >  (centers[r][c].second - y_innerSquareOffset)  &&
			 y_center <= (centers[r][c].second + y_innerSquareOffset))) {
			
			centralCoordinates.push_back(v[el]);
		}
		else if ((x_center > bounds[0][c] && x_center <= bounds[0][c+1]) &&
				 (y_center > bounds[1][r] && y_center <= bounds[1][r+1])) {
		
			lateralCoordinates.push_back(v[el]);
		}
		else {
			outerCoordinates.push_back(v[el]);
		}

	}

}

// SLB
void TobiiTracker::screenToWindowCoordinates() {

	if (tobiiwindowcoord) {

		cout << "Using window coordinates as per config specification." << endl;

		int x_offset = windowActiveArea.left;
		int y_offset = windowActiveArea.bottom;

		for (int i=0; i<rows; i++) {
			for (int j=0; j<rows; j++) {
				centers[i][j].first -= x_offset;
				centers[i][j].second -= y_offset;
			}
		}

		for (int i=0; i<centralCoordinates.size(); i++) {
			centralCoordinates[i].first -= x_offset;
			centralCoordinates[i].second -= y_offset;
		}

		for (int i=0; i<lateralCoordinates.size(); i++) {
			lateralCoordinates[i].first -= x_offset;
			lateralCoordinates[i].second -= y_offset;
		}

		for (int i=0; i<outerCoordinates.size(); i++) {
			outerCoordinates[i].first -= x_offset;
			outerCoordinates[i].second -= y_offset;
		}

	} else {

		cout << "Using screen coordinates as per config specification." << endl;

	}

}

// SLB
void TobiiTracker::printStats() {

	//cout << "Focussed Icon ID -> "			<< id_max << ": (" << ((i_max!=-1) ? rows-i_max : i_max) << "," << j_max << ")"							<< endl;
	cout << "Focussed Icon ID -> "			<< id_max << ": (" << ((i_max!=-1) ? i_max+1 : i_max) << "," << ((j_max!=-1) ? j_max+1 : j_max) << ")"	<< endl;
	cout << "Sorted IDs -> "				<< selected_IDs_string												<< endl;
	cout << "Matrix dimensions (rows-columns) -> "			<< "(" << rows << "," << rows << ")"				<< endl;
	cout << "Window active origin (bottom-left) -> "		<< "(" << windowActiveArea.left << "," << windowActiveArea.bottom << ")"				<< endl;
	cout << "Window active width -> "		<< windowActiveArea.right											<< endl;
	cout << "Window active height -> "		<< windowActiveArea.top												<< endl;
	cout << "Inner Square Percentage -> "	<< innerSquarePercentage											<< endl;
	cout << "Icon Focus Percentage -> "		<< 100*focusPercent << "%"											<< endl;
	cout << "Central Focus Percentage -> "	<< 100*centralFocus << "%"											<< endl;
	cout << "Lateral Focus Percentage -> "	<< 100*lateralFocus << "%"											<< endl;
	
	if (array_data_structs.size() != 0) {
		i_max = (rows-1) - i_max; //
		cout << "Coordinates of the center -> ("				<< centers[i_max][j_max].first << ","			<<
																	centers[i_max][j_max].second << ")"			<< endl;
	}

}

// SLB
void TobiiTracker::writeBioFile() {

	// SLB TODO add a conditional check whether to enable from gui the writing on file or not
	
	expectedFile.open(pathToBiometricsFile, std::ios_base::app | std::ios_base::out);
	
	//expectedFile << "Focussed Icon ID = "						<< id_max << ": (" << ((i_max!=-1) ? rows-i_max : i_max) << "," << j_max << ")"		<< endl;
	expectedFile << "Focussed Icon ID = "	<< id_max << ": (" << ((i_max!=-1) ? i_max+1 : i_max) << "," << ((j_max!=-1) ? j_max+1 : j_max) << ")"	<< endl;
	expectedFile << "Sorted IDs = "								<< selected_IDs_string							<< endl;
	expectedFile << "Matrix dimensions (rows-columns) = "		<< "(" << rows << "," << rows << ")"			<< endl;
	expectedFile << "Window active origin (bottom-left) = "		<< "(" << windowActiveArea.left << "," << windowActiveArea.bottom << ")"			<< endl;
	expectedFile << "Window active width = "					<< windowActiveArea.right						<< endl;
	expectedFile << "Window active height = "					<< windowActiveArea.top							<< endl;
	expectedFile << "Icon width = "								<< x_icon_size									<< endl;
	expectedFile << "Icon height = "							<< y_icon_size									<< endl;
	expectedFile << "Inner Square Percentage = "				<< innerSquarePercentage						<< endl;
	expectedFile << "Total of Eye-Tracking Points = "			<< totEyetrackingPoints							<< endl;
	expectedFile << "Total of Focus Points = "					<< max_value << "/" << totEyetrackingPoints		<< endl;
	expectedFile << "Total of Central Focus Points = "			<< max_inner << "/" << max_value				<< endl;
	expectedFile << "Total of Lateral Focus Points = "			<< max_outer << "/" << max_value				<< endl;
	expectedFile << "Total of External Focus Points = "			<< totEyetrackingPoints-max_value << "/" << totEyetrackingPoints					<< endl;
	
	expectedFile << "Percentage of Focus on Icon = "			<< focusPercent									<< endl;
	expectedFile << "Percentage of Central Focus on Icon = "	<< centralFocus									<< endl;
	expectedFile << "Percentage of Lateral Focus on Icon = "	<< lateralFocus									<< endl;
	
	if (array_data_structs.size() != 0) {
		i_max = (rows-1) - i_max; //
		expectedFile << "Coordinates of the center = ("			<< centers[i_max][j_max].first << ","			<<
																	centers[i_max][j_max].second << ")"			<< endl;
	}

	if (centralCoordinates.size() != 0) {
		expectedFile << "List of Central focus Eye-Tracking Coordinates:" << endl;
		for (int it=0; it < centralCoordinates.size(); it++) 
			expectedFile << "(" << centralCoordinates[it].first << "," << centralCoordinates[it].second << ")\t";
		expectedFile << endl;
	}

	if (lateralCoordinates.size() != 0) {
		expectedFile << "List of Lateral focus Eye-Tracking Coordinates:" << endl;
		for (int it=0; it < lateralCoordinates.size(); it++) 
			expectedFile << "(" << lateralCoordinates[it].first << "," << lateralCoordinates[it].second << ")\t";
		expectedFile << endl;
	}

	if (outerCoordinates.size() != 0) {
		expectedFile << "List of outer focus Eye-Tracking Coordinates:" << endl;
		for (int it=0; it < outerCoordinates.size(); it++) 
			expectedFile << "(" << outerCoordinates[it].first << "," << outerCoordinates[it].second << ")\t";
		expectedFile << endl;
	}

	expectedFile << "******************************" << endl;
	expectedFile.flush();
	expectedFile.close();

}

// SLB
/*
pair <float, float> TobiiTracker::getEventParams() {
	
	pair <float, float> point;
	point.first = eventParams.X;
	point.second = eventParams.Y;
	
	return point;
}
*/

// SLB
void TobiiTracker::showGaze(int g_Width, int g_Height) {

	if (tobiigazeon) {

		RECT winsize = getWindowInfo();
		float x_scale = (float) g_Width / winsize.right;
		float y_scale = (float) g_Height / winsize.top;

		float pointsize = 20;
		//float r = (float) (x_icon_size*innerSquarePercentage) / 200;
		float r = pointsize;
	
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_POINT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (int i=0; i<points.size(); i++) {

			float x = (points[i].first	- winsize.left) * x_scale;
			float y = (points[i].second	- winsize.bottom) * y_scale;
		
			static const double M_PI = 3.14159265358979323846264338327950288;
			static const double inc = M_PI / 8;
			static const double max = 2 * M_PI;
			glLineWidth(3.0);
			glBegin(GL_LINE_LOOP);
			for (double d=0; d<max; d+=inc) {
				glColor4f(1.0f, 0.0f, 0.0f, 0.3f/(points.size()-i)); // for multiple points
				glVertex2f(x + r*cos(d), y + r*sin(d));
			}
			glEnd();
			glLineWidth(1.0);

			glPointSize(pointsize); //*(1.5/(points.size()-i)));
			glBegin(GL_POINTS);
				glColor4f(0.0f, 0.0f, 1.0f, 0.2f/(points.size()-i));
				glVertex2f(x, y);
			glEnd();
			/*
			glPointSize(pointsize/2);
			glBegin(GL_POINTS);
				glColor4f(0.0f, 1.0f, 0.0f, 0.2f/(points.size()-i));
				glVertex2f(x, y);
			glEnd();
			*/
		}

	}
	
}

// SLB
void TobiiTracker::showInnerSquare(int g_Width, int g_Height, int text_height, int rows) {

	if (tobiishowinnersquare) {
		squareBounds(g_Width, g_Height, text_height);

		RECT winsize = getWindowInfo();
		float x_scale = (float) g_Width / winsize.right;
		float y_scale = (float) g_Height / winsize.top;
		int point_size = x_innerSquareOffset * 2 / x_scale;

		for (int r=0; r<rows; r++) {
			for (int c=0; c<rows; c++) {

				float x = (centers[r][c].first	- winsize.left) * x_scale;
				float y = (centers[r][c].second	- winsize.bottom) * y_scale;

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDisable(GL_POINT_SMOOTH);
				glPointSize(point_size);
				glBegin(GL_POINTS);
					glColor4f(0.0f, 0.0f, 1.0f, 0.2f);
					glVertex2f(x, y);
				glEnd();
			
			}
		}
	}

}

// SLB
void TobiiTracker::readConfiguration() {

	pugi::xml_document doc;
	//pugi::xml_parse_result result = doc.load_file("./configuration.xml");
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	
	if (result) {
		username				= doc.child("name").first_child().value();
		rows					= atoi(doc.child("row_num_interface").first_child().value());
		innerSquarePercentage	= atoi(doc.child("tobiiinnersquarepc").first_child().value());			// SLB
		tobiienabled			= (bool) atoi(doc.child("tobiienabled").first_child().value());			// SLB
		tobiigazeon				= (bool) atoi(doc.child("tobiigazeenabled").first_child().value());		// SLB
		tobiifiltered			= (bool) atoi(doc.child("tobiifiltered").first_child().value());		// SLB
		tobiiwindowcoord		= (bool) atoi(doc.child("tobiiwindowcoord").first_child().value());		// SLB
		tobiishowinnersquare	= (bool) atoi(doc.child("tobiishowinnersquare").first_child().value());	// SLB
	}
	else
		cout << "Could not find configuration file." << endl;
}

// SLB
void TobiiTracker::updateConfiguration() {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str());

	if (result) {

		pugi::xml_node node = doc.child("tobiiresultID");
		node.first_child().set_value(std::to_string(selected_IDs[0]).c_str());

		node = doc.child("tobiiresultIDs");
		pugi::xml_node children = node.child("resultID");
		while (children) {
			node.remove_child("resultID");
			children = node.child("resultID");
		}

		for (int i=0; i<array_data_structs.size(); i++) {
			children = node.append_child("resultID").append_child(pugi::node_pcdata);
			children.set_value(std::to_string(selected_IDs[i]).c_str());
		}

		node = doc.child("tobiiresultPC");
		node.first_child().set_value(std::to_string(focusPercent).c_str());

		node = doc.child("tobiiresultPCs");
		children = node.child("resultPC");
		while (children) {
			node.remove_child("resultPC");
			children = node.child("resultPC");
		}

		for (int i=0; i<array_data_structs.size(); i++) {
			children = node.append_child("resultPC").append_child(pugi::node_pcdata);
			children.set_value(std::to_string((float) selected_values[i] / (float) count_tot_valid_Points).c_str());
		}

		doc.save_file((configFilesRoot + "/configuration.xml").c_str());
	}
	else
		cout << "Could not find configuration file." << endl;

}