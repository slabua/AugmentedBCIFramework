#ifndef INTERFACELISTENER_H
#define INTERFACELISTENER_H

#include <afxmt.h>

#define START_CALIBRATION 100
#define END_CALIBRATION -100
#define START_STIMULUS 200
#define END_STIMULUS -200
#define FLASH_RC 300
#define FLASH_S 400


class InterfaceListener {
	public:
		InterfaceListener();
		void startAcquisition();
		static int bufferToInt(char*);
		static UINT ListenNetwork(LPVOID);
		int getSymbolTag();
		void stopAcquisition();
		bool isStartedInterface();
		void setStartedInterface(bool);
		void setEndedStimulus();
		bool getEndedStimulus();
		CEvent& getSymbolTagEvent();
		CEvent& getStartedCalibrationEvent();
		CEvent& getCallNextStimulus();
		CEvent& getFinishCalibrationEvent();
	private:
		static bool endedStimulus;
		static bool is_listening;
		//static bool valueReady;
		static bool _startedOnlineP300Interface;
		static int symbolTag;
		bool waitForParameters;
		static CEvent _switchSimbolTag;
		static CEvent _startedCalibration;
		static CEvent _callNextStimulus;
		static CEvent _finishCalibration;
};


#endif