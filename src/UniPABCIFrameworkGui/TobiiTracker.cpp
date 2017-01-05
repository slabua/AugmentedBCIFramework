#include "TobiiTracker.h"
#include <iostream>
#include <assert.h>
#include "rootpaths.h" // SLB
#include "EyeX.h"
#include "pugixml.hpp"


// ID of the global interactor that provides our data stream; must be unique within the application.
static const TX_STRING InteractorId = "Twilight Sparkle Gui";

// Global variables
static TX_HANDLE g_hGlobalInteractorSnapshot = TX_EMPTY_HANDLE;
static TX_CONTEXTHANDLE hContext = TX_EMPTY_HANDLE;

//TX_GAZEPOINTDATAEVENTPARAMS eventParams; // SLB

#pragma comment (lib, "Tobii.EyeX.Client.lib")

using namespace std;

// SLB
bool is_running		= false;
bool userTracked	= false;
bool userLooking	= false;

bool tobiiconnected = false;

//pair <int, int> screenBounds;
TX_SIZE2 screenBounds;


TobiiTracker::TobiiTracker()
	: tobiienabled(false)
{
}

TobiiTracker::~TobiiTracker()
{
	Disable(); // SLB
}

void TX_CALLCONVENTION OnStateReceived(TX_HANDLE hStateBag) {
//void OnStateReceived(TX_HANDLE hStateBag) {
	
	TX_BOOL success;
	TX_INTEGER eyeTrackingState;
	TX_SIZE2 displaySize;
	//TX_SIZE2 screenBounds;
	TX_INTEGER presenceData;
	TX_INTEGER gazeTracking;

	//if (is_running) {
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
		success = (txGetStateValueAsInteger(hStateBag, TX_STATEPATH_GAZETRACKING, &gazeTracking) == TX_RESULT_OK);
		if (success) {
			userTracked = (gazeTracking == TX_GAZETRACKING_GAZETRACKED);
			printf("User's eye-gaze is %s\n", gazeTracking == TX_GAZETRACKING_GAZETRACKED ? "tracked" : "NOT tracked");
		}
		
	//}
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
	
	TX_GAZEPOINTDATAPARAMS params = { TX_GAZEPOINTDATAMODE_UNFILTERED }; // SLB
	success = txCreateGlobalInteractorSnapshot(hContext, InteractorId, &g_hGlobalInteractorSnapshot, &hInteractor) == TX_RESULT_OK;
	success &= txCreateGazePointDataBehavior(hInteractor, &params) == TX_RESULT_OK;

	txReleaseObject(&hInteractor);

	return success;
	
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
			
			//eventParams.Y = screenBounds.second - eventParams.Y; // SLB sets the screen origin to the bottom-left corner
			eventParams.Y = screenBounds.Height - eventParams.Y; // SLB sets the screen origin to the bottom-left corner
				
			//cout << eventParams.X << "\t" << eventParams.Y << endl; // SLB

			// SLB Secondary monitor must be positioned to the right of the main monitor and it must have the same height
			if ((eventParams.X-GetSystemMetrics(SM_CXSCREEN) > 0 && eventParams.X-GetSystemMetrics(SM_CXSCREEN) < screenBounds.Width) &&
				(eventParams.Y > 0 && eventParams.Y < screenBounds.Height)) {
					userLooking = true;
					//cout << "User is looking" << endl;
			}
			else {
				userLooking = false;
				//cout << "User is NOT looking" << endl;
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
		
		//screenBounds = getScreenSize();

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

}

// SLB
void TobiiTracker::setRunning(bool flag) {
	
	is_running = flag;
}

// SLB
bool TobiiTracker::getUserTracked() {

	//cout << ((userTracked && userLooking) ? "LOOKING" : "NOT LOOKING") << endl; 
	return userTracked && userLooking;
	//return userTracked; // && userLooking;
}

/* SLB - General/MFC/Use Standard Windows Libraries
pair <int, int> TobiiTracker::getScreenSize() {

	RECT screenDimensions;
	pair <int, int> screenBounds;

	GetWindowRect(GetDesktopWindow(), &screenDimensions);

	screenBounds.first = screenDimensions.right;	// Screen Width
	screenBounds.second = screenDimensions.bottom;	// Screen Height
	
	//cout << screenBounds.first << "\t" << screenBounds.second << endl;

	return screenBounds;
}
*/

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
/*
pair <float, float> TobiiTracker::getEventParams() {
	
	pair <float, float> point;
	point.first = eventParams.X;
	point.second = eventParams.Y;
	
	return point;
}
*/

// SLB
void TobiiTracker::readConfiguration() {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file((configFilesRoot + "/configuration.xml").c_str()); // SLB
	
	if (result) {
		username		= doc.child("name").first_child().value();
		tobiienabled	= (bool) atoi(doc.child("tobiienabled").first_child().value());			// SLB
	}
	else
		cout << "Could not find configuration file." << endl;
}
