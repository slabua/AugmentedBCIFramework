#include "P300SignalAcquisition.h"
#include <iostream>
#include "rootpaths.h" // SLB

using std::cout;
using std::endl;

deque<HANDLE> P300SignalAcquisition::_callSequenceHandles;			//stores the handles to the currently opened devices
bool P300SignalAcquisition::_isRunning;							//flag that indicates if the thread is currently running
bool P300SignalAcquisition::_bufferOverrun;						//flag indicating if an overrun occured at the application buffer
UCHAR P300SignalAcquisition::NUMBER_OF_CHANNELS;
int P300SignalAcquisition::NUMBER_OF_SCANS;		
CRingBuffer<float> P300SignalAcquisition::_buffer;					//the application buffer where received data will be stored for each device
CMutex P300SignalAcquisition::_bufferLock;							//mutex used to manage concurrent thread access to the application buffer
CEvent P300SignalAcquisition::_dataAcquisitionStopped;				//event that signals that data acquisition thread has been stopped
CEvent P300SignalAcquisition::_newDataAvailable;	


P300SignalAcquisition::P300SignalAcquisition(LPSTR* slaveDevices, int slaveDevicesSize, LPSTR masterDevice, int buf_size, int rate, int chann, int scan) {
	
	//for (int i=0; i<4; i++)
	for (int i=0; i<chann; i++) // SLB
		_channelsToAcquire[i] = i+1;

	_masterSerial = masterDevice;	
	_slaveSerials = slaveDevices;
	SLAVE_SERIALS_SIZE = slaveDevicesSize;
	_mode = M_NORMAL;
	_isRunning = false;
	_bufferOverrun = false;
	callSequenceSerials = GetCallSequence();
	NUMBER_OF_CHANNELS = chann;
	NUMBER_OF_SCANS = scan;
	BUFFER_SIZE_SECONDS = buf_size;
	SAMPLE_RATE_HZ = rate;
}

deque<LPSTR> P300SignalAcquisition::GetCallSequence() {
	deque<LPSTR> callSequence;

	//enqueue the slave devices at the beginning of the list
	for (int i=0; i<SLAVE_SERIALS_SIZE; i++)
		callSequence.push_back(_slaveSerials[i]);

	//add the master device at the end of the list
	callSequence.push_back(_masterSerial);

	return callSequence;
}

