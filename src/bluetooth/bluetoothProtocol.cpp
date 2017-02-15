/*
 * bluetoothProtocol.c
 *
 *  Created on: Feb 15, 2017
 *      Author: daniel
 */

#include "bluetoothProtocol.h"
#include "bluetoothDevice.h"


void setBluetoothDevice(bluetoothDevice* device)
{
	bttDevice = device;
}

bluetoothDevice* getBluetoothDevice()
{
	return bttDevice;
}

void changeConnectionStatus(states state)
{
	bttDevice->change_connection_status(state);
}

void messageReceived(const struct ble_msg_attributes_value_evt_t * msg)
{
	if (msg->handle == OBJECT_ID_HANDLE)
	{
		uint16 offset = msg->offset;
		for (uint8 i = 0; i < msg->value.len; i++) // i should only take the value 0!
		{
			bttDevice->change_tracked_object(static_cast<tracked_objects>(msg->value.data[i]));
		}
	}
}


extern "C"{
void print_raw_packet(struct ble_header *hdr, unsigned char *data)
{
	printf("Incoming packet: ");
	int i;
	for (i = 0; i < sizeof(*hdr); i++) {
		printf("%02x ", ((unsigned char *)hdr)[i]);
	}
	for (i = 0; i < hdr->lolen; i++) {
		printf("%02x ", data[i]);
	}
	printf("\n");
}

/**
 * Send BGAPI packet using UART interface
 *
 * @param len1 Length of fixed portion of packet (always at least 4)
 * @param data1 Fixed-length portion of BGAPI packet (should always be <len1> bytes long)
 * @param len2 Length of variable portion of packet data payload (trailing uint8array or uint16array)
 * @param data2 Variable-length portion of data payload (should always be <len2> bytes long)
 */
void send_api_packet(uint8 len1, uint8* data1, uint16 len2, uint8* data2) {
#ifdef _DEBUG
	// display outgoing BGAPI packet
	print_raw_packet((struct ble_header *)data1, data2);
#endif

	// transmit complete packet via UART
	if (uart_tx(len1, data1) || uart_tx(len2, data2)) {
		// uart_tx returns non-zero on failure
		printf("ERROR: Writing to serial port failed\n");
	}
}

/**
 * Receive BGAPI packet using UART interface
 *
 * @param timeout_ms Milliseconds to wait before timing out on the UART RX operation
 */
int read_api_packet(int timeout_ms) {
	unsigned char data[256]; // enough for BLE
	struct ble_header hdr;
	int r;

	r = uart_rx(sizeof(hdr), (unsigned char *) &hdr, timeout_ms);
	if (!r) {
		return -1; // timeout
	} else if (r < 0) {
		printf("ERROR: Reading header failed. Error code:%d\n", r);
		return 1;
	}

	if (hdr.lolen) {
		r = uart_rx(hdr.lolen, data, timeout_ms);
		if (r <= 0) {
			printf("ERROR: Reading data failed. Error code:%d\n", r);
			return 1;
		}
	}

	// use BGLib function to create correct ble_msg structure based on the header
	// (header contains command class/ID info, used to identify the right structure to apply)
	const struct ble_msg *msg = ble_get_msg_hdr(hdr);

#ifdef DEBUG
	// display incoming BGAPI packet
	print_raw_packet(&hdr, data);
#endif

	if (!msg) {
		printf("ERROR: Unknown message received\n");
	}

	// call the appropriate handler function with any payload data
	// (this is what triggers the ble_evt_* and ble_rsp_* functions)
	msg->handler(data);

	return 0;
}

void advertiseBle() {
	ble_cmd_gap_set_adv_parameters(32 /* adv_interval_min */, 48 /* adv_interval_max */, 7 /* adv_channels */);
	ble_cmd_gap_set_mode(gap_general_discoverable, gap_undirected_connectable);

}

void ble_rsp_system_hello(const void* nul)
{
	printf("Device responded to hello message and is functional.\n");
}

void ble_evt_connection_status(const struct ble_msg_connection_status_evt_t *msg)
{
	// New connection
	if (msg->flags & connection_connected) {
		changeConnectionStatus(state_connected);
		printf("Connected.\n");
	}
	else
	{
		printf("Not connected.\n");
	}
}

void ble_evt_connection_disconnected(
	const struct ble_msg_connection_disconnected_evt_t *msg) {
	changeConnectionStatus(state_disconnected);
	printf("Connection terminated, continue advertising.\n");
	advertiseBle();
}

void ble_evt_attributes_value(const struct ble_msg_attributes_value_evt_t * msg)
{
	printf("Local attribute value was written by a remote device.\n");
	messageReceived(msg);
}
}

