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
* Based on a previous work -P300 core- of (in alphabetical order):
* Rosario Misuraca, Walter Tranchina, Giuseppe Trubia.
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

#ifndef P300SIGNALACQUISITION_H
#define P300SIGNALACQUISITION_H

#include <afxmt.h>
#include <string>
#include <deque>
#include "ringbuffer.h"
#include "gUSBamp.h"

using std::string;
using std::deque;


class P300SignalAcquisition {
public:
	P300SignalAcquisition(LPSTR*, int, LPSTR, int, int, int, int);
	deque<LPSTR> GetCallSequence();					// Metodo di costruzione della sequenza di avvio dei dispositivi
	void OpenAndInitDevice();						// Metodo adibito al collegamento con i dispositivi BCI
	void StartAcquisition();						// Metodo di avvio del thread di acquisizione
	void setBufferSizeSeconds(int);
	void setSampleRateHz(int);
	void setNumberOfChannels(UCHAR);
	void setNumberOfScans(int);
	int getNumDevices();
	int getNumberOfScans();
	int getNumberOfChannels();
	void resetBuffer();
	void lockMutex();
	void unlockMutex();
	bool getBufferOverrun();
	void setBufferOverrun(bool);
	void CloseDevices();
	static void StopAcquisition();
	void StopdataAcquisition();
	CEvent& getNewDataAvailableEvent();
	bool ReadData(float*, int*, string*);
	static UINT DoAcquisition(LPVOID);	// Metodo avviato dal thread di acquisizione dei dati. Dapprima riempie la variabile BUFFERS[0][QUEUE_SIZE], quindi scrive tutto sul _buffer.
private:
	LPSTR _masterSerial; 						//id of the plugged device
	LPSTR* _slaveSerials;						//specify the serial numbers of the devices used as slaves
	int SLAVE_SERIALS_SIZE;
	static deque<HANDLE> _callSequenceHandles;			//stores the handles to the currently opened devices
	deque<LPSTR> callSequenceSerials;
	static bool _isRunning;							//flag that indicates if the thread is currently running
	static bool _bufferOverrun;						//flag indicating if an overrun occured at the application buffer
	int BUFFER_SIZE_SECONDS;					//the size of the application buffer in seconds
	int SAMPLE_RATE_HZ;							//the sample rate in Hz (see documentation of the g.USBamp API for details on this value and the NUMBER_OF_SCANS!)
	static UCHAR NUMBER_OF_CHANNELS;
	static int NUMBER_OF_SCANS;
	static CRingBuffer<float> _buffer;					//the application buffer where received data will be stored for each device
	static CMutex _bufferLock;							//mutex used to manage concurrent thread access to the application buffer
	const static int QUEUE_SIZE = 5;			//the number of GT_GetData calls that will be queued during acquisition to avoid loss of data
	UCHAR _channelsToAcquire[4];
	CHANNEL _bipolarSettings;					//don't use bipolar derivation (all values will be initialized to zero)
	UCHAR _mode;
	static CEvent _dataAcquisitionStopped;				//event that signals that data acquisition thread has been stopped
	static CEvent _newDataAvailable;					//event to avoid polling the application data buffer for new data
};

#endif