void P300SignalAcquisition::OpenAndInitDevice() {

	REF _commonReference	= { FALSE, FALSE, FALSE, FALSE };	//don't connect groups to common reference because connected to g.gammabox
	GND _commonGround		= { FALSE, FALSE, FALSE, FALSE };	//don't connect groups to common ground because connected to g.gammabox

	try {
		for (deque<LPSTR>::iterator serialNumber = callSequenceSerials.begin(); serialNumber != callSequenceSerials.end(); serialNumber++) {
			//open the device
			HANDLE hDevice = GT_OpenDeviceEx(*serialNumber);
			//printFilterInformation();
			//printNotchInformation();
			if (hDevice == NULL)
				throw string("Error on GT_OpenDeviceEx: Couldn't open device ").append(*serialNumber);

			//add the device handle to the list of opened devices
			_callSequenceHandles.push_back(hDevice);

			//set the channels from that data should be acquired
			if (!GT_SetChannels(hDevice, _channelsToAcquire, NUMBER_OF_CHANNELS))
				throw string("Error on GT_SetChannels: Couldn't set channels to acquire for device ").append(*serialNumber);

			//set the sample rate
			if (!GT_SetSampleRate(hDevice, SAMPLE_RATE_HZ))
				throw string("Error on GT_SetSampleRate: Couldn't set sample rate for device ").append(*serialNumber);

			//disable the trigger line
			if (!GT_EnableTriggerLine(hDevice, false))
				throw string("Error on GT_EnableTriggerLine: Couldn't disable trigger line for device ").append(*serialNumber);

			//set the number of scans that should be received simultaneously
			if (!GT_SetBufferSize(hDevice, NUMBER_OF_SCANS))
				throw string("Error on GT_SetBufferSize: Couldn't set the buffer size for device ").append(*serialNumber);

			//don't use bandpass and notch for each channel
			for (int i=0; i<NUMBER_OF_CHANNELS; i++) {
				//don't use a bandpass filter for any channel
				if (!GT_SetBandPass(hDevice, _channelsToAcquire[i], 51))
					throw string("Error on GT_SetBandPass: Couldn't set no bandpass filter for device ").append(*serialNumber);

				//don't use a notch filter for any channel
				if (!GT_SetNotch(hDevice, _channelsToAcquire[i], 2))
					throw string("Error on GT_SetNotch: Couldn't set no notch filter for device ").append(*serialNumber);
			}

			//determine master device as the last device in the list
			bool isSlave = (*serialNumber != callSequenceSerials.back());

			//set slave/master mode of the device
			if (!GT_SetSlave(hDevice, isSlave))
				throw string("Error on GT_SetSlave: Couldn't set slave/master mode for device ").append(*serialNumber);

			//disable shortcut function
			if (!GT_EnableSC(hDevice, false))
				throw string("Error on GT_EnableSC: Couldn't disable shortcut function for device ").append(*serialNumber);

			//set unipolar derivation
			_bipolarSettings.Channel1 = 0;
			_bipolarSettings.Channel2 = 0;
			_bipolarSettings.Channel3 = 0;
			_bipolarSettings.Channel4 = 0;
			_bipolarSettings.Channel5 = 0;
			_bipolarSettings.Channel6 = 0;
			_bipolarSettings.Channel7 = 0;
			_bipolarSettings.Channel8 = 0;
			_bipolarSettings.Channel9 = 0;
			_bipolarSettings.Channel10 = 0;
			_bipolarSettings.Channel11 = 0;
			_bipolarSettings.Channel12 = 0;
			_bipolarSettings.Channel13 = 0;
			_bipolarSettings.Channel14 = 0;
			_bipolarSettings.Channel15 = 0;
			_bipolarSettings.Channel16 = 0;

			if (!GT_SetBipolar(hDevice, _bipolarSettings))
				throw string("Error on GT_SetBipolar: Couldn't set unipolar derivation for device ").append(*serialNumber);

			if (_mode == M_COUNTER)
			if (!GT_SetMode(hDevice, M_NORMAL))
				throw string("Error on GT_SetMode: Couldn't set mode M_NORMAL (before mode M_COUNTER) for device ").append(*serialNumber);

			//set acquisition mode
			if (!GT_SetMode(hDevice, _mode))
				throw string("Error on GT_SetMode: Couldn't set mode for device ").append(*serialNumber);

			//for g.USBamp devices set common ground and common reference
			if (strncmp(*serialNumber, "U", 1) == 0 && (_mode == M_NORMAL || _mode == M_COUNTER)) {
				//don't connect the 4 groups to common reference
				if (!GT_SetReference(hDevice, _commonReference))
					throw string("Error on GT_SetReference: Couldn't set common reference for device ").append(*serialNumber);

				//don't connect the 4 groups to common ground
				if (!GT_SetGround(hDevice, _commonGround))
					throw string("Error on GT_SetGround: Couldn't set common ground for device ").append(*serialNumber);
			}

			printf("\tg.USBamp %s initialized as %s (#%d in the call sequence)!\n", *serialNumber, (isSlave) ? "slave" : "master", _callSequenceHandles.size());
		}

	}
	catch (string& exception)	{
		//in case an exception occured, close all opened devices...
		while (!_callSequenceHandles.empty()) {
			GT_CloseDevice(&_callSequenceHandles.front());
			_callSequenceHandles.pop_front();
		}

		//...and rethrow the exception to notify the caller of this method about the error
		throw exception;
	}
}

//closes each opened device and removes it from the call sequence
void P300SignalAcquisition::CloseDevices() {
	while (!_callSequenceHandles.empty()) {
		//closes each opened device and removes it from the call sequence
		BOOL ret = GT_CloseDevice(&_callSequenceHandles.front());
		_callSequenceHandles.pop_front();
	}
}

void P300SignalAcquisition::setBufferSizeSeconds(int sec) {

	BUFFER_SIZE_SECONDS = sec;
}

void P300SignalAcquisition::setSampleRateHz(int rate) {

	SAMPLE_RATE_HZ = rate;
}

void P300SignalAcquisition::setNumberOfChannels(UCHAR num) {

	NUMBER_OF_CHANNELS = num;
}

void P300SignalAcquisition::setNumberOfScans(int scans) {

	NUMBER_OF_SCANS = scans;
}

