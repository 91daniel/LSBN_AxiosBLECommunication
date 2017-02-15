/*
 * bluetoothDevice.h
 *
 *  Created on: Feb 15, 2017
 *      Author: daniel
 */

#ifndef BLUETOOTHDEVICE_H_
#define BLUETOOTHDEVICE_H_

#include <pthread.h>

#include <stdint.h>

#include "cmd_def.h"
#include "uart.h"
#include "bluetoothProtocol.h"

class bluetoothDevice {
public:
	bluetoothDevice();
	~bluetoothDevice();

	bool initializeDevice();
	void shutdownDevice();

	bool sendPose(uint8_t* poseData, uint8_t length);
	tracked_objects currentTrackedObject();

	int lock();

	void change_connection_status(states new_state);
	void change_tracked_object(tracked_objects new_tracked_object);

	static void *run_ble(void *ptr);

	bool isConnected;

private:
	//int lastTrackedObjectId;
	char * uart_port;
	static bool bleRunning;
	bool read();
	bool write();
	void changeConnectionStatus(bool isNowConnected);

	states connection_state = state_disconnected;
	tracked_objects tracked_object = object_short_pointer;

};



#endif /* BLUETOOTHDEVICE_H_ */

