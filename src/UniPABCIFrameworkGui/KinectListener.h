#ifndef KINECTLISTENER_H
#define KINECTLISTENER_H

#include <afxmt.h>
//#include <afxwin.h>

#define GAMEK_ROCK			3
#define GAMEK_PAPER			1
#define GAMEK_SCISSORS		2
#define GAMEK_READY			9
#define GAMEK_DISCONNECTED	8
#define GAMEK_MINMOVES		10


class KinectListener {

	public:
		KinectListener();
		void startAcquisition();
		void stopAcquisition();
		static int bufferToInt(char*);
		static UINT ListenNetwork(LPVOID);
		int getMove();
		bool isStartedKinect();
		void setStartedKinect(bool);
		CEvent& getMoveReceivedEvent();
		CEvent& getIsRockEvent();
		CEvent& getIsPaperEvent();
		CEvent& getIsScissorsEvent();

	private:
		static bool isListening;
		static bool _startedKinect;
		static int gameMove;
		static CEvent _moveReceived;
		static CEvent _isRock;
		static CEvent _isPaper;
		static CEvent _isScissors;

};


#endif