int P300SignalAcquisition::getNumDevices() {

	return _callSequenceHandles.size();
}

int P300SignalAcquisition::getNumberOfChannels() {

	return NUMBER_OF_CHANNELS;
}

int P300SignalAcquisition::getNumberOfScans() {

	return NUMBER_OF_SCANS;
}

void P300SignalAcquisition::StartAcquisition() {

	_isRunning = true;
	_bufferOverrun = false;

	//give main process (the data processing thread) high priority
	HANDLE hProcess = GetCurrentProcess();
	SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS);

	//initialize application data buffer to the specified number of seconds
	_buffer.Initialize(BUFFER_SIZE_SECONDS * SAMPLE_RATE_HZ * NUMBER_OF_CHANNELS * (unsigned int)_callSequenceHandles.size());

	//reset event
	_dataAcquisitionStopped.ResetEvent();

}

//Stops the data acquisition thread
void P300SignalAcquisition::StopAcquisition() {

	//tell thread to stop data acquisition
	_isRunning = false;
	//is_listening = false;					-----------------------------------------------------------------------
	//wait until the thread has stopped data acquisition
	//DWORD ret = WaitForSingleObject(_dataAcquisitionStopped.m_hObject, 60000);
	//reset the main process (data processing thread) to normal priority
	HANDLE hProcess = GetCurrentProcess();
	SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS);
	
}

UINT P300SignalAcquisition::DoAcquisition(LPVOID param) {
	
	int queueIndex = 0;
	int nPoints = NUMBER_OF_SCANS * NUMBER_OF_CHANNELS;
	DWORD bufferSizeBytes = HEADER_SIZE + nPoints * sizeof(float);
	int numDevices = (int)_callSequenceHandles.size();
	DWORD numBytesReceived = 0;

	//create the temporary data buffers (the device will write data into those)
	BYTE*** buffers = new BYTE**[numDevices];
	OVERLAPPED** overlapped = new OVERLAPPED*[numDevices];

	__try{
		//for each device create a number of QUEUE_SIZE data buffers

		buffers[0] = new BYTE*[QUEUE_SIZE];
		overlapped[0] = new OVERLAPPED[QUEUE_SIZE];

		//for each data buffer allocate a number of bufferSizeBytes bytes
		for (queueIndex = 0; queueIndex<QUEUE_SIZE; queueIndex++)	{
			buffers[0][queueIndex] = new BYTE[bufferSizeBytes];
			memset(&(overlapped[0][queueIndex]), 0, sizeof(OVERLAPPED));

			//create a windows event handle that will be signalled when new data from the device has been received for each data buffer
			overlapped[0][queueIndex].hEvent = CreateEvent(NULL, false, false, NULL);
		}
		//start the devices (master device must be started at last)

		HANDLE hDevice = _callSequenceHandles[0];

		if (!GT_Start(hDevice))	{
			//throw string("Error on GT_Start: Couldn't start data acquisition of device.");
			cout << "\tError on GT_Start: Couldn't start data acquisition of device.\n";
			return 0;
		}

		//queue-up the first batch of transfer requests
		for (queueIndex = 0; queueIndex<QUEUE_SIZE; queueIndex++) {
			if (!GT_GetData(hDevice, buffers[0][queueIndex], bufferSizeBytes, &overlapped[0][queueIndex])) {
				cout << "\tError on GT_GetData.\n";
				return 0;
			}
		}

		queueIndex = 0;

		//continouos data acquisition
		while (_isRunning) {
			//receive data from each device

			HANDLE hDevice = _callSequenceHandles[0];

			//wait for notification from the system telling that new data is available

			if (WaitForSingleObject(overlapped[0][queueIndex].hEvent, 1000) == WAIT_TIMEOUT) {
				//throw string("Error on data transfer: timeout occured.");
				cout << "Error on data transfer: timeout occured." << "\n";
				return 0;
			}

			//get number of received bytes...
			GetOverlappedResult(hDevice, &overlapped[0][queueIndex], &numBytesReceived, false);

			//...and check if we lost something (number of received bytes must be equal to the previously allocated buffer size)
			if (numBytesReceived != bufferSizeBytes) {
				//throw string("Error on data transfer: samples lost.");
				cout << "Error on data transfer: samples lost." << "\n";
				return 0;
			}

			//to store the received data into the application data buffer at once, lock it
			_bufferLock.Lock();

			__try{
				//if we are going to overrun on writing the received data into the buffer, set the appropriate flag; the reading thread will handle the overrun
				_bufferOverrun = (_buffer.GetFreeSize() < (nPoints * numDevices));

				//store received data from each device in the correct order (that is scan-wise, where one scan includes all channels of all devices) ignoring the header
				for (int scanIndex = 0; scanIndex < NUMBER_OF_SCANS; scanIndex++) {
					//	float * destBuffer=0;

					//	_buffer.Read(destBuffer, 10);
					//cout<<cont++<<" "<< &destBuffer<<endl;	
					_buffer.Write((float*)(buffers[0][queueIndex] + scanIndex * NUMBER_OF_CHANNELS * sizeof(float)+HEADER_SIZE), NUMBER_OF_CHANNELS);
					//cout << "HO SCRITTO SUL _BUFFER" << endl;
				}
			}
			__finally{
				//release the previously acquired lock
				_bufferLock.Unlock();
			}

			//add new GetData call to the queue replacing the currently received one

			if (!GT_GetData(_callSequenceHandles[0], buffers[0][queueIndex], bufferSizeBytes, &overlapped[0][queueIndex])) {
				cout << "\tError on GT_GetData.\n";
				return 0;
			}

			//signal processing (main) thread that new data is available
			_newDataAvailable.SetEvent();

			//increment circular queueIndex to process the next queue at the next loop repitition (on overrun start at index 0 again)

			queueIndex = (queueIndex + 1) % QUEUE_SIZE;
		}
	}
	__finally {
		cout << "Stopping devices and cleaning up..." << "\n";

		//clean up allocated resources for each device

		HANDLE hDevice = _callSequenceHandles[0];

		//clean up allocated resources for each queue per device
		for (int j = 0; j<QUEUE_SIZE; j++) {
			WaitForSingleObject(overlapped[0][queueIndex].hEvent, 1000);
			CloseHandle(overlapped[0][queueIndex].hEvent);

			delete[] buffers[0][queueIndex];

			//increment queue index
			queueIndex = (queueIndex + 1) % QUEUE_SIZE;
		}

		//stop device
		cout << "done" << endl;
		GT_Stop(hDevice);

		//reset device
		GT_ResetTransfer(hDevice);

		delete[] overlapped[0];
		delete[] buffers[0];

		delete[] buffers;
		delete[] overlapped;

		//reset _isRunning flag
		_isRunning = false;
		//is_listening = false;				
		//signal event
		//_dataAcquisitionStopped.SetEvent();
		//StopAcquisition();
		//end thread
//		AfxEndThread(0xdead);
		//closesocket(s);
		//WSACleanup();

		
	}
	return 0xdead;
}

