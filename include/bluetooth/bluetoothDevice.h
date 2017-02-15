/*
 * bluetoothDevice.h
 *
 *  Created on: Feb 15, 2017
 *      Author: daniel
 */

#ifndef BLUETOOTHDEVICE_H_
#define BLUETOOTHDEVICE_H_

#include <connection_status.h>
#include <pthread.h>
#include <mutex>
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
	tracked_objects getCurrentTrackedObject();

	void change_connection_status(status new_status);
	void change_tracked_object(tracked_objects new_tracked_object);
private:

	char * uart_port;
	std::mutex connection_status_mutex;
	status connection_status = status_disconnected;
	std::mutex tracked_object_mutex;
	tracked_objects tracked_object = object_short_pointer;

	void changeConnectionStatus(bool isNowConnected);

	static bool bleRunning;
	static void *run_ble(void *ptr);

};



#endif /* BLUETOOTHDEVICE_H_ */

