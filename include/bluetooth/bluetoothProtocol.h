/*
 * bluetoothProtocol.h
 *
 *  Created on: Feb 15, 2017
 *      Author: daniel
 */

#ifndef INCLUDE_BLUETOOTH_BLUETOOTHPROTOCOL_H_
#define INCLUDE_BLUETOOTH_BLUETOOTHPROTOCOL_H_


#include "cmd_def.h"
#include "uart.h"

#define TRACKING_DATA_HANDLE 8
#define OBJECT_ID_HANDLE 13

class bluetoothDevice;

typedef enum {
	state_disconnected,
	state_connected,
} states;

typedef enum {
	object_short_pointer,
	object_long_pointer,
	object_robot_pointer,
	object_suction
} tracked_objects;



void setBluetoothDevice(bluetoothDevice* device);
bluetoothDevice* getBluetoothDevice();

void changeConnectionStatus(states state);
void messageReceived(const struct ble_msg_attributes_value_evt_t * msg);

bluetoothDevice* bttDevice;

extern "C"{
	void print_raw_packet(struct ble_header *hdr, unsigned char *data);
	void send_api_packet(uint8 len1, uint8* data1, uint16 len2, uint8* data2);
	int read_api_packet(int timeout_ms);
	void advertiseBle();
}
#endif /* INCLUDE_BLUETOOTH_BLUETOOTHPROTOCOL_H_ */