void P300SignalAcquisition::lockMutex() {

	_bufferLock.Lock();
}

void P300SignalAcquisition::unlockMutex() {

	_bufferLock.Unlock();
}

bool P300SignalAcquisition::getBufferOverrun() {

	return _bufferOverrun;
}

void P300SignalAcquisition::setBufferOverrun(bool flag) {

	_bufferOverrun = flag;
}

bool P300SignalAcquisition::ReadData(float* destBuffer, int *errorCode, string *errorMessage) {

	int validPoints = getNumberOfChannels() * getNumberOfScans() * getNumDevices();
	//wait until requested amount of data is ready
	if (_buffer.GetSize() < validPoints)	{
		*errorCode = 2;
		*errorMessage = "Not enough data available";
		return false;
	}

	//acquire lock on the application buffer for reading
	_bufferLock.Lock();
	
	__try{
		//if buffer run over report error and reset buffer
		if (_bufferOverrun)	{
			cout << "bufferReset" << endl;
			_buffer.Reset();
			*errorCode = 1;
			*errorMessage = "Error on reading data from the application data buffer: buffer overrun.";
			_bufferOverrun = false;
			return false;
		}

		//copy the data from the application buffer into the destination buffer
		_buffer.Read(destBuffer, 1000);

	}
	__finally{
		_bufferLock.Unlock();
	}
	
	*errorCode = 0;
	*errorMessage = "No error occured.";
	return true;
}

void P300SignalAcquisition::resetBuffer() {

	_buffer.Reset();
}

CEvent& P300SignalAcquisition::getNewDataAvailableEvent() {

	return _newDataAvailable;
}