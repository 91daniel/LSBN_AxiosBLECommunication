/*
 * bluetoothDevice.cpp
 *
 *  Created on: Feb 15, 2017
 *      Author: daniel
 */
#include <mutex>
#include <string>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "bluetoothDevice.h"


bool bluetoothDevice::bleRunning = false;

void *bluetoothDevice::run_ble(void *ptr) {

	bleRunning = true;
	while (bleRunning) {
		read_api_packet(100 /* timeout */);
	}
	return NULL;
}


bluetoothDevice::bluetoothDevice() : uart_port("/dev/ttyACM0") {
}
bluetoothDevice::~bluetoothDevice() {}

bool bluetoothDevice::initializeDevice() {
	// Start communication with Bluetooth USB stick
	if (uart_open(uart_port)) {
		printf("ERROR: Unable to open serial port. Is the Bluetooth USB stick plugged in?\n");
		return 0;
	}

	// Reset dongle to get it into known state
	ble_cmd_system_reset(0);
	uart_close();
	do {
		usleep(500); // 0.5s
	} while (uart_open(uart_port));

	bglib_output = send_api_packet;

	//ble_cmd_connection_disconnect(0);

	std::cout << "UART open, advertise Bled112." << std::endl;
	advertiseBle();

	/* this variable is our reference to the second thread */
	pthread_t ble_thread;
	if (pthread_create(&ble_thread, NULL, bluetoothDevice::run_ble, NULL)) {
		fprintf(stderr, "ERROR: Unable to create thread\n");
		return 0;
	}

	return 1;

}
void bluetoothDevice::shutdownDevice() {}

bool bluetoothDevice::sendPose(uint8_t* poseData, uint8_t length) {
    std::lock_guard<std::mutex> lock(connection_status_mutex);
	if(this->connection_status == status_connected) {
		ble_cmd_attributes_send(0, TRACKING_DATA_HANDLE, length, poseData);
		return true;
	}
	return false;
}

tracked_objects bluetoothDevice::getCurrentTrackedObject() {
    std::lock_guard<std::mutex> lock(tracked_object_mutex);
	return tracked_object;
}

void bluetoothDevice::change_connection_status(status new_connection_status)
{
    std::lock_guard<std::mutex> lock(connection_status_mutex);
#ifdef _DEBUG
	printf("DEBUG: Connection status changed: %i --> %i\n", static_cast<int>(connection_status), static_cast<int>(new_connection_status));
#endif
	connection_status = new_connection_status;
}

void bluetoothDevice::change_tracked_object(tracked_objects new_tracked_object)
{
    std::lock_guard<std::mutex> lock(tracked_object_mutex);

#ifdef _DEBUG
	printf("New tracked object %i", static_cast<int>(new_tracked_object));
#endif
	tracked_object = new_tracked_object;
}